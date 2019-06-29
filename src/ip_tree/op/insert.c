#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "insert.h"

#include "../data/node.h"
#include "../data/typedesc.h"
#include "../data/tree.h"

#include "../../ip.h"

/* actual insert function() - no NULL check guards */
static int _ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
);


int ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
) {
   if ( (tree == NULL) || (var_addr == NULL) ) {
      errno = EINVAL;
      return -1;
   } else {
      return _ip_tree_insert ( tree, var_addr );
   }
}


int ip4_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip4_addr_t* const restrict addr
) {
   ip_addr_variant_t var_addr;

   if ( addr == NULL ) {
      errno = EINVAL;
      return -1;

   } else {
      var_addr.v4.addr      = addr->addr;
      var_addr.v4.prefixlen = addr->prefixlen;

      return ip_tree_insert ( tree, &var_addr );
   }
}


int ip6_tree_insert (
   struct ip_tree* const restrict tree,
   const struct ip6_addr_t* const restrict addr
) {
   ip_addr_variant_t var_addr;

   if ( addr == NULL ) {
      errno = EINVAL;
      return -1;

   } else {
      var_addr.v6.addr      = addr->addr;
      var_addr.v6.prefixlen = addr->prefixlen;

      return ip_tree_insert ( tree, &var_addr );
   }
}


static int _ip_tree_insert (
   struct ip_tree* const restrict tree,
   const ip_addr_variant_t* const restrict var_addr
) {
   /* determine prefixlen == tree depth of var_addr */
   const ip_prefixlen_t prefixlen = (
      tree->tdesc->f_get_addr_prefixlen ( var_addr )
   );

   /*
    * index 0..(MAX_PREFIXLEN - 1) that indicates how many bits are set
    * in the netmask of the current node.
    * This is equivalent to the depth in the data structure.
    * */
   ip_prefixlen_t cur_prefixpos;

   /* current node in the subnet path - shared, do not free */
   struct ip_tree_node* cur_node;

   /* pointer to the next node in the subnet path, child node of cur_node */
   /* shared, do not free */
   struct ip_tree_node* sub_node;

   /* prefixlen should be within valid range (always >= 0 due to uint) */
   if ( prefixlen > (tree->tdesc->max_prefixlen) ) {
      errno = EINVAL;
      return -1;
   }

   /*
    * create nodes as needed, mark them as intermediate for now
    *
    * start at level 0, cur_node points to the target node after this loop
    * */
   cur_node = tree->root;
   for ( cur_prefixpos = 0; cur_prefixpos < prefixlen; cur_prefixpos++ ) {
      if ( cur_node->hot ) {
         /*
          * addr to be inserted overshadowed by hot node
          * (at upper level or target node already hot)
          * */
         return 0;

      } else if (
         ! (tree->tdesc->f_check_addr_bit_set) ( var_addr, cur_prefixpos + 1 )
      ) {
         /* bit is unset -> follow left path, create if necessary */
         sub_node = cur_node->left;
         if ( sub_node == NULL ) {

            sub_node = tree->tdesc->f_new_child ( cur_node, false, false );
            if ( sub_node == NULL ) { return -1; }
            cur_node->left = sub_node;
         }

      } else {
         /* bit is set -> follow right path, create if necessary */
         sub_node = cur_node->right;
         if ( sub_node == NULL ) {
            sub_node = tree->tdesc->f_new_child ( cur_node, true, false );
            if ( sub_node == NULL ) { return -1; }
            cur_node->right = sub_node;
         }
      }

      cur_node = sub_node;
   }

   /* mark most recent node as hot */
   cur_node->hot = true;

   /* purge intermediate child nodes (if they exist) */
   tree->tdesc->f_destroy ( &(cur_node->left) );
   tree->tdesc->f_destroy ( &(cur_node->right) );

   return 0;
}
