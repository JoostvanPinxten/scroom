# Scroom - Generic viewer for 2D data
# Copyright (C) 2009-2010 Kees-Jan Dijkzeul
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License, version 2, as published by the Free Software Foundation.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

AC_PREREQ([2.64])
AC_INIT([Scroom],
        m4_esyscmd([build-aux/git-version-gen | tr -d '\n']),
        [http://fuzzy.homedns.org/trac/scroom/newticket],
        [scroom],
        [http://fuzzy.homedns.org/trac/scroom])
AM_INIT_AUTOMAKE([-Wall -Werror foreign silent-rules])
AM_SILENT_RULES([yes])
LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([inc/presentationinterface.hh])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
dnl AC_PROG_CC
AC_PROG_LN_S
dnl AC_PROG_INSTALL

AC_LANG([C++])

# Configurable features
AC_ARG_ENABLE([mutracx-hacks],
  [AS_HELP_STRING([--enable-mutracx-hacks], 
    [Build for a Mutracx environment @<:@default=no@:>@])],
  [want_mutracx_hacks=${enableval}],
  [want_mutracx_hacks=unknown])
AC_ARG_ENABLE([doxygen-all-code],
  [  --enable-doxygen-all-code  Generate doxygen documentation for all code,
                             instead of just the api],
  [want_doxygen_all_code=${enableval}],
  [want_doxygen_all_code=no])

# Doxygen
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME], [doxyfile], [doc])
  
# Checks for packages.
pkg_modules="gtk+-2.0 >= 2.0.0, gthread-2.0, libglade-2.0 glib-2.0 cairo"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset pow select sqrt strerror])

# Mutracx specifics:
AC_MSG_CHECKING([for Mutracx development environment])
need_mutracx_hacks=no
if pkg-config "gtk+-2.0 >= 2.16.0"
then
  need_mutracx_hacks=no
  AC_MSG_RESULT([no])
else
  need_mutracx_hacks=yes
  AC_MSG_RESULT([yes])
fi

# Checking for boost
have_boost_thread_headers=no
AC_CHECK_HEADERS([boost/thread.hpp boost/thread/mutex.hpp], [have_boost_thread_headers=yes])
have_boost_thread_lib=no
# boost_thread-gcc42-mt is the name used at Mutracx...
AC_SEARCH_LIBS([_ZN5boost6thread6detachEv], [boost_thread-mt boost_thread-gcc42-mt], [have_boost_thread_lib=yes])

# Some sanity checks
if test "x${have_boost_thread_headers}" = xno
then
  AC_MSG_ERROR([Boost thread header files not found.])
fi
if test "x${have_boost_thread_lib}" = xno
then
  AC_MSG_ERROR([Boost thread library not found.])
fi
if test "x${need_mutracx_hacks}" = xyes
then
  # Print a warning if the user said "no" to mutracx hacks. If he said
  # "yes", or nothing at all, be quiet and enable anyway.
  if test "x${want_mutracx_hacks}" = xno
  then
    AC_MSG_WARN([Mutracx environment detected. Enabling mutracx-hacks])
  fi
  want_mutracx_hacks=yes
fi

# If no doxygen, then we can't process all code
DX_IF_FEATURE([doc], [], [want_doxygen_all_code=no])

# want_mutracx_hacks is now set to "yes" if they are either needed or
# requested.
if test "x${want_mutracx_hacks}" = xyes
then
  AC_DEFINE([MUTRACX_HACKS], 1, [Hacks for Mutracx environment enabled])
fi
if test "x${want_doxygen_all_code}" = xyes
then
  DX_EXTRA_SRC="\$(top_srcdir)/gui \$(top_srcdir)/libs \$(top_srcdir)/plugins"
else
  DX_EXTRA_SRC=
fi
AC_SUBST(DX_EXTRA_SRC)

# GetText
dnl GETTEXT_PACKAGE=scroom
dnl AC_SUBST(GETTEXT_PACKAGE)
dnl AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])
dnl 
dnl dnl Add the languages which your application supports here.
dnl ALL_LINGUAS=""
dnl AM_GLIB_GNU_GETTEXT


AC_CONFIG_FILES([Makefile
                 inc/Makefile
                 libs/Makefile
                 libs/threadpool/Makefile
                 libs/tiled-bitmap/Makefile
                 plugins/Makefile
                 plugins/colormap/Makefile
                 plugins/example/Makefile
                 plugins/tiff/Makefile
                 gui/Makefile
                 gui/ENVIRONMENT
                 gui/src/Makefile
                 doxyfile
])

dnl                  libs/memory-manager/Makefile

# Misc defines
AC_SUBST([plugindir], [$libdir/$PACKAGE])

AC_OUTPUT

# Check if doxygen is requested/enabled (for producing the diagnostic
# output below only)
DX_IF_FEATURE([doc], [want_doxygen=yes], [want_doxygen=no])

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix:   '${prefix}'.
 Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
           '${CXX} ${CXXFLAGS} ${CPPFLAGS}'

 Package features:
   Mutracx hacks: ${want_mutracx_hacks}
   Doxygen:       ${want_doxygen}
     all code:    ${want_doxygen_all_code}

 Now type 'make @<:@<target>@:>@'
   where the optional <target> is:
     all                - build all binaries
     install            - install everything
     doxygen-doc        - generate documentation
                          (if enabled)

--------------------------------------------------"

