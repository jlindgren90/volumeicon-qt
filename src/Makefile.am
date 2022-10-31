AM_CFLAGS = -Wall
AM_CFLAGS += @GTK_CFLAGS@ @ALSA_CFLAGS@ @NOTIFY_CFLAGS@

LIBS = @GTK_LIBS@ @ALSA_LIBS@ @NOTIFY_LIBS@ -lm

bin_PROGRAMS = volumeicon

volumeicon_SOURCES = \
	volumeicon.c \
	config.c \
	config.h \
	alsa_backend.c \
	alsa_backend.h
