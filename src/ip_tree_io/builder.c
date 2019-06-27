#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "builder.h"

#include "../ip_tree.h"
#include "../ip.h"

#include "../util/strutil.h"
#include "../util/parse_ip.h"


static struct ip_tree_build_data* ip_tree_build_data_new_empty (void);
static int ip_tree_build_data_init_v4 ( struct ip_tree_build_data* const obj );
static int ip_tree_build_data_init_v6 ( struct ip_tree_build_data* const obj );


static int ip_tree_builder_parse_ip_v4 (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
);

static int ip_tree_builder_parse_ip_v6 (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
);

static int ip_tree_builder_parse_ip_mixed (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
);


static struct ip_tree_build_data* ip_tree_build_data_new_empty (void) {
    struct ip_tree_build_data* obj;

    obj = malloc ( sizeof *obj );
    if ( obj != NULL ) {
        obj->v4         = NULL;
        obj->v6         = NULL;
        obj->f_parse    = NULL;
    }

    return obj;
}


static int ip_tree_build_data_init_v4 ( struct ip_tree_build_data* const obj ) {
    obj->v4 = ip4_tree_new();
    return ( obj->v4 != NULL ) ? 0 : -1;
}


static int ip_tree_build_data_init_v6 ( struct ip_tree_build_data* const obj ) {
    obj->v6 = ip6_tree_new();
    return ( obj->v6 != NULL ) ? 0 : -1;
}


struct ip_tree_build_data* ip_tree_builder_new_v4 (void) {
    struct ip_tree_build_data* obj;

    obj = ip_tree_build_data_new_empty();
    if ( obj != NULL ) {
        obj->f_parse = ip_tree_builder_parse_ip_v4;

        if ( ip_tree_build_data_init_v4 ( obj ) != 0 ) {
            free ( obj );
            obj = NULL;
        }
    }

    return obj;
}


struct ip_tree_build_data* ip_tree_builder_new_v6 (void) {
    struct ip_tree_build_data* obj;

    obj = ip_tree_build_data_new_empty();
    if ( obj != NULL ) {
        obj->f_parse = ip_tree_builder_parse_ip_v6;

        if ( ip_tree_build_data_init_v6 ( obj ) != 0 ) {
            free ( obj );
            obj = NULL;
        }
    }

    return obj;
}


struct ip_tree_build_data* ip_tree_builder_new_mixed (void) {
    struct ip_tree_build_data* obj;

    obj = ip_tree_build_data_new_empty();
    if ( obj != NULL ) {
        obj->f_parse = ip_tree_builder_parse_ip_mixed;

        if (
            ( ip_tree_build_data_init_v4 ( obj ) != 0 )
            || ( ip_tree_build_data_init_v6 ( obj ) != 0 )
        ) {
            free ( obj );
            obj = NULL;
        }
    }

    return obj;
}


void ip_tree_builder_destroy ( struct ip_tree_build_data** const obj_ptr ) {
    if ( *obj_ptr != NULL ) {
        ip_tree_destroy ( &((*obj_ptr)->v4) );
        ip_tree_destroy ( &((*obj_ptr)->v6) );

        free ( *obj_ptr );
        *obj_ptr = NULL;
    }
}

struct ip_tree* ip_tree_builder_steal_v4 (
    struct ip_tree_build_data* const obj
) {
    struct ip_tree* tree;

    tree = obj->v4;
    obj->v4 = NULL;
    return tree;
}


struct ip_tree* ip_tree_builder_steal_v6 (
    struct ip_tree_build_data* const obj
) {
    struct ip_tree* tree;

    tree = obj->v6;
    obj->v6 = NULL;
    return tree;
}

#define ip_tree_builder_parse__free_buf()  \
    do { \
        if ( line != NULL ) { free(line); line = NULL; } \
    } while (0)

#define ip_tree_builder_parse__return(_retcode)  \
    do { \
        ip_tree_builder_parse__free_buf(); \
        return (_retcode); \
    } while (0)

int ip_tree_builder_parse (
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going
) {
    struct ip_tree_builder_parse_data pstate;
    int         ret;
    bool        one_hot;
    unsigned    keep_going_status;  /* 0x1 -> any success, 0x2 -> any invalid */
    size_t      line_size;
    ssize_t     nread;
    size_t      slen;
    char*       line;
    char*       sline;

    line = NULL;
    line_size = 0;
    keep_going_status = 0;

    while ( ( nread = getline ( &line, &line_size, input_stream ) ) != -1 ) {
        slen = (size_t) nread;
        sline = str_strip ( line, &slen );

        if ( *sline != '\0' ) {
            pstate.addr_type = PARSE_IP_TYPE_NONE;

            ret = obj->f_parse ( sline, slen, &pstate );
            switch ( ret ) {
                case PARSE_IP_RET_SUCCESS:
                    one_hot = false;

                    if ( (pstate.addr_type & PARSE_IP_TYPE_IPV4) != 0 ) {
                        ret = ip4_tree_insert ( obj->v4, &(pstate.addr_v4) );
                        if ( ret != 0 ) { ip_tree_builder_parse__return ( -1 ); }
                        one_hot = true;
                    }

                    if ( (pstate.addr_type & PARSE_IP_TYPE_IPV6) != 0 ) {
                        ret = ip6_tree_insert ( obj->v6, &(pstate.addr_v6) );
                        if ( ret != 0 ) { ip_tree_builder_parse__return ( -1 ); }
                        one_hot = true;
                    }
                    if ( ! one_hot ) { ip_tree_builder_parse__return ( -1 ); }
                    keep_going_status |= 0x1;
                    break;

                case PARSE_IP_RET_INVALID:
                    if ( keep_going ) {
                        keep_going_status |= 0x2;
                    } else {
                        ip_tree_builder_parse__return ( ret );
                    }
                    break;

                default:
                    ip_tree_builder_parse__return ( -1 );
                    break;
            }
        }
    }

    switch ( keep_going_status ) {
        case 0x0:
        case 0x1:
        case 0x3:
            ret = PARSE_IP_RET_SUCCESS;
            break;

        case 0x2:
            ret = PARSE_IP_RET_INVALID;
            break;

        default:
            ret = PARSE_IP_RET_FAIL;
            break;
    }

    ip_tree_builder_parse__return ( ret );
}
#undef ip_tree_builder_parse__return
#undef ip_tree_builder_parse__free_buf


static int ip_tree_builder_parse_ip_v4 (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
) {
    pstate->addr_type = PARSE_IP_TYPE_IPV4;

    return parse_ip4_addr ( line, slen, &(pstate->addr_v4) );
}

static int ip_tree_builder_parse_ip_v6 (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
) {
    pstate->addr_type = PARSE_IP_TYPE_IPV6;

    return parse_ip6_addr ( line, slen, &(pstate->addr_v6) );
}

static int ip_tree_builder_parse_ip_mixed (
    char* const restrict line,
    const size_t slen,
    struct ip_tree_builder_parse_data* const restrict pstate
) {
    return parse_ip_addr (
        line,
        slen,
        &(pstate->addr_type),
        &(pstate->addr_v4),
        &(pstate->addr_v6)
    );
}
