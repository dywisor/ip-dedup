#ifndef _HAVE_MACROS_H_
#define _HAVE_MACROS_H_

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#ifndef USE_OPENBSD_PLEDGE
#define USE_OPENBSD_PLEDGE 0
#endif

#if USE_OPENBSD_PLEDGE
#define OPENBSD_PLEDGE(_promises, _execpromises)  \
    do { \
        if ( pledge ( _promises, _execpromises ) == -1 ) { \
            return -1; \
        } \
    } while (0)

#else
#define OPENBSD_PLEDGE(_promises, _execpromises)  {}
#endif



#endif  /* _HAVE_MACROS_H_ */
