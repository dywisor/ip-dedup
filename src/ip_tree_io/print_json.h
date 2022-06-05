#ifndef _HAVE_IP_TREE_IO_PRINT_JSON_H_
#define _HAVE_IP_TREE_IO_PRINT_JSON_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../ip_tree.h"


/**
 * Prints two IP trees (one being IPv4, the other one IPv6) in JSON format.
 *
 * Output format: JSON object,
 *                {
 *                  "inet": [
 *                    {
 *                      "network"  "<address>/<prefixlen>",
 *                      "address": "<address>",
 *                      "netmask": "<netmask",
 *                      "prefixlen": <prefixlen>
 *                    },
 *                    ...
 *                  ],
 *
 *                  "inet6": [
 *                    {
 *                      "network"  "<address>/<prefixlen>",
 *                      "address": "<address>",
 *                      "network_exploded": "<address_exploded>/<prefixlen>",
 *                      "address_exploded": "<address_exploded>",
 *                      "prefixlen": <prefixlen>
 *                    },
 *                    ...
 *                  ],
 *                }
 *
 * */
void fprint_ip_tree_json (
    FILE* const restrict stream,
    struct ip_tree* const restrict tree_v4,
    struct ip_tree* const restrict tree_v6
);



#endif
