# SPDX-License-Identifier: AGPL-3.0-or-later

CPPFLAGS ?= -Wall -Wextra -Wpedantic -Wno-unused-parameter
CPPFLAGS := $(CPPFLAGS)

CFLAGS := $(CFLAGS)
CFLAGS += -std=c99

LDFLAGS := $(LDFLAGS)
LDFLAGS += -lhidapi-libusb

whisperer : whisperer.c
whisperer.c : cmd.c

clean:
	rm -f whisperer

.PHONY: clean
