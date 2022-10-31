AM_CXXFLAGS = -Wall
AM_CXXFLAGS += @GTK_CFLAGS@ @ALSA_CFLAGS@ @NOTIFY_CFLAGS@

LIBS = @GTK_LIBS@ @ALSA_LIBS@ @NOTIFY_LIBS@ -lm

bin_PROGRAMS = volumeicon

volumeicon_SOURCES = \
	volumeicon.cpp \
	config.cpp \
	config.h \
	alsa_backend.cpp \
	alsa_backend.h
