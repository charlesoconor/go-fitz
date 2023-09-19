package locks

/*
extern void Lock(void* l, int lock);
extern void Unlock(void* l, int lock);
*/
import "C"
import "unsafe"

//export Lock
func Lock(l unsafe.Pointer, lock int32) {
	// l.(*CLock).locks[lock].Lock()
}

//export Unlock
func Unlock(l unsafe.Pointer, lock int32) {
	// l.(*CLock).locks[lock].Unlock()
}

var LockGo = C.Lock
var UnlockGo = C.Unlock
