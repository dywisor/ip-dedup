#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>      /* fflush(), fclose() */

#include "globals.h"

#include "../../ip.h"
#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"
#include "../../util/parse_ip/common.h"


int ipdedup_globals_init ( struct ipdedup_globals* const restrict g ) {
    g->tree_mode        = IPDEDUP_TREE_MODE_NONE;
    g->tree_builder     = NULL;

    g->tree_v4          = NULL;
    g->tree_v6          = NULL;

    g->prog_name        = NULL;

    g->outfile          = NULL;
    g->outstream        = NULL;
    g->close_outstream  = false;

    g->want_invert      = false;
    g->want_keep_going  = false;

    return 0;
}


void ipdedup_globals_free ( struct ipdedup_globals* const restrict g ) {
    ip_tree_builder_destroy ( &(g->tree_builder) );

    if ( g->outstream != NULL ) {
        fflush ( g->outstream );        /* ignore retcode */

        if ( g->close_outstream ) {
            fclose ( g->outstream );    /* ignore retcode */
            g->outstream = NULL;
        }
    }
}


void ipdedup_globals_init_tree_view (
    struct ipdedup_globals* const restrict g
) {
    g->tree_v4 = NULL;
    g->tree_v6 = NULL;

    if ( (g->tree_mode & IPDEDUP_TREE_MODE_IPV4) != 0 ) {
        g->tree_v4 = g->tree_builder->v4;
    }

    if ( (g->tree_mode & IPDEDUP_TREE_MODE_IPV6) != 0 ) {
        g->tree_v6 = g->tree_builder->v6;
    }
}
