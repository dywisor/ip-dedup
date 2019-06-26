#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tree.h"
#include "node.h"
#include "typedesc.h"


static int _ip_tree_init_null ( struct ip_tree* const restrict tree );


static int _ip_tree_init_null ( struct ip_tree* const restrict tree ) {
   tree->root = tree->tdesc->f_new_null();
   if ( tree->root == NULL ) { return -1; }

   return 0;
}


struct ip_tree* ip4_tree_new (void) {
   struct ip_tree* tree;

   tree = (struct ip_tree*) malloc ( sizeof *tree );
   if ( tree != NULL ) {
      tree->tdesc = &IP4_TREE_NODE_OPS;

      if ( _ip_tree_init_null ( tree ) != 0 ) {
         free ( tree );
         tree = NULL;
      }
   }

   return tree;
}


struct ip_tree* ip6_tree_new (void) {
   struct ip_tree* tree;

   tree = (struct ip_tree*) malloc ( sizeof *tree );
   if ( tree != NULL ) {
      tree->tdesc = &IP6_TREE_NODE_OPS;

      if ( _ip_tree_init_null ( tree ) != 0 ) {
         free ( tree );
         tree = NULL;
      }
   }

   return tree;
}


void ip_tree_destroy ( struct ip_tree** const restrict tree_ptr ) {
   const struct ip_tree_typedesc* tdesc;

   if ( *tree_ptr != NULL ) {
      tdesc = (*tree_ptr)->tdesc;

      tdesc->f_destroy ( &((*tree_ptr)->root) );
      (*tree_ptr)->tdesc = NULL;

      free ( *tree_ptr );
      *tree_ptr = NULL;
   }
}
