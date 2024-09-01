# SPDX-License-Identifier: AGPL-3.0-or-later

MAKEFLAGS += --no-builtin-variables --no-builtin-rules

CC ?= cc
CC := $(CC)

CPPFLAGS := $(CPPFLAGS)
CPPFLAGS += -I include

CFLAGS ?= -Wall -Wextra -Wpedantic -Wno-unused-parameter
CFLAGS := $(CFLAGS)
CFLAGS += -std=c99

LDFLAGS := $(LDFLAGS)

LDLIBS := $(LDLIBS)
LDLIBS += -lhidapi-libusb

directories.source := source

directories.build := build
directories.build.objects := $(directories.build)/objects
directories.build.targets := $(directories.build)/targets

directories.build.all := $(directories.build)/

objects.all :=

objects.all += whisperer.o whisperer/command.o whisperer/devices/ite/829x.o
objects.all := $(addprefix $(directories.build.objects)/, $(objects.all))
directories.build.all += $(dir $(objects.all))

targets.all :=

targets.whisperer := whisperer
targets.whisperer := $(addprefix $(directories.build.targets)/, $(targets.whisperer))
targets.all += $(targets.whisperer)
directories.build.all += $(dir $(targets.all))

$(targets.whisperer) : $(objects.all) | directories
	$(strip $(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^)

$(directories.build.objects)/%.o : $(directories.source)/%.c | directories
	$(strip $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<)

targets.phony :=

targets.phony += whisperer
whisperer: $(targets.whisperer)

targets.phony += all
all: whisperer

targets.phony += clean
clean:
	rm -rf $(directories.build)/

targets.phony += directories
directories:
	mkdir -p $(sort $(directories.build.all))

.PHONY: $(targets.phony)
.DEFAULT_TARGET := whisperer
