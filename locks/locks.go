package fitz

/*
extern void lockGo(void *user, int lock);
extern void unlockGo(void *user, int lock);
*/
import "C"
import "unsafe"

// export lockGo
func Lock(l unsafe.Pointer, lock int) {
	// l.(*CLock).locks[lock].Lock()
}

// export unlockGo
func Unlock(l unsafe.Pointer, lock int) {
	// l.(*CLock).locks[lock].Unlock()
}

var LockGo = C.lockGo
var UnlockGo = C.unlockGo
