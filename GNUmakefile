S := $(CURDIR)

SRC = $(S)/src
MK_INCLUDE = $(S)/mk

O := $(S)/build
O_OBJ = $(O)/obj
O_BIN = $(O)/bin

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


ip-dedup: $(O_BIN)/ip-dedup

$(O):
	mkdir -p -- $@

$(O_BIN) $(O_OBJ): | $(O)
	mkdir -p -- $@

$(O_OBJ)/%.o: $(SRC)/%.c $(wildcard $(SRC)/%.h) | $(O_OBJ)
	mkdir -p -- $(@D)
	$(COMPILE_C) $< -o $@

$(O_BIN)/ip-dedup: \
	$(foreach f,$(OBUNDLE_APP_IP_DEDUP),$(O_OBJ)/$(f).o $(wildcard $(SRC)/$(f).h)) | $(O_BIN)

	$(LINK_O) $(filter-out %.h,$^) -o $@


FORCE:

.PHONY: $(PHONY)
