package locks

/*
#include <mupdf/fitz.h>
*/
import "C"

import (
	"sync"
	"unsafe"
)

type CLock struct {
	// Need to hold onto a reference to the go locks so
	// they won't be gc'ed before this object's lifetime ends.
	locks []sync.Mutex

	ctx *C.struct_fz_locks_context
}

func New(numLocks int) *CLock {
	locks := make([]sync.Mutex, numLocks)

	ctx := C.malloc(C.size_t(C.struct_fz_locks_context))
	ctx.user = locks
	ctx.lock = Lock
	ctx.unlock = Unlock

	return &CLock{
		locks: locks,
		ctx:   ctx,
	}

}

func (l *CLock) Close() {
	C.free(unsafe.Pointer(l.ctx))
	l.ctx = nil
	l.locks = nil
}

func Lock(l interface{}, lock int) {
	l.(*CLock).locs[lock].Lock()
}

func Lock(l interface{}, lock int) {
	l.(*CLock).locs[lock].Unlock()
}
