S := ${.CURDIR}

PHONY =

PHONY += all
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

.include "mk/compile_c.mk"
.include "mk/obj_defs.mk"

ODEP_IP_DEDUP := ${OBUNDLE_APP_IP_DEDUP:%=src/%.o}

.SUFFIXES: .c .o
.c.o:
	mkdir -p -- ${@D}
	${COMPILE_C} ${<} -o ${@}

ip-dedup: ${ODEP_IP_DEDUP}
	$(LINK_O) ${ODEP_IP_DEDUP} -o ${@}


.PHONY: ${PHONY}
