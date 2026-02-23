SRCDIR = ../src
INCLDIR = ../include

WINPKGSDIR = $(HOME)/develop/win-pkgs
WINPKGINCLDIR = $(WINPKGSDIR)/include
WINPKGLIBDIR = $(WINPKGSDIR)/lib

CC = x86_64-w64-mingw32-gcc
COPTS = -O3 -g0 -Wall -I $(INCLDIR) -I $(WINPKGINCLDIR) -L . -L $(WINPKGLIBDIR)

all: avl.lib test-avl.exe

avl.obj: $(SRCDIR)/avl.c $(INCLDIR)/avl.h
	$(CC) $(COPTS) -o avl.obj -c $(SRCDIR)/avl.c

test-avl.exe: test-avl.obj avl.obj
	$(CC) $(COPTS) -o test-avl test-avl.obj avl.obj

test-avl.obj: $(SRCDIR)/test-avl.c $(INCLDIR)/avl.h
	$(CC) $(COPTS) -o test-avl.obj -c $(SRCDIR)/test-avl.c

libavl.a: avl.obj
	ar rcs libavl.a avl.obj

avl.lib: libavl.a
	@cp libavl.a avl.lib
	ranlib avl.lib

clean:
	@rm -f *.a *.obj *.lib *.exe

install: avl.lib
	@cp avl.lib $(WINPKGLIBDIR)
	@cp ../include/avl.h $(WINPKGINCLDIR)

uninstall:
	@rm -f $(WINPKGLIBDIR)/avl.lib
	@rm -f $(WINPKGINCLDIR)/avl.h

