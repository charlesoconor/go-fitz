#ifndef QUAD_H
#define QUAD_H

#include "mupdf/fitz.h"

#ifndef FLT_EPSILON
  #define FLT_EPSILON 1e-5
#endif

static int skip_quad_corrections = 0;
static int small_glyph_heights = 0;

static const float
font_ascender(fz_context *ctx, fz_font *font)
{
    if (skip_quad_corrections) {
        return 0.8f;
    }
    return fz_font_ascender(ctx, font);
}

static const float
font_descender(fz_context *ctx, fz_font *font)
{
    if (skip_quad_corrections) {
        return -0.2f;
    }
    return fz_font_descender(ctx, font);
}

//---------------------------------------------------------------------------
// APPEND non-ascii runes in unicode escape format to fz_buffer
//---------------------------------------------------------------------------
void append_rune(fz_context *ctx, fz_buffer *buff, int ch)
{
    if ((ch >= 32 && ch <= 255) || ch == 10) {
        fz_append_byte(ctx, buff, ch);
    } else if (ch <= 0xffff) {  // 4 hex digits
        fz_append_printf(ctx, buff, "\\u%04x", ch);
    } else {  // 8 hex digits
        fz_append_printf(ctx, buff, "\\U%08x", ch);
    }
}


// re-compute char quad if ascender/descender values make no sense
static fz_quad
char_quad(fz_context *ctx, fz_stext_line *line, fz_stext_char *ch)
{
    if (skip_quad_corrections) {  // no special handling
        return ch->quad;
    }
    if (line->wmode) {  // never touch vertical write mode
        return ch->quad;
    }
    fz_font *font = ch->font;
    float asc = font_ascender(ctx, font);
    float dsc = font_descender(ctx, font);
    float c, s, fsize = ch->size;
    float asc_dsc = asc - dsc + FLT_EPSILON;
    if (asc_dsc >= 1 && small_glyph_heights == 0) {  // no problem
       return ch->quad;
    }
    if (asc < 1e-3) {  // probably Tesseract glyphless font
        dsc = -0.1f;
        asc = 0.9f;
        asc_dsc = 1.0f;
    }

    if (small_glyph_heights || asc_dsc < 1) {
        dsc = dsc / asc_dsc;
        asc = asc / asc_dsc;
    }
    asc_dsc = asc - dsc;
    asc = asc * fsize / asc_dsc;
    dsc = dsc * fsize / asc_dsc;

    /* ------------------------------
    Re-compute quad with the adjusted ascender / descender values:
    Move ch->origin to (0,0) and de-rotate quad, then adjust the corners,
    re-rotate and move back to ch->origin location.
    ------------------------------ */
    fz_matrix trm1, trm2, xlate1, xlate2;
    fz_quad quad;
    c = line->dir.x;  // cosine
    s = line->dir.y;  // sine
    trm1 = fz_make_matrix(c, -s, s, c, 0, 0);  // derotate
    trm2 = fz_make_matrix(c, s, -s, c, 0, 0);  // rotate
    if (c == -1) {  // left-right flip
        trm1.d = 1;
        trm2.d = 1;
    }
    xlate1 = fz_make_matrix(1, 0, 0, 1, -ch->origin.x, -ch->origin.y);
    xlate2 = fz_make_matrix(1, 0, 0, 1, ch->origin.x, ch->origin.y);

    quad = fz_transform_quad(ch->quad, xlate1);  // move origin to (0,0)
    quad = fz_transform_quad(quad, trm1);  // de-rotate corners

    // adjust vertical coordinates
    if (c == 1 && quad.ul.y > 0) {  // up-down flip
        quad.ul.y = asc;
        quad.ur.y = asc;
        quad.ll.y = dsc;
        quad.lr.y = dsc;
    } else {
        quad.ul.y = -asc;
        quad.ur.y = -asc;
        quad.ll.y = -dsc;
        quad.lr.y = -dsc;
    }

    // adjust horizontal coordinates that are too crazy:
    // (1) left x must be >= 0
    // (2) if bbox width is 0, lookup char advance in font.
    if (quad.ll.x < 0) {
        quad.ll.x = 0;
        quad.ul.x = 0;
    }
    float cwidth = quad.lr.x - quad.ll.x;
    if (cwidth < FLT_EPSILON) {
        int glyph = fz_encode_character(ctx, font, ch->c);
        if (glyph) {
            float fwidth = fz_advance_glyph(ctx, font, glyph, line->wmode);
            quad.lr.x = quad.ll.x + fwidth * fsize;
            quad.ur.x = quad.lr.x;
        }
    }

    quad = fz_transform_quad(quad, trm2);  // rotate back
    quad = fz_transform_quad(quad, xlate2);  // translate back
    return quad;
}

// Go doesn't know how to deal with C union types so doing this logic here.
static fz_stext_line*
first_text_line_from_stest_block(fz_stext_block *line)
{
    return line->u.t.first_line;
}

#endif /* !QUAD_H */
