#ifndef _HAVE_APP_IP_DEDUP_GLOBALS_H_
#define _HAVE_APP_IP_DEDUP_GLOBALS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../ip.h"
#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"

/**
 * mode of tree operations, choose from
 * - IPv4 only
 * - IPv6 only
 * - mixed IPv4 / IPv6
 * */
enum {
    IPDEDUP_TREE_MODE_NONE  = 0,

    IPDEDUP_TREE_MODE_IPV4  = 0x1,
    IPDEDUP_TREE_MODE_IPV6  = 0x2,
    IPDEDUP_TREE_MODE_MIXED = (IPDEDUP_TREE_MODE_IPV4 | IPDEDUP_TREE_MODE_IPV6)
};


struct ipdedup_globals;

/**
 * main function dispatcher, usually depends on mode of tree operations
 *
 * ipdedup_main_action :: globals -> int
 * */
typedef int (*ipdedup_main_action) (struct ipdedup_globals* const restrict);

struct ipdedup_globals {
    int tree_mode;
    struct ip_tree_build_data* tree_builder;

    /* shared references to the tree builder's output data, do not free them */
    struct ip_tree* tree_v4;
    struct ip_tree* tree_v6;

    FILE* outstream;
    bool close_outstream;

    bool want_invert;
    bool want_keep_going;
};

/**
 * Initializes the program's global variables.
 *
 * @param g
 *
 * @return 0 on success, non-zero otherwise (e.g. malloc() failed)
 * */
int ipdedup_globals_init ( struct ipdedup_globals* const restrict g );


/**
 * Destroys the program's global variables.
 *
 * @param g
 *
 * @return None (implicit)
 * */
void ipdedup_globals_free ( struct ipdedup_globals* const restrict g );


/**
 * Initializes the tree view attribute.
 *
 * @param g
 *
 * @return None (implicit)
 * */
void ipdedup_globals_init_tree_view ( struct ipdedup_globals* const restrict g );

#endif  /* _HAVE_APP_IP_DEDUP_GLOBALS_H_ */