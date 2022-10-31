AM_CXXFLAGS = -Wall -std=c++11
AM_CXXFLAGS += @GLIB_CFLAGS@ @QT_CFLAGS@ @ALSA_CFLAGS@ @NOTIFY_CFLAGS@

LIBS = @GLIB_LIBS@ @QT_LIBS@ @ALSA_LIBS@ @NOTIFY_LIBS@ -lm

bin_PROGRAMS = volumeicon

volumeicon_SOURCES = \
	volumeicon.cpp \
	config.cpp \
	config.h \
	alsa_backend.cpp \
	alsa_backend.h
