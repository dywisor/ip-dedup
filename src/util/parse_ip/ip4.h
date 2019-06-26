#ifndef _HAVE_UTIL_PARSE_IP_IP4_H_
#define _HAVE_UTIL_PARSE_IP_IP4_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../ip/ip4.h"

int parse_ip4_addr (
   char* const restrict line,
   const size_t slen,
   struct ip4_addr_t* const restrict addr
);

int parse_ip4_addr_split (
    char* const addr_str,
    const char* const prefixlen_str,
    struct ip4_addr_t* const restrict addr
);

#endif  /* _HAVE_UTIL_PARSE_IP_IP4_H_ */
