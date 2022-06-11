#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "print_json.h"

#include "../ip_tree.h"
#include "../ip.h"
#include "../util/print_ip.h"
#include "../util/dynarray.h"


static const char* PRINT_IP_TREE_JSON_INDENT = "  ";


static int fprint_ip4_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);


static int fprint_ip6_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);


static void fprint_ip4_addr_json (
    FILE* const restrict stream,
    const struct ip4_addr_t* const restrict addr
);


static void fprint_ip6_addr_json (
    FILE* const restrict stream,
    const struct ip6_addr_t* const restrict addr,
    const char* const compact_addr_str
);


void fprint_ip_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree_v4,
    struct ip_tree* const restrict tree_v6
) {
    fprintf ( stream, "{\n" );

    /* IPv4 */
    fprintf ( stream, "%s\"inet\": [\n", PRINT_IP_TREE_JSON_INDENT );
    if ( tree_v4 != NULL ) {
        fprint_ip4_tree_json ( stream, tree_v4 );
    }
    fprintf ( stream, "%s],\n", PRINT_IP_TREE_JSON_INDENT );

    /* IPv6 */
    fprintf ( stream, "%s\"inet6\": [\n", PRINT_IP_TREE_JSON_INDENT );
    if ( tree_v6 != NULL ) {
        fprint_ip6_tree_json ( stream, tree_v6 );
    }
    fprintf ( stream, "%s]\n", PRINT_IP_TREE_JSON_INDENT );

    fprintf ( stream, "}\n" );
}


static int fprint_ip4_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
    struct dynarray* darr;
    size_t k;

    darr = ip4_tree_collect_addr ( tree );
    if ( darr == NULL ) { return -1; }

    dynarray_foreach ( darr, k ) {
        const struct ip4_addr_t* const addr = dynarray_get_as (
            darr, k, const struct ip4_addr_t*
        );

        if ( k > 0 ) {
            fprintf ( stream, ",\n" );
        }

        fprint_ip4_addr_json ( stream, addr );
    }

    if ( darr->len > 0 ) {
        fprintf ( stream, "\n" );
    }

    dynarray_free_ptr ( &darr );

    return 0;
}


static int fprint_ip6_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
    struct dynarray* darr;
    char compact_addr_str [IP6_ADDR_STR_BUF_SIZE];
    size_t k;
    int ret;

    darr = ip6_tree_collect_addr ( tree );
    if ( darr == NULL ) { return -1; }

    ret = 0;

    for ( k = 0; (k < darr->len) && (ret == 0); k++ ) {
        const struct ip6_addr_t* const addr = dynarray_get_as (
            darr, k, const struct ip6_addr_t*
        );

        if ( k > 0 ) {
            fprintf ( stream, ",\n" );
        }

        if ( ip6_addr_data_into_str ( &(addr->addr), compact_addr_str ) != NULL ) {
            fprint_ip6_addr_json ( stream, addr, compact_addr_str );
        } else {
            ret = -1;
        }
    }

    if ( darr->len > 0 ) {
        fprintf ( stream, "\n" );
    }

    dynarray_free_ptr ( &darr );

    return ret;
}


static void fprint_ip4_addr_json (
    FILE* const restrict stream,
    const struct ip4_addr_t* const restrict addr
) {
    ip4_addr_data_t netmask;
    ip4_addr_data_t first_addr;
    ip4_addr_data_t last_addr;

    /* calculate netmask */
    netmask = (
        IP4_MAX_ADDR & (
            IP4_MAX_ADDR << (IP4_MAX_PREFIXLEN - addr->prefixlen)
        )
    );

    if ( addr->prefixlen >= IP4_MAX_PREFIXLEN ) {
        first_addr = addr->addr;
        last_addr  = addr->addr;

    } else if ( (addr->prefixlen + 1) == IP4_MAX_PREFIXLEN ) {
        /* /31 has no network/broadcast address */
        first_addr = addr->addr;
        last_addr  = addr->addr + 1;

    } else {
        first_addr = (addr->addr) + 1;
        last_addr  = ( (addr->addr) | ~netmask ) - 1;
    }

    /* begin of object */
    fprintf (
        stream,
        "%s%s{\n",
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT
    );

    /* network: network with prefixlen */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP4_NET_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "network",
        ip4_addr_fmt_args((addr)->addr),
        (addr)->prefixlen
    );

    /* address: network without prefixlen */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP4_ADDR_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "address",
        ip4_addr_fmt_args((addr)->addr)
    );

    /* netmask: prefixlen in network address format */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP4_ADDR_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "netmask",
        ip4_addr_fmt_args(netmask)
    );

    /* first: first address in network */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP4_ADDR_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "first",
        ip4_addr_fmt_args(first_addr)
    );

    /* last: last address in network */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP4_ADDR_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "last",
        ip4_addr_fmt_args(last_addr)
    );

    /* prefixlen: prefixlen */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": " IP_PREFIXLEN_FMT "\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "prefixlen",
        (addr)->prefixlen
    );

    /* end of object (no trailing comma/newline) */
    fprintf (
        stream,
        "%s%s}",
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT
    );
}


static void fprint_ip6_addr_json (
    FILE* const restrict stream,
    const struct ip6_addr_t* const restrict addr,
    const char* const compact_addr_str
) {
    /* begin of object */

    fprintf (
        stream,
        "%s%s{\n",
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT
    );

    /* network: network with prefixlen (in compact format) */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"%s/" IP_PREFIXLEN_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "network",
        compact_addr_str,
        (addr)->prefixlen
    );

    /* address: network without prefixlen (in compact format) */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"%s\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "address",
        compact_addr_str
    );

    /* network_exploded: network with prefixlen (in exploded format) */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP6_NET_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "network_exploded",
        ip6_addr_fmt_args((addr)->addr),
        (addr)->prefixlen
    );

    /* address_exploded: network without prefixlen (in exploded format) */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": \"" IP6_ADDR_FMT "\",\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "address_exploded",
        ip6_addr_fmt_args((addr)->addr)
    );

    /* prefixlen: prefixlen */
    fprintf ( \
        stream,
        ("%s%s%s\"%s\": " IP_PREFIXLEN_FMT "\n"),
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT,
        "prefixlen",
        (addr)->prefixlen
    );

    /* end of object (no trailing comma/newline) */
    fprintf (
        stream,
        "%s%s}",
        PRINT_IP_TREE_JSON_INDENT,
        PRINT_IP_TREE_JSON_INDENT
    );
}


#undef _FPRINT_JSON_ATTR
