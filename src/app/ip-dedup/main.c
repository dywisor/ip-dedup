#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sysexits.h>

#include "globals.h"
#include "version.h"

#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"
#include "../../ip_tree_io/print.h"

#include "../../util/dynarray.h"

/**
 * The actual main function.
 *
 * main() wraps this function and provides handling of global vars
 * and interpretation of exit codes.
 *
 * @param g
 * @param argc
 * @param argv
 *
 * @return
 * */
__attribute__((warn_unused_result))
static int main_inner (
   struct ipdedup_globals* const restrict g, int argc, char** argv
);


/**
 * Part of the inner main function, parses input files / stdin
 * and builds the IP trees.
 *
 * @param g
 * @param prog_name
 * @param argc
 * @param argv
 *
 * @return 0 on success, else non-zero (see PARSE_IP_RET status codes)
 * */
__attribute__((warn_unused_result))
static int main_inner_parse_input (
   struct ipdedup_globals* const restrict g
);

__attribute__((warn_unused_result))
static int main_interpret_parse_ret ( const int parse_ret );


/**
 * tree operations for the inner main function.
 * */
__attribute__((warn_unused_result))
static int main_run (
   struct ipdedup_globals* const restrict g
);


/**
 * Guess tree operations mode based on program name.
 *
 * @param prog_name  progam basename
 *
 * @return IPDEDUP_TREE_MODE
 * */
static int guess_tree_mode ( const char* const restrict prog_name );


/**
 * Determine the program's name from its path.
 *
 * @param prog_path
 *
 * @return prog name
 * */
static const char* get_prog_name ( const char* const restrict prog_path );


/**
 * Prints a message describing the program
 * and its usage to the given output stream.
 *
 * @param stream         output stream
 * @param prog_name      name of the program
 *
 * @return None (implicit)
 * */
static void print_description (
   FILE* const restrict stream,
   const char* const restrict prog_name
);


/**
 * Prints a message describing the program's usage to the given output stream.
 *
 * @param stream         output stream
 * @param prog_name      name of the program
 *
 * @return None (implicit)
 * */
static void print_usage (
   FILE* const restrict stream,
   const char* const restrict prog_name
);


int main ( int argc, char** argv ) {
   int ret;
   struct ipdedup_globals g;

   if ( ipdedup_globals_init ( &g ) != 0 ) {
      return EX_OSERR;
   }

   ret = main_inner ( &g, argc, argv );

   ipdedup_globals_free ( &g );

   if ( ret == 0 ) {
      return EXIT_SUCCESS;

   } else if ( ret < 0 ) {
      return EX_SOFTWARE;

   } else {
      return ret;
   }
}


#define MAIN_PRINT_USAGE_ERR_TERSE(_msg)  \
   do {  \
      fprintf ( stderr, "Error: %s\n", (_msg) );  \
   } while (0)

#define MAIN_PRINT_USAGE_ERR(_msg)  \
   do {  \
      fprintf ( stderr, "Error: %s\n\n", (_msg) ); \
      print_usage ( stderr, g->prog_name ); \
   } while (0)


static int main_interpret_parse_ret ( const int parse_ret ) {
   switch ( parse_ret ) {
      case PARSE_IP_RET_SUCCESS:
         return 0;

      case PARSE_IP_RET_BAD_INFILE_DUP:
         MAIN_PRINT_USAGE_ERR_TERSE ( "will not read stdin twice." );
         return EX_USAGE;

      case PARSE_IP_RET_BAD_INFILE:
         return EX_USAGE;

      case PARSE_IP_RET_READ_ERR:
         return EX_IOERR;

      case PARSE_IP_RET_FAIL:
         return EX_SOFTWARE;

      case PARSE_IP_RET_INVALID:
         return EX_DATAERR;

      case PARSE_IP_RET_DID_NOT_TRY:
      case PARSE_IP_RET_NOT_IMPLEMENTED:
      case PARSE_IP_RET_EOF:
         return EX_SOFTWARE;

      default:
         return EX_SOFTWARE;
   }
}

static int main_inner_parse_input (
   struct ipdedup_globals* const restrict g
) {
   int parse_ret;

   if ( g->infiles == NULL ) {
      parse_ret = ip_tree_builder_parse_stream_do_insert (
         g->tree_builder, stdin, g->want_keep_going
      );

   } else {
      parse_ret = ip_tree_builder_parse_files_do_insert (
         g->tree_builder, g->infiles, g->want_keep_going
      );
      dynarray_free_ptr ( &(g->infiles) );
   }

   return main_interpret_parse_ret ( parse_ret );
}


