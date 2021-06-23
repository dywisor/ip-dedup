#ifndef _HAVE_UTIL_PRINT_IP_H_
#define _HAVE_UTIL_PRINT_IP_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../ip.h"


/* print func typedef */

typedef int (*ip4_print_func) (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
);

typedef int (*ip6_print_func) (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
);


/* IPv4 */
int fprint_ip4_addr_data (
    FILE* const restrict stream,
    const ip4_addr_data_t addr_data
);


int fprint_ip4_addr (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
);


int fprint_ip4_net (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
);


/* IPv6 */
int fprint_ip6_addr_data (
    FILE* const restrict stream,
    const ip6_addr_data_t addr_data
);


int fprint_ip6_addr (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
);


int fprint_ip6_net (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
);


#endif  /* _HAVE_UTIL_PRINT_IP_H_ */
