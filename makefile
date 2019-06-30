S := ${.CURDIR}

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

PHONY += clean
clean:
	test ! -d ./src || find ./src -type f -name '*.o' -delete
	test ! -d ./src || find ./src -depth -type d -empty -delete
	test ! -f ./ip-dedup || rm -- ./ip-dedup


ODEP_IP_DEDUP := ${OBUNDLE_APP_IP_DEDUP:%=src/%.o}

.SUFFIXES: .c .o
.c.o:
	mkdir -p -- ${@D}
	${COMPILE_C} ${<} -o ${@}

ip-dedup: ${ODEP_IP_DEDUP}
	$(LINK_O) ${ODEP_IP_DEDUP} -o ${@}


.PHONY: ${PHONY}
