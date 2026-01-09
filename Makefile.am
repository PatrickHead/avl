AM_CPPFLAGS = -I $(srcdir)/include

AM_CFLAGS = -O3 -g0 -Wall

ARFLAGS = cr

lib_LIBRARIES = lib/libavl.a
lib_libavl_a_SOURCES = src/avl.c include/avl.h

bin_PROGRAMS = bin/test-avl
bin_test_avl_SOURCES = src/test-avl.c
bin_test_avl_LDADD = lib/libavl.a

include_HEADERS = include/avl.h

EXTRA_DIST = windows acdoxygen.m4 amdoxygen.am doxygen.avl.cfg avl.pc .gitignore

super-clean: clean distclean
	@find . -name 'Makefile.in' -delete
	@find . -name 'config.h.in' -delete
	@find . -name '*.tar' -delete
	@find . -name '*.tar.*' -delete
	@find . -name '*~' -delete
	@find . -name '*.obj' -delete
	@find . -name '*.lib' -delete
	@find . -name '*.exe' -delete
	@rm -rf autom4te.cache
	@rm -f aclocal.m4
	@rm -f configure
	@rm -rf src/.deps
	@rm -f texput.log

include amdoxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST += $(DX_CONFIG)

install-data-hook:
	@if test -d $(DX_HTML_DIR); \
	 then \
	   echo installing $(DX_HTML_DIR) to $(prefix)/share/doc/avl; \
	   mkdir -p $(prefix)/share/doc/avl; \
	   cp -r $(DX_HTML_DIR) $(prefix)/share/doc/avl; \
	 fi
	@echo installing avl.pc
	@mkdir -p $(prefix)/share/pkgconfig
	@cp avl.pc $(prefix)/share/pkgconfig/avl.pc

uninstall-hook:
	@rm -rf $(prefix)/share/doc/avl
	@rm -rf $(prefix)/share/pkgconfig/avl.pc

code-stats:
	@echo -n " total lines of source code: "; wc -l src/*.[ch] include/*.[ch] | tail -1 | sed -e 's/ total//' -e 's/^  *//'
	@echo -n "actual lines of source code: "; (gcc -fpreprocessed -dD -E src/*.[ch] include/*.[ch] | sed -e '/^$$/d' -e '/^# [0-9]/d' | wc -l)

