S := $(CURDIR)

SRC = $(S)/src
MK_INCLUDE = $(S)/mk

O := $(S)/build
O_OBJ = $(O)/obj
O_BIN = $(O)/bin

all: ip-dedup


_WARNFLAGS   =
ifeq ($(NO_WERROR),)
_WARNFLAGS  += -Werror
_WARNFLAGS  += -Wno-unused-parameter -Wno-error=unused-parameter
_WARNFLAGS  += -Wno-unknown-pragmas  -Wno-error=unknown-pragmas
endif

_WARNFLAGS  += -Wall -Wextra
_WARNFLAGS  += -Wwrite-strings -Wdeclaration-after-statement
_WARNFLAGS  += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
_WARNFLAGS  += -pedantic
_WARNFLAGS  += -Wtrampolines
_WARNFLAGS  += -Wunsafe-loop-optimizations
_WARNFLAGS  += -Wformat -Wno-format-extra-args -Wformat-security
_WARNFLAGS  += -Wformat-nonliteral -Wformat=2

ifeq ($(STATIC),1)
_CC_OPT_STATIC = -static
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
