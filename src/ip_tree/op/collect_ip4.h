#ifndef _HAVE_IP_TREE_OP_COLLECT_IP4_H_
#define _HAVE_IP_TREE_OP_COLLECT_IP4_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../data/tree.h"

#include "../../ip.h"
#include "../../util/dynarray.h"


int ip4_tree_collect_addr_into (
   struct ip_tree* const restrict tree,
   struct dynarray* const restrict darr
);

struct dynarray* ip4_tree_collect_addr ( struct ip_tree* const restrict tree );

#endif  /* _HAVE_IP_TREE_OP_COLLECT_IP4_H_ */
