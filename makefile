S := ${.CURDIR}

# compat bits for combined GNU/BSD make targets, do not use them in this file
O     = .
O_OBJ = $(O)/src

PHONY =

PHONY += all
all: ip-dedup

.include "mk/warnflags_base.mk"
.ifdef NO_WERROR
.include "mk/warnflags_no_werror.mk"
.endif

.if defined(STATIC) && ${STATIC} == 1
.include "mk/static.mk"
.endif

.include "mk/compile_c.mk"
.include "mk/obj_defs.mk"

.include "mk/install_vars.mk"

.include "mk/common_targets.mk"


ODEP_IP_DEDUP := ${OBUNDLE_APP_IP_DEDUP:%=src/%.o}

.SUFFIXES: .c .o
.c.o:
	mkdir -p -- ${@D}
	${COMPILE_C} ${<} -o ${@}

ip-dedup: ${ODEP_IP_DEDUP}
	$(LINK_O) ${ODEP_IP_DEDUP} -o ${@}


.PHONY: ${PHONY}
