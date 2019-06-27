#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "addr.h"
#include "base.h"


void ip6_addr_init_null ( struct ip6_addr_t* const restrict obj ) {
    obj->prefixlen = 0;
    obj->addr.low  = 0;
    obj->addr.high = 0;
}


void ip6_addr_init (
    struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixlen,
    const ip6_addr_data_t* const restrict addr
) {
    obj->prefixlen = prefixlen;
    obj->addr.low  = addr->low;
    obj->addr.high = addr->high;
}


void ip6_addr_init_child (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src,
    const bool subaddr_bit_is_set
) {
    obj->prefixlen = src->prefixlen + 1;
    ip6_calc_set_bit_at_prefixpos (
        &(src->addr), obj->prefixlen, subaddr_bit_is_set, &(obj->addr)
    );
}


void ip6_addr_init_sibling (
    struct ip6_addr_t* const restrict obj,
    const struct ip6_addr_t* const restrict src
) {
    obj->prefixlen = src->prefixlen;
    ip6_calc_flip_bit_at_prefixpos (
        &(src->addr), obj->prefixlen, &(obj->addr)
    );
}


void ip6_addr_flip_inplace ( struct ip6_addr_t* const restrict obj ) {
    const ip6_addr_data_t old_addr = {
        .low  = obj->addr.low,
        .high = obj->addr.high
    };

    ip6_calc_flip_bit_at_prefixpos ( &old_addr, obj->prefixlen, &(obj->addr));
}


bool ip6_addr_bit_is_set_at (
    const struct ip6_addr_t* const restrict obj,
    const ip_prefixlen_t prefixpos
) {
    return ip6_calc_bit_is_set_at_prefixpos ( &(obj->addr), prefixpos );
}
