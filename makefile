S := ${.CURDIR}

SRC = ${S}/src
MK_INCLUDE = ${S}/mk

O := ${S}/build
O_OBJ = ${O}/obj
O_BIN = ${O}/bin

all: ip-dedup

_WARNFLAGS   =
.ifdef NO_WERROR
_WARNFLAGS  += -Werror
_WARNFLAGS  += -Wno-unused-parameter -Wno-error=unused-parameter
_WARNFLAGS  += -Wno-unknown-pragmas  -Wno-error=unknown-pragmas
.endif

_WARNFLAGS  += -Wall -Wextra
_WARNFLAGS  += -Wwrite-strings -Wdeclaration-after-statement
_WARNFLAGS  += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
_WARNFLAGS  += -pedantic
_WARNFLAGS  += -Wtrampolines
_WARNFLAGS  += -Wunsafe-loop-optimizations
_WARNFLAGS  += -Wformat -Wno-format-extra-args -Wformat-security
_WARNFLAGS  += -Wformat-nonliteral -Wformat=2

.if defined(STATIC) && ${STATIC} == 1
_CC_OPT_STATIC = -static
.endif

ip-dedup: ${O_BIN}/ip-dedup


.include "mk/compile_c.mk"

.include "mk/obj_defs.mk"

ODEP_IP_DEDUP =
.for odef in ${OBUNDLE_APP_IP_DEDUP}
ODEP_IP_DEDUP += ${O_OBJ}/${odef}.o

${O_OBJ}/${odef}.o: ${SRC}/${odef}.c
	mkdir -p -- ${@D}
	${COMPILE_C} ${SRC}/${odef}.c -o ${@}

.endfor

${O_BIN}/ip-dedup: ${ODEP_IP_DEDUP}
	mkdir -p -- ${@D}
	$(LINK_O) ${ODEP_IP_DEDUP} -o ${@}
