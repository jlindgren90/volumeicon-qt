# Process this file with autoconf to produce a configure script
AC_INIT([volumeicon], [0.5.1], [http://www.softwarebakery.com/maato/volumeicon.html])

AM_INIT_AUTOMAKE

# Check for programs
AC_LANG([C])
AC_PROG_CC

# Check for gtk
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Check for alsa
PKG_CHECK_MODULES([ALSA], [alsa])
AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)

# Check for libnotify
PKG_CHECK_MODULES([NOTIFY], [libnotify >= 0.7.0])
AC_SUBST(NOTIFY_CFLAGS)
AC_SUBST(NOTIFY_LIBS)

DEFAULT_MIXERAPP="xterm -e 'alsamixer'"
AC_ARG_WITH(default-mixerapp,
[  --with-default-mixerapp=appname
                           Default command for "Open Mixer" menu item
                           (default: "xterm -e 'alsamixer'")],
DEFAULT_MIXERAPP=$withval)
AC_DEFINE_UNQUOTED(DEFAULT_MIXERAPP,"${DEFAULT_MIXERAPP}",[Default mixer application])

AC_CONFIG_FILES(Makefile \
		src/Makefile \
		doc/Makefile)

AC_OUTPUT