static int main_inner (
   struct ipdedup_globals* const restrict g, int argc, char** argv
) {
   static const char* const PROG_OPTIONS = "46ahiko:";

   int opt;

   g->prog_name = get_prog_name ( argv[0] );

   while ( ( opt = getopt ( argc, argv, PROG_OPTIONS ) ) != -1 ) {
      switch ( opt ) {
         case '4':
            g->tree_mode = IPDEDUP_TREE_MODE_IPV4;
            break;

         case '6':
            g->tree_mode = IPDEDUP_TREE_MODE_IPV6;
            break;

         case 'a':
            g->tree_mode = IPDEDUP_TREE_MODE_MIXED;
            break;

         case 'h':
            print_description ( stdout, g->prog_name );
            return 0;

         case 'i':
            g->want_invert = true;
            break;

         case 'k':
            g->want_keep_going = true;
            break;

         case 'o':
            if ( (optarg == NULL) || (*optarg == '\0') ) {
               MAIN_PRINT_USAGE_ERR ( "-o needs a non-empty argument." );
               return EX_USAGE;

            } else if ( (*optarg == '-') && (*(optarg + 1) == '\0') ) {
               /* switch to stdout */
               g->outfile = NULL;

            } else {
               g->outfile = optarg;
            }
            break;

         default:
            print_usage ( stderr, g->prog_name );
            return EX_USAGE;
      }
   }

   if ( optind < argc ) {
      g->infiles = new_dynarray ( (argc - optind) );
      if ( g->infiles == NULL ) { return EX_OSERR; }

      dynarray_set_data_readonly ( g->infiles );

      for ( opt = optind; opt < argc; opt++ ) {
         const char* const arg = argv[opt];

         if ( (arg == NULL) || (*arg == '\0') ) {
            MAIN_PRINT_USAGE_ERR ( "expected non-empty positional argument." );
            return EX_USAGE;

         } else if ( dynarray_append ( g->infiles, (void*) arg ) != 0 ) {
            return EX_OSERR;
         }
      }
   }

   if ( g->tree_mode == IPDEDUP_TREE_MODE_NONE ) {
      g->tree_mode = guess_tree_mode ( g->prog_name );
   }

   /* initialize tree builder and main dispatcher,
    * depending on tree mode */
   g->tree_builder = ip_tree_builder_new ( g->tree_mode );

   if ( g->tree_builder == NULL ) {
      perror ( "Failed to initialize ip tree data structures" );
      /* possibly EX_SOFTWARE if tree_mode was not valid */
      return EX_OSERR;
   }

   /* initialize tree view */
   ipdedup_globals_init_tree_view ( g );

   /* dispatch */
   return main_run ( g );
}


static int main_run (
   struct ipdedup_globals* const restrict g
) {
   int ret;

   /* parse input */
   ret = main_inner_parse_input ( g );  /* uses optind */
   if ( ret != 0 ) {
      MAIN_PRINT_USAGE_ERR ( "Failed to parse input" );
      return ret;
   }

   /* IPv4 operations */
   if ( g->tree_v4 != NULL ) {
      /* collapse tree */
      ip_tree_collapse ( g->tree_v4 );

      /* invert if requested */
      if ( g->want_invert ) {
         if ( ip_tree_invert ( g->tree_v4 ) != 0 ) {
            return EX_SOFTWARE;
         }
      }
   }

   /* IPv6 operations */
   if ( g->tree_v6 != NULL ) {
      /* collapse tree */
      ip_tree_collapse ( g->tree_v6 );

      /* invert if requested */
      if ( g->want_invert ) {
         if ( ip_tree_invert ( g->tree_v6 ) != 0 ) {
            return EX_SOFTWARE;
         }
      }
   }

   /* open outstream */
   if ( g->outfile == NULL ) {
      g->close_outstream = false;
      g->outstream = stdout;

   } else {
      g->close_outstream = true;
      g->outstream = fopen ( g->outfile, "w" );
      if ( g->outstream == NULL ) { return EX_CANTCREAT; }
   }

   /* print */
   if ( g->tree_v4 != NULL ) { fprint_ip4_tree ( g->outstream, g->tree_v4 ); }
   if ( g->tree_v6 != NULL ) { fprint_ip6_tree ( g->outstream, g->tree_v6 ); }

   return 0;
}


