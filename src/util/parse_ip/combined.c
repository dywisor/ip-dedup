#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "ip4.h"
#include "ip6.h"
#include "mixed.h"
#include "combined.h"

#include "../../ip.h"


int parse_ip4_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
) {
    pstate->addr_type = PARSE_IP_TYPE_IPV4;

    return parse_ip4_addr ( line, slen, &(pstate->addr_v4) );
}


int parse_ip6_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
) {
    pstate->addr_type = PARSE_IP_TYPE_IPV6;

    return parse_ip6_addr ( line, slen, &(pstate->addr_v6) );
}


int parse_ip_addr_combined (
    char* const restrict line,
    const size_t slen,
    struct parse_ip_addr_data* const restrict pstate
) {
    return parse_ip_mixed_addr (
        line,
        slen,
        &(pstate->addr_type),
        &(pstate->addr_v4),
        &(pstate->addr_v6)
    );
}
