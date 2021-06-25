PN := ip-dedup

S_FILES   = $(S)/files
S_IPLIST  = $(S_FILES)/list
S_SCRIPTS = $(S_FILES)/scripts

# whether to build a binary that will be run from the git repo
STANDALONE ?= 0

# whether to disable -Werror
NO_WERROR ?= 0

# whether to build a static binary
STATIC ?= 0

# pass some hardening flags to the compiler
#  Note that HARDEN==1 is not compatible with STATIC==1
HARDEN ?= 1

# whether to use OpenBSD's pledge(2) system call
# to restrict allowed operations
# NOTE: this is set in the main GNUmakefile/makefile
#USE_OPENBSD_PLEDGE ?= 0

# whether to include support for long options
#
#   Besides adding not-yet-documented long options,
#   this allows to mix positional arguments with options,
#   e.g. "ip-dedup <FILE> -6".
#
USE_LONGOPT ?= 0

CC_OPTS_EXTRA += -DUSE_LONGOPT=$(USE_LONGOPT)