#undef MAIN_PRINT_USAGE_ERR
#undef MAIN_PRINT_USAGE_ERR_TERSE

static int guess_tree_mode ( const char* const restrict prog_name ) {
   const char* s;

   if ( prog_name != NULL ) {
      s = prog_name;

      /* ip4, ipv4 | ip6, ipv6 */
      if ( (*s == 'i') && (*(s + 1) == 'p') ) {
         s += 2;
         if ( *s == 'v' ) { s++; }

         switch ( *s ) {
            case '4':
               return IPDEDUP_TREE_MODE_IPV4;

            case '6':
               return IPDEDUP_TREE_MODE_IPV6;
         }
      }
   }

   return IPDEDUP_TREE_MODE_MIXED;
}


/* POSIX basename() may modify input argument, use own impl */
static const char* get_prog_name ( const char* const restrict prog_path ) {
   const char* prog_name;
   const char* s;

   if ( prog_path == NULL ) { return prog_path; }

   prog_name = prog_path;

   for ( s = prog_path; ; s++ ) {
      switch ( *s ) {
         case '\0':
            return ( ( *prog_name == '\0' ) ? prog_path : prog_name );

         case '/':
            prog_name = s + 1;
            break;
      }
   }
}


static void print_description (
   FILE* const restrict stream,
   const char* const restrict prog_name
) {
   fprintf (
      stream,
      (
         "%s %u.%u%s\n"
         "\n"
         "  Merges and aggregates IPv4/IPv6 addresses/networks and writes\n"
         "  the resulting networks in <addr>/<prefixlen> notation to stdout.\n"
         "\n"
         "  The output will be sorted. When operating in mixed mode,\n"
         "  IPv4 networks will be written before any IPv6 networks.\n"
         "\n"
         "  The input format is one network address per line,\n"
         "  optionally followed by a slash char '/' and a prefixlen.\n"
         "  If omitted, the prefixlen is guessed based on the number\n"
         "  of non-zero blocks in the network address.\n"
         "\n"
      ),
      IPDEDUP_NAME,
      IPDEDUP_VER_MAJOR,
      IPDEDUP_VER_MINOR,
      ((IPDEDUP_VER_SUFFIX != NULL) ? IPDEDUP_VER_SUFFIX : "")
   );

   print_usage ( stream, prog_name );
}


static void print_usage (
   FILE* const restrict stream,
   const char* const restrict prog_name
) {
   fprintf (
      stream,
      (
         "Usage:\n"
         "  %s {-4|-6|-a|-h|-i|-k|-o <FILE>} [<FILE>...]\n"
         "\n"
         "Options:\n"
         "  -4           IPv4 mode\n"
         "  -6           IPv6 mode\n"
         "  -a           IPv4/IPv6 mixed mode\n"
         "  -h           print this help message and exit\n"
         "  -i           invert networks\n"
         "  -k           skip invalid input instead of exiting with non-zero code\n"
         "  -o <FILE>    write output to <FILE> instead of stdout\n"
         "\n"
         "Positional Arguments:\n"
         "  FILE...      read networks from files instead of stdin\n"
         "\n"
         "Exit Codes:\n"
         "  %3d          success\n"
         "  %3d          unspecified error\n"
         "  %3d          usage error\n"
         "  %3d          failed to parse input network specification\n"
         "  %3d          software logic broken\n"
         "  %3d          failed to get system resources\n"
         "  %3d          failed to open output file\n"
         "  %3d          failed to open input file\n"
         "\n"
         "Notes:\n"
         "  For each line in the input (files),\n"
         "  leading and trailing whitespace is removed.\n"
         "  Empty lines, possibly caused by this conversion, are silently ignored.\n"
         "\n"
         "  The keep-going mode (-k) allows for invalid input.\n"
         "  However, if no input network specification could be parsed\n"
         "  but at least one was seen, then %s exits non-zero (%d).\n"
         "  This rules applies to each individual input file.\n"
      ),

      /* Usage: */
      prog_name,

      /* Options: */

      /* Arguments: */

      /* Exit Codes: */
      EXIT_SUCCESS,
      EXIT_FAILURE,
      EX_USAGE,
      EX_DATAERR,
      EX_SOFTWARE,
      EX_OSERR,
      EX_CANTCREAT,
      EX_IOERR,

      /* Notes: */
      prog_name,
      EX_DATAERR
   );
}
