#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "readfile.h"
#include "strutil.h"


static int _readfile_state_init_stream (
    struct readfile_state* const rstate,
    FILE* const input_stream,
    const bool close_on_free
);


void readfile_state_init_null ( struct readfile_state* const rstate ) {
    rstate->stream          = NULL;
    rstate->close_stream    = false;

    rstate->line_buf        = NULL;
    rstate->line_buf_size   = 0;

    rstate->line            = NULL;
    rstate->line_len        = 0;
}


void readfile_state_free_data ( struct readfile_state* const rstate ) {
    if ( rstate->line_buf != NULL ) {
        free ( rstate->line_buf = NULL );
    }

    if ( rstate->close_stream && (rstate->stream != NULL) ) {
        fclose ( rstate->stream );  /* retcode ignored */
    }

    readfile_state_init_null ( rstate );
}


static int _readfile_state_init_stream (
    struct readfile_state* const rstate,
    FILE* const input_stream,
    const bool close_on_free
) {
    readfile_state_init_null ( rstate );

    if ( input_stream == NULL ) {
        /* free() not necessary */
        return -1;

    } else {
        rstate->close_stream = close_on_free;
        rstate->stream       = input_stream;

        return 0;
    }
}


int readfile_init_stream (
    struct readfile_state* const rstate,
    FILE* const input_stream
) {
    return _readfile_state_init_stream ( rstate, input_stream, false );
}


int readfile_init_file (
    struct readfile_state* const rstate,
    const char* const input_file
) {
    return _readfile_state_init_stream (
        rstate,
        fopen ( input_file, "r" ),
        true
    );
}


int readfile_next (
    struct readfile_state* const rstate
) {
    ssize_t nread;

    nread = getline (
        &(rstate->line_buf), &(rstate->line_buf_size), rstate->stream
    );

    if ( nread < 0 ) {
        /* maybe check errno */
        return READFILE_RET_EOF;
    }

    rstate->line_len = (size_t) nread;
    rstate->line     = str_strip ( rstate->line_buf, &(rstate->line_len) );

    if ( *(rstate->line) == '\0' ) {
        return READFILE_RET_EMPTY_LINE;
    } else {
        return READFILE_RET_LINE;
    }
}


int readfile_next_effective (
    struct readfile_state* const rstate
) {
    int ret;

    do {
        ret = readfile_next ( rstate );

        switch ( ret ) {
            case READFILE_RET_EMPTY_LINE:
                break;

            default:
                return ret;
        }
    } while (1);
}
