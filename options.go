package fitz

type Options struct {
	concurency bool
}

type Option func(*Options)

// If this option is passed we will create N locks for the underlying
// mupdf ctx. This will allow us to run more threads at once touching
// the object but at the cost of more memory since there are some heavy
// copies.
func Concurency(opt *Options) {
	opt.concurency = true
}

func buildOpts(options ...Option) Options {
	opt := Options{}
	for _, fn := range options {
		fn(&opt)
	}
	return opt
}
