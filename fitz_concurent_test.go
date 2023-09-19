package fitz_test

import (
	"path/filepath"
	"testing"

	"github.com/gen2brain/go-fitz"
)

func TestCreateLock(t *testing.T) {
	doc, err := fitz.New(filepath.Join("testdata", "test.pdf"), fitz.Concurency)
	if err != nil {
		t.Error(err)
	}
	defer doc.Close()
}
