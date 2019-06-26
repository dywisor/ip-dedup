#ifndef _HAVE_DATA_IP_TREE_DATA_NODE_H_
#define _HAVE_DATA_IP_TREE_DATA_NODE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../ip.h"

/**
 * IPv4 subnet tree node
 *
 * Consists of an ip address (addr) and a netmask length (prefixlen).
 *
 * The purpose of this data structure is merging and deduplicating IP networks,
 * e.g. for aggregating blocklists or routes.
 * Each level of the tree represents a single bit in the network address,
 * alongside with the appropriate netmask length for that particular level.
 *
 * A network can be split into two subnets,
 * except for the leaf case (IPv4 prefixlen == 32).
 * These subnets may be reached via the 'left' or 'right' attribute,
 * depending on whether the subnet's bit is set to zero or one, respectively.
 *
 * The 'hot' attribute indicates whether network of a node is relevant,
 * i.e. has been explicitly added to tree or is the result of merging/dedup.
 * If the 'hot' attribute is set to false, then this node exists solely
 * for some 'hot' subnet at a deeper level. *
 *
 * Suppose you add "192.168.128.0/18" (128 + 0 in the 3rd block) to the tree.
 * This creates 17 transient nodes for representing "192.168.128.0/17"
 * and creates a 'hot' child node for "192.168.128.0/18",
 * stored in the 'left' attr of the "192.168.128.0/17" node.
 *
 * Afterwards you add "192.168.192.0/18" (128 + 64 in the 3rd block).
 * This will reuse the first transient 17 nodes because the bit sequence is the same.
 * A new 'hot' child node at level 18 gets created for "192.168.192.0/18",
 * which gets stored in the 'right' attr of the "192.168.128.0/17" node.
 *
 * Once more you add "192.168.240.0/20". No new nodes will be created
 * since this network is overshadowed by the "192.168.192.0/18" node.
 *
 * Then you call ip4_tree_collapse() on the data structure.
 * It will detect that both subnets of the "192.168.128.0/17" node are 'hot'
 * and merges them by marking the "192.168.128.0/17" node as 'hot'.
 * The subnets are not of use anymore and their data gets freed.
 *
 * Finally, you add "192.168.0.0/16".
 * This will mark the existing "192.168.0.0/16" node as 'hot'
 * and free() all overshadowed subnetworks, including "192.168.128.0/17".
 * */
struct ip_tree_node;
struct ip_tree_node {
   struct ip_tree_node* left;
   struct ip_tree_node* right;

   bool                 hot;
   ip_addr_variant_t      addr;
};

bool ip_tree_node_left_hot ( const struct ip_tree_node* const restrict node );
bool ip_tree_node_right_hot ( const struct ip_tree_node* const restrict node );


/* IPv4 functions */
void ip4_tree_node_init_null ( struct ip_tree_node* const restrict node );

struct ip_tree_node* ip4_tree_node_new_null (void);

struct ip_tree_node* ip4_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
);

struct ip_tree_node* ip4_tree_node_new_child (
   const struct ip_tree_node* const restrict src,
   const bool bit_set,
   const bool hot
);

void ip4_tree_node_destroy (
   struct ip_tree_node** const restrict node_ptr
);

void ip4_tree_node_flip_addr_inplace ( struct ip_tree_node* const restrict node );


/* IPv6 functions */
void ip6_tree_node_init_null ( struct ip_tree_node* const restrict node );

struct ip_tree_node* ip6_tree_node_new_null (void);

struct ip_tree_node* ip6_tree_node_new_sibling (
   const struct ip_tree_node* const restrict src,
   const bool hot
);

struct ip_tree_node* ip6_tree_node_new_child (
   const struct ip_tree_node* const restrict src,
   const bool bit_set,
   const bool hot
);

void ip6_tree_node_destroy (
   struct ip_tree_node** const restrict node_ptr
);

void ip6_tree_node_flip_addr_inplace ( struct ip_tree_node* const restrict node );

#endif
