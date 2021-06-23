#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "print_ip.h"

#include "../ip.h"


/* IPv4 */

int fprint_ip4_addr_data (
    FILE* const restrict stream,
    const ip4_addr_data_t addr_data
) {
    return fprintf (
        stream, (IP4_ADDR_FMT "\n"), ip4_addr_fmt_args(addr_data)
    );
}


int fprint_ip4_addr (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
) {
    return fprintf (
        stream, (IP4_ADDR_FMT "\n"), ip4_addr_fmt_args((addr)->addr)
    );
}


int fprint_ip4_net (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
) {
    return fprintf (
        stream, (IP4_NET_FMT "\n"),
        ip4_addr_fmt_args((addr)->addr), (addr)->prefixlen
    );
}


int fprint_ip4_addr_or_net (
    FILE* const restrict stream,
    const struct ip4_addr_t* const addr
) {
    if ( addr->prefixlen == IP4_MAX_PREFIXLEN ) {
        return fprint_ip4_addr ( stream, addr );
    } else {
        return fprint_ip4_net ( stream, addr );
    }
}


/* IPv6 */
int fprint_ip6_addr_data (
    FILE* const restrict stream,
    const ip6_addr_data_t addr_data
) {
    return fprintf (
        stream, (IP6_ADDR_FMT "\n"), ip6_addr_fmt_args(addr_data)
    );
}


int fprint_ip6_addr (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
) {
    return fprintf (
        stream, (IP6_ADDR_FMT "\n"), ip6_addr_fmt_args((addr)->addr)
    );
}


int fprint_ip6_net (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
) {
    return fprintf (
        stream, (IP6_NET_FMT "\n"),
        ip6_addr_fmt_args((addr)->addr), (addr)->prefixlen
    );
}


int fprint_ip6_addr_or_net (
    FILE* const restrict stream,
    const struct ip6_addr_t* const addr
) {
    if ( addr->prefixlen == IP6_MAX_PREFIXLEN ) {
        return fprint_ip6_addr ( stream, addr );
    } else {
        return fprint_ip6_net ( stream, addr );
    }
}
