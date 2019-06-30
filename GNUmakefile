S := $(CURDIR)

SRC = $(S)/src
MK_INCLUDE = $(S)/mk

O := $(S)/obj
O_OBJ = $(O)/src

PHONY =

PHONY += all
all: ip-dedup

include $(MK_INCLUDE)/install_vars.mk
include $(MK_INCLUDE)/prj.mk

ifeq ($(STANDALONE),1)
include $(MK_INCLUDE)/datadir_standalone.mk
else
include $(MK_INCLUDE)/datadir_install.mk
endif

ifeq ("","$(IPDEDUP_DATADIR)")
CC_OPTS_EXTRA += -UIPDEDUP_DATADIR
else
CC_OPTS_EXTRA += -DIPDEDUP_DATADIR=\"$(IPDEDUP_DATADIR)\"
endif

include $(MK_INCLUDE)/warnflags_base.mk
ifeq ($(NO_WERROR),1)
include $(MK_INCLUDE)/warnflags_no_werror.mk
endif

ifeq ($(STATIC),1)
include $(MK_INCLUDE)/static.mk
endif

ifeq ($(HARDEN),1)
include $(MK_INCLUDE)/c_harden.mk
endif

include $(MK_INCLUDE)/compile_c.mk
include $(MK_INCLUDE)/obj_defs.mk

include $(MK_INCLUDE)/common_targets.mk


PHONY += ip-dedup
ip-dedup: $(O)/ip-dedup

$(O):
	$(MKDIRP) -- $@

$(O_OBJ):
	$(MKDIRP) -- $@

$(O_OBJ)/%.o: $(SRC)/%.c $(wildcard $(SRC)/%.h) | $(O_OBJ)
	$(MKDIRP) -- $(@D)
	$(COMPILE_C) $< -o $@

$(O)/ip-dedup: \
	$(foreach f,$(OBUNDLE_APP_IP_DEDUP),$(O_OBJ)/$(f).o $(wildcard $(SRC)/$(f).h)) | $(O)

	$(LINK_O) $(filter-out %.h,$^) -o $@


FORCE:

.PHONY: $(PHONY)
