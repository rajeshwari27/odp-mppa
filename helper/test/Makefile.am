include $(top_srcdir)/test/Makefile.inc
bindir      = $(datarootdir)/tests/helpers/

AM_CFLAGS += -I$(srcdir)/common
AM_LDFLAGS += -static

EXECUTABLES = odp_chksum$(EXEEXT) \
              odp_thread$(EXEEXT) \
              odp_process$(EXEEXT)\
              odph_pause$(EXEEXT)

COMPILE_ONLY =

TESTSCRIPTS =

if test_helper
TESTS = $(EXECUTABLES) $(TESTSCRIPTS)
endif

dist_bin_SCRIPTS =

bin_PROGRAMS = $(EXECUTABLES) $(COMPILE_ONLY)


dist_odp_chksum_SOURCES = odp_chksum.c
dist_odp_thread_SOURCES = odp_thread.c
odp_thread_LDADD = $(LIB)/libodphelper.la $(LIB)/libodp.la
dist_odp_process_SOURCES = odp_process.c
odp_process_LDADD = $(LIB)/libodphelper.la $(LIB)/libodp.la
odph_pause_SOURCES = odph_pause.c
