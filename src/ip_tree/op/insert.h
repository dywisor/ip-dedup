#ifndef _HAVE_IP_TREE_OP_INSERT_H_
#define _HAVE_IP_TREE_OP_INSERT_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"

#include "../../ip.h"

/**
 * Adds a 'hot' network to the tree.
 *
 * Note: iterative
 *
 * @param root:       tree
 * @param ip_addr:    IPv4 address (as integer)
 * @param prefixlen:  netmask length (as integer)
 *
 * @returns 0 on success, any other value indicates an error
 * */
int ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict addr
);

/** IPv4 variant of ip_tree_insert() */
int ip4_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip4_addr_t* const restrict addr
);

/** IPv6 variant of ip_tree_insert() */
int ip6_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip6_addr_t* const restrict addr
);


#endif  /* _HAVE_IP_TREE_OP_INSERT_H_ */
