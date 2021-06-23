#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "print.h"

#include "../ip_tree.h"
#include "../ip.h"
#include "../util/print_ip.h"
#include "../util/dynarray.h"


/* print loop helpers definition */
static void _fprint_ip4_tree (
    ip4_print_func f_fprint_ip4,
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);

static void _fprint_ip6_tree (
    ip6_print_func f_fprint_ip6,
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
);


/* print functions */
void fprint_ip4_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
    _fprint_ip4_tree ( fprint_ip4_net, stream, tree );
}


void fprint_ip6_tree (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
    _fprint_ip6_tree ( fprint_ip6_net, stream, tree );
}


/* specific print loop for compact IPv6 tree output */
int fprint_ip6_tree_compact (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree
) {
   struct dynarray* darr;
   char addr_str [IP6_ADDR_STR_BUF_SIZE];
   size_t k;
   int ret;

   darr = ip6_tree_collect_addr ( tree );
   if ( darr == NULL ) { return -1; }

   ret = 0;

   for ( k = 0; (k < darr->len) && (ret == 0); k++ ) {
      const struct ip6_addr_t* const addr = dynarray_get_as (
         darr, k, const struct ip6_addr_t*
      );

      if ( ip6_addr_data_into_str ( &(addr->addr), addr_str ) != NULL ) {
         fprintf (
            stream,
            ("%s/" IP_PREFIXLEN_FMT "\n"),
            addr_str, addr->prefixlen
         );
      } else {
         ret = -1;
      }
   }

   memset ( addr_str, 0, IP6_ADDR_STR_BUF_SIZE );
   dynarray_free_ptr ( &darr );

   return ret;
}


/* print loop helpers */
static void _fprint_ip4_tree (
    ip4_print_func f_fprint_ip4,
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

         f_fprint_ip4 ( stream, addr );
      }

      dynarray_free_ptr ( &darr );
   }
}


static void _fprint_ip6_tree (
    ip6_print_func f_fprint_ip6,
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

         f_fprint_ip6 ( stream, addr );
      }

      dynarray_free_ptr ( &darr );
   }
}
