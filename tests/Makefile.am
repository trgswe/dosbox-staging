if BUILD_TESTS

AM_CPPFLAGS = -I$(top_srcdir)/include

SUBDIRS = \
	../src/cpu \
	../src/debug \
	../src/dos \
	../src/fpu \
	../src/gui \
	../src/hardware \
	../src/libs \
	../src/ints \
	../src/midi \
	../src/misc \
	../src/shell \
	../src/platform

bin_PROGRAMS = tests

tests_CXXFLAGS = $(AM_CXXFLAGS) $(CXXFLAGS)
if USE_GTEST_STATIC
tests_CXXFLAGS += -Wl,-Bstatic -lgtest_main -lgtest -Wl,-Bdynamic
else
tests_CXXFLAGS += -lgtest_main -lgtest
endif

tests_SOURCES = \
	example.cpp \
	mem.cpp \
	soft_limiter.cpp \
	support.cpp \
	../src/dosbox.cpp

#/tests_LDADD = \
#/	../src/misc/libmisc.a

tests_LDADD = \
	../src/cpu/libcpu.a \
	../src/debug/libdebug.a \
	../src/dos/libdos.a \
	../src/fpu/libfpu.a \
	../src/gui/libgui.a \
	../src/hardware/libhardware.a \
	../src/hardware/mame/libmame.a \
	../src/hardware/serialport/libserial.a \
	../src/ints/libints.a \
	../src/libs/decoders/libdecoders.a \
	../src/libs/gui_tk/libgui_tk.a \
	../src/libs/nuked/libnuked.a \
	../src/libs/ppscale/libppscale.a \
	../src/midi/libmidi.a \
	../src/misc/libmisc.a \
	../src/shell/libshell.a

# Override automake's distclean target to prevent it recursing into
# SUBDIRS and failing (it was already invoked in there via src/Makefile.am).
distclean:
	-rm tests$(EXEEXT)
	-rm *.o
	-rm Makefile

endif
