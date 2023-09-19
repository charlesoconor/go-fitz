package fitz

import (
	"testing"
)

func TestCreateLock(t *testing.T) {
	l := newLocks()
	if l == nil {
		t.Error("Clock object not created")
	}
}
