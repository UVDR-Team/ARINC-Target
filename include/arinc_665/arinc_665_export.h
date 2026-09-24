
#ifndef ARINC_665_EXPORT_H
#define ARINC_665_EXPORT_H

#ifdef ARINC_665_STATIC_DEFINE
#  define ARINC_665_EXPORT
#  define ARINC_665_NO_EXPORT
#else
#  ifndef ARINC_665_EXPORT
#    ifdef arinc_665_EXPORTS
        /* We are building this library */
#      define ARINC_665_EXPORT 
#    else
        /* We are using this library */
#      define ARINC_665_EXPORT 
#    endif
#  endif

#  ifndef ARINC_665_NO_EXPORT
#    define ARINC_665_NO_EXPORT 
#  endif
#endif

#ifndef ARINC_665_DEPRECATED
#  define ARINC_665_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef ARINC_665_DEPRECATED_EXPORT
#  define ARINC_665_DEPRECATED_EXPORT ARINC_665_EXPORT ARINC_665_DEPRECATED
#endif

#ifndef ARINC_665_DEPRECATED_NO_EXPORT
#  define ARINC_665_DEPRECATED_NO_EXPORT ARINC_665_NO_EXPORT ARINC_665_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef ARINC_665_NO_DEPRECATED
#    define ARINC_665_NO_DEPRECATED
#  endif
#endif

#endif /* ARINC_665_EXPORT_H */
