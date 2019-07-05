#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "print.h"

#include "../ip_tree.h"
#include "../ip.h"
#include "../util/print_ip.h"
#include "../util/dynarray.h"


void fprint_ip4_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
   struct dynarray* darr;
   size_t k;

   darr = ip4_tree_collect_addr ( tree );
   if ( darr != NULL ) {
      dynarray_foreach ( darr, k ) {
         const struct ip4_addr_t* const addr = dynarray_get_as (
            darr, k, const struct ip4_addr_t*
         );

         fprint_ip4_net ( stream, addr );
      }

      dynarray_free_ptr ( &darr );
   }
}


void fprint_ip6_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
   struct dynarray* darr;
   size_t k;

   darr = ip6_tree_collect_addr ( tree );
   if ( darr != NULL ) {
      dynarray_foreach ( darr, k ) {
         const struct ip6_addr_t* const addr = dynarray_get_as (
            darr, k, const struct ip6_addr_t*
         );

         fprint_ip6_net ( stream, addr );
      }

      dynarray_free_ptr ( &darr );
   }
}
