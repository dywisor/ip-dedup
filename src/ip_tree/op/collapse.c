#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "collapse.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"


static void _ip_tree_collapse (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct ip_tree_node* const restrict node
);


int ip_tree_collapse ( struct ip_tree* const restrict tree ) {
   if ( tree == NULL ) {
      errno = EINVAL;
      return -1;

   } else {
      _ip_tree_collapse ( tree->tdesc, tree->root );
      return 0;
   }
}


static void _ip_tree_collapse (
   const struct ip_tree_typedesc* const restrict tdesc,
   struct ip_tree_node* const restrict node
) {
   /* whether to mark this node as hot and consume subnets */
   bool can_collapse;

   if ( node->hot ) {
      /* collapse now, no recursion -- this should be a no-op */
      can_collapse = true;

   } else {
      /* collapse subnets first */
      if ( node->left != NULL ) {
         _ip_tree_collapse ( tdesc, node->left );  /* recursive */
      }

      if ( node->right != NULL ) {
         _ip_tree_collapse ( tdesc, node->right );  /* recursive */
      }

      /* collapse to supernet if both subnets are hot */
      can_collapse = (
         ip_tree_node_left_hot ( node )
         && ip_tree_node_right_hot ( node )
      );
   }

   if ( can_collapse ) {
      tdesc->f_destroy ( &(node->left) );
      tdesc->f_destroy ( &(node->right) );
      node->hot = true;
   }
}
