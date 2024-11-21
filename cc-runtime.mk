MAKEFLAGS += -rR
.SUFFIXES:

CC := cc
AR := ar

CFLAGS := -g -O2 -pipe
CPPFLAGS :=

override CFLAGS += \
    -Wall \
    -Wextra \
    -std=c11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -ffunction-sections \
    -fdata-sections \
    -fvisibility=hidden

override CPPFLAGS := \
    $(CPPFLAGS) \
    -MMD \
    -MP

override CFILES := $(shell find -L * -type f -name '*.c' | LC_ALL=C sort)
override OBJ := $(CFILES:.c=.c.o)
override HEADER_DEPS := $(CFILES:.c=.c.d)

$(info Building cc-runtime.a...)

.PHONY: all
all: cc-runtime.a

cc-runtime.a: $(OBJ)
	@$(AR) rcs $@ $(OBJ)
	@ls -la $@
	@echo "Done."

-include $(HEADER_DEPS)

%.c.o: %.c cc-runtime.mk
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f cc-runtime.a $(OBJ) $(HEADER_DEPS)
