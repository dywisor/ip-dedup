#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include "base.h"
#include "../base.h"

static ip_bitpos_t ip6_calc_bitpos ( const ip_prefixlen_t prefixpos ) {
    return ( IP6_MAX_PREFIXLEN - prefixpos );
}

static void ip6_calc_ro_bitpos_in (
    const ip6_addr_data_t* const restrict src,
    const ip_prefixlen_t prefixpos,
    ip6_addr_data_chunk_t* const restrict chunk_out,
    ip_bitpos_t* const restrict bitpos_out,
    bool* const restrict high_out
) {
    ip_bitpos_t bpos;

    bpos = ip6_calc_bitpos(prefixpos);

    if ( bpos < 64 ) {
        *chunk_out  = src->low;
        *bitpos_out = bpos;
        *high_out   = false;

    } else {
        *chunk_out  = src->high;
        *bitpos_out = bpos - 64;
        *high_out   = true;
    }
}


static ip6_addr_data_chunk_t ip6_calc_bit ( const ip_bitpos_t bpos ) {
    ip6_addr_data_chunk_t ret;

    ret = 0x1;
    ret <<= bpos;

    return ret;
}


bool ip6_calc_bit_is_set_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos
) {
    ip6_addr_data_chunk_t chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &chunk, &bpos, &is_high );

    return ( (chunk & ip6_calc_bit(bpos)) != 0 ) ? true : false;
}


void ip6_calc_flip_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    ip6_addr_data_t* const restrict dst
) {
    ip6_addr_data_chunk_t src_chunk;
    ip6_addr_data_chunk_t flipped;
    ip6_addr_data_chunk_t new_chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &src_chunk, &bpos, &is_high );

    flipped   = ip6_calc_bit(bpos);
    new_chunk = (src_chunk ^ flipped);

    if ( is_high ) {
        dst->low  = bits->low;
        dst->high = new_chunk;
    } else {
        dst->low  = new_chunk;
        dst->high = bits->high;
    }
}

void ip6_calc_set_bit_at_prefixpos (
    const ip6_addr_data_t* const restrict bits,
    const ip_prefixlen_t prefixpos,
    bool bit_set,
    ip6_addr_data_t* const restrict dst
) {
    ip6_addr_data_chunk_t src_chunk;
    ip6_addr_data_chunk_t bit_mask;
    ip6_addr_data_chunk_t new_chunk;
    ip_bitpos_t bpos;
    bool is_high;

    ip6_calc_ro_bitpos_in ( bits, prefixpos, &src_chunk, &bpos, &is_high );

    bit_mask  = ip6_calc_bit(bpos);
    new_chunk = ( bit_set ? (src_chunk | bit_mask) : (src_chunk & ~bit_mask) );

    if ( is_high ) {
        dst->low  = bits->low;
        dst->high = new_chunk;
    } else {
        dst->low  = new_chunk;
        dst->high = bits->high;
    }
}
