#ifndef _HAVE_UTIL_PARSE_IP_MIXED_H_
#define _HAVE_UTIL_PARSE_IP_MIXED_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../ip.h"

enum {
    PARSE_IP_TYPE_NONE  = 0x0,
    PARSE_IP_TYPE_IPV4  = 0x1,
    PARSE_IP_TYPE_IPV6  = 0x2,
    PARSE_IP_TYPE_BOTH  = (PARSE_IP_TYPE_IPV4 | PARSE_IP_TYPE_IPV6)
};


int parse_ip_addr (
    char* const line,
    const size_t slen,
    int* const type_result,
    struct ip4_addr_t* const restrict addr_v4,
    struct ip6_addr_t* const restrict addr_v6
);


int parse_ip_addr_split (
    char* const addr_str,
    const char* const prefixlen_str,
    int* const type_result,
    struct ip4_addr_t* const restrict addr_v4,
    struct ip6_addr_t* const restrict addr_v6
);

#endif  /* _HAVE_UTIL_PARSE_IP_MIXED_H_ */
