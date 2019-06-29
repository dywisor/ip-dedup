#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "builder.h"

#include "../ip_tree.h"
#include "../ip.h"

#include "../util/readfile.h"
#include "../util/parse_ip.h"


static struct ip_tree_build_data* ip_tree_build_data_new_empty (void);
static int ip_tree_build_data_init_v4 ( struct ip_tree_build_data* const obj );
static int ip_tree_build_data_init_v6 ( struct ip_tree_build_data* const obj );



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
        obj->f_parse = parse_ip4_addr_combined;

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
        obj->f_parse = parse_ip6_addr_combined;

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
        obj->f_parse = parse_ip_addr_combined;

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


int ip_tree_builder_parse (
    struct ip_tree_build_data* const restrict obj,
    FILE* const restrict input_stream,
    const bool keep_going
) {
    struct parse_ip_addr_data pstate;
    struct readfile_state rstate;

    int         ret;
    bool        one_hot;
    unsigned    keep_going_status;  /* 0x1 -> any success, 0x2 -> any invalid */

    /* no-init pstate */

    if ( readfile_init_stream ( &rstate, input_stream ) != 0 ) { return -1; }

    keep_going_status = 0;

    while ( readfile_next_effective ( &rstate ) == READFILE_RET_LINE ) {

        pstate.addr_type = PARSE_IP_TYPE_NONE;

        ret = obj->f_parse ( rstate.line, rstate.line_len, &pstate );
        switch ( ret ) {
            case PARSE_IP_RET_SUCCESS:
                one_hot = false;

                if ( (pstate.addr_type & PARSE_IP_TYPE_IPV4) != 0 ) {
                    ret = ip4_tree_insert ( obj->v4, &(pstate.addr_v4) );
                    if ( ret != 0 ) {
                        readfile_state_free_data ( &rstate );
                        return -1;
                    }
                    one_hot = true;
                }

                if ( (pstate.addr_type & PARSE_IP_TYPE_IPV6) != 0 ) {
                    ret = ip6_tree_insert ( obj->v6, &(pstate.addr_v6) );
                    if ( ret != 0 ) {
                        readfile_state_free_data ( &rstate );
                        return -1;
                    }
                    one_hot = true;
                }

                if ( ! one_hot ) {
                    if ( ret != 0 ) {
                        readfile_state_free_data ( &rstate );
                        return -1;
                    }
                }
                keep_going_status |= 0x1;
                break;

            case PARSE_IP_RET_INVALID:
                if ( keep_going ) {
                    keep_going_status |= 0x2;
                } else {
                    readfile_state_free_data ( &rstate );
                    return ret;
                }
                break;

            default:
                readfile_state_free_data ( &rstate );
                return -1;
                break;  /* unreachable */
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

    readfile_state_free_data ( &rstate );
    return ret;
}
