PROJECT_ROOT = ..
ARCH_ROOT    = arch/$(ARCH)
LIB_ROOT     = $(PROJECT_ROOT)/lib

ifndef TARGET_ROOT
TARGET_ROOT = $(TARGET)-$(ARCH)
endif

ifneq (,$(wildcard $(TARGET_ROOT)/Makefile))
$(error $(TARGET)-$(ARCH) uses a target-specific Makefile)
endif

BINARY = build/$(TARGET)-$(ARCH)/$(TARGET).elf

CROSS_CHAIN = /usr/local/cross/bin/$(ARCH)-elf
CC = $(CROSS_CHAIN)-gcc
LD = $(CROSS_CHAIN)-ld

CFLAGS =           \
	-std=c99       \
	-fno-builtin   \
	-ffreestanding \
	-nostdlib      \
	-nostartfiles  \
	-nodefaultlibs \
	-Wall          \
	-Werror
LDFLAGS = -T $(ARCH_ROOT)/link.ld

LIBS =                        \
	-L $(LIB_ROOT)/build/libh \
	-l h-$(ARCH)              \

INCLUDES =                           \
	-I .                             \
	-I $(LIB_ROOT)/libh/include      \
	-I $(LIB_ROOT)/libh/arch/$(ARCH)

CSRC = $(wildcard $(TARGET_ROOT)/*.c)
OBJS = $(subst $(TARGET_ROOT), build/$(TARGET)-$(ARCH)/obj, $(addsuffix .o, $(basename $(CSRC))))

-include $(ARCH_ROOT)/Makefile.inc

.PHONY: all checkdirs clean

all: $(BINARY)
checkdirs: build/$(TARGET)-$(ARCH)/obj

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

build/$(TARGET)-$(ARCH)/obj/%.o: $(TARGET_ROOT)/%.c | checkdirs
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/$(TARGET)-$(ARCH)/obj:
	@mkdir -p $@

clean:
	@rm -rf build/$(TARGET)-$(ARCH)/obj $(BINARY)
