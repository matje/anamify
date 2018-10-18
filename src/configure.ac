dnl
dnl Some global settings
dnl

AC_INIT(anamify,0.0.1,matthijs@pletterpet.nl)

CFLAGS="$CFLAGS"

dnl
dnl Checks for programs and headers
dnl
AC_PROG_CC
AC_CHECK_HEADERS([arpa/inet.h ldns/ldns.h ldns/host2str.h stdio.h])

dnl
dnl Checks for library functions
dnl
AC_FUNC_MALLOC
AC_CHECK_SIZEOF(void*)
AC_CHECK_FUNCS([memcpy memset])
AC_SEARCH_LIBS(getdns, [getdns])
AC_CHECK_LIB(ldns, ldns_zone_new_frm_fp_l,
        [LIBS="-lldns $LIBS"
        AC_SUBST(LIBS)],
        [AC_MSG_ERROR(["ldns not found"])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
