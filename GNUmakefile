S := $(CURDIR)

SRC = $(S)/src
MK_INCLUDE = $(S)/mk

O := $(S)/obj
O_OBJ = $(O)/src

PHONY =

PHONY += all
all: ip-dedup

include $(MK_INCLUDE)/warnflags_base.mk
ifeq ($(NO_WERROR),)
include $(MK_INCLUDE)/warnflags_no_werror.mk
endif

ifeq ($(STATIC),1)
include $(MK_INCLUDE)/static.mk
endif

include $(MK_INCLUDE)/compile_c.mk
include $(MK_INCLUDE)/obj_defs.mk

include $(MK_INCLUDE)/clean_target.mk

include $(MK_INCLUDE)/install_vars.mk
include $(MK_INCLUDE)/install_target.mk


PHONY += ip-dedup
ip-dedup: $(O)/ip-dedup

$(O):
	mkdir -p -- $@

$(O_OBJ):
	mkdir -p -- $@

$(O_OBJ)/%.o: $(SRC)/%.c $(wildcard $(SRC)/%.h) | $(O_OBJ)
	mkdir -p -- $(@D)
	$(COMPILE_C) $< -o $@

$(O)/ip-dedup: \
	$(foreach f,$(OBUNDLE_APP_IP_DEDUP),$(O_OBJ)/$(f).o $(wildcard $(SRC)/$(f).h)) | $(O)

	$(LINK_O) $(filter-out %.h,$^) -o $@


FORCE:

.PHONY: $(PHONY)
