# compat bits for combined GNU/BSD make targets, do not use them in this file
S    := ${.CURDIR}
O     = .
O_OBJ = $(O)/src
# ---

PHONY =

PHONY += all
all: ip-dedup

.include "mk/install_vars.mk"
.include "mk/prj.mk"

STANDALONE ?= 0
.if ${STANDALONE} == 1
.include "mk/datadir_standalone.mk"
.else
.include "mk/datadir_install.mk"
.endif

.if !defined(IPDEDUP_DATADIR) || ${IPDEDUP_DATADIR} == ""
CC_OPTS_EXTRA += -UIPDEDUP_DATADIR
.else
CC_OPTS_EXTRA += -DIPDEDUP_DATADIR=\"$(IPDEDUP_DATADIR)\"
.endif

.include "mk/warnflags_base.mk"
.if !defined(NO_WERROR) || ${NO_WERROR} == ""
.include "mk/warnflags_no_werror.mk"
.endif

.if defined(STATIC) && ${STATIC} == 1
.include "mk/static.mk"
.endif

HARDEN ?= 1
.if ${HARDEN} == 1
.include "mk/c_harden.mk"
.endif

.include "mk/compile_c.mk"
.include "mk/obj_defs.mk"

.include "mk/common_targets.mk"


ODEP_IP_DEDUP := ${OBUNDLE_APP_IP_DEDUP:%=src/%.o}

.SUFFIXES: .c .o
.c.o:
	${MKDIRP} -- ${@D}
	${COMPILE_C} ${<} -o ${@}

ip-dedup: ${ODEP_IP_DEDUP}
	$(LINK_O) ${ODEP_IP_DEDUP} -o ${@}


.PHONY: ${PHONY}
