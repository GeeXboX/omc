# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(omc, 0.1.0, ben@geexbox.org)
AM_INIT_AUTOMAKE([gnits std-options check-news dist-bzip2 no-dist-gzip])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([limits.h fcntl.h sys/ioctl.h syslog.h])
AC_CHECK_HEADERS([Evas.h], [], [
  AC_MSG_ERROR(*** You need Evas to build this program ! ***)
])
AC_CHECK_HEADERS([Ecore.h], [], [
  AC_MSG_ERROR(*** You need Ecore to build this program ! ***)
])
AC_CHECK_HEADERS([Ecore_Evas.h], [], [
  AC_MSG_ERROR(*** You need Ecore_Evas to build this program ! ***)
])
AC_CHECK_HEADERS([xine.h], [], [
  AC_MSG_ERROR(*** You need Xine to build this program ! ***)
])
AC_CHECK_HEADERS([curl/curl.h], [], [
  AC_MSG_ERROR(*** You need Curl to build this program ! ***)
])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T

# Checks for library functions
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset socket strcasecmp strchr strrchr strdup strndup strerror])

# Checks for libraries.
AC_SEARCH_LIBS(evas_new, evas, [], [exit])
AC_SEARCH_LIBS(ecore_timer_add, ecore, [], [exit])
AC_SEARCH_LIBS(ecore_evas_init, ecore_evas, [], [exit])
AC_SEARCH_LIBS(xine_init, xine, [], [exit])
AC_SEARCH_LIBS(curl_easy_perform, curl, [], [exit])

if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_REENTRANT"
fi

AC_DEFINE(_GNU_SOURCE, [],[Use GNU])

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        ])
AC_OUTPUT
