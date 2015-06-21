/* windows/config.h.  Generated from config.h.in by configure.  */
#ifndef CODA_CONFIG_WIN_H
#define CODA_CONFIG_WIN_H

/* include windows specific headers */
#include <windows.h>
#include <io.h>
#include <direct.h>

/* include string.h for size_t definition */
#include <string.h>

/* include sys/types.h and sys/stat.h because we are going to override off_t and stat */
#include <sys/types.h>
#include <sys/stat.h>

/* This is the current version of libcoda */
#define CODA_VERSION "2.13"

/* Version number of package */
#define VERSION "2.13"

/* Version of linked-in QIAP library */
#define QIAP_VERSION "2.13-coda"

/* some defines that are needed to configure expat */
#define HAVE_MEMMOVE 1
/* we need to redefine ELEMENT_TYPE because it conflicts with io.h contents */
#define ELEMENT_TYPE ELEMENT_TYPE_RENAMED
#define XML_NS 1
#define XML_DTD 1
#define XML_LARGE_SIZE 1
#define XML_CONTEXT_BYTES 1024
#define XML_BUILDING_EXPAT 1

/* some defines that are needed for pcre */
#define PCRE_STATIC

/* redefines for special string handling functions */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf _snprintf
#if defined(_MSC_VER) && _MSC_VER < 1500
#define vsnprintf _vsnprintf
#endif

/* redefines for file handling functions */
#define open(arg1,arg2) _open(arg1,arg2)
#define close(arg1) _close(arg1)
#define read(arg1,arg2,arg3) _read(arg1,arg2,arg3)
#define lseek(arg1,arg2,arg3) _lseeki64(arg1,arg2,arg3)
#define off_t __int64
#define stat _stati64
#define S_IFREG _S_IFREG

#ifdef HAVE_HDF4
/* Define to 1 if HDF4 Vdata/Vgroup attributes can be used (only enable if
   CODA is linked against HDF4 library version 4.2r2 or higher). */
#define ENABLE_HDF4_VDATA_ATTRIBUTES 1
#endif

/* Make sure we use the 1.6 compatibility API for HDF 1.8 */
#define H5_USE_16_API

#define YYMALLOC malloc
#define YYFREE free

#define CODA_USE_QIAP 1
#define HAVE_LIBWS2_32 1

/* coda-idl defines */

#ifndef CODA_DEFINITION_IDL
#define CODA_DEFINITION_IDL "../definitions"
#endif

#ifdef IDL_V5_3
#define HAVE_IDL_SYSFUN_DEF2 1
#endif

#ifdef IDL_V5_4
#define HAVE_IDL_SYSFUN_DEF2 1
#define HAVE_IDL_SYSRTN_UNION 1
#endif

/* coda-matlab defines */

#ifndef CODA_DEFINITION_MATLAB
#define CODA_DEFINITION_MATLAB "../definitions"
#endif

#if defined(MATLAB_R11) || defined(MATLAB_R12)
#define mxCreateDoubleScalar replacement_mxCreateDoubleScalar
#else
#define HAVE_MXCREATEDOUBLESCALAR 1
#endif

#ifdef MATLAB_R11
#define mxCreateNumericMatrix replacement_mxCreateNumericMatrix
#else
#define HAVE_MXCREATENUMERICMATRIX 1
#endif

#endif /* !defined(CODA_CONFIG_WIN_H) */
