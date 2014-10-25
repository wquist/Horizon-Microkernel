# Set up some path variables
TARGET_ROOT = $(TARGET)
BUILD_ROOT  = build/$(TARGET)
OBJ_ROOT    = $(BUILD_ROOT)/obj-$(ARCH)

BINARY = $(BUILD_ROOT)/$(TARGET)-$(ARCH).a

# Define the cross compiler and archiver - should be in format of ARCH-FORMAT-TOOL
# Only using elf for now, though, so it is not a variable
CROSS_CHAIN = /usr/local/cross/bin/$(ARCH)-elf
CC = $(CROSS_CHAIN)-gcc
AR = $(CROSS_CHAIN)-ar

# Enable highest optimization and the newest C standard, disable stdlib, and use strict error checking
CFLAGS =           \
	-O3            \
	-std=c11       \
	-fno-builtin   \
	-ffreestanding \
	-nostdlib      \
	-nostartfiles  \
	-nodefaultlibs \
	-Wall          \
	-Werror        \
	-fno-tree-loop-distribute-patterns
ARFLAGS = rcs

# Use a default of include/ and src/, but allow a unique Makefile.inc to specify more
INC_DIRS = $(TARGET_ROOT)/include/
-include $(TARGET_ROOT)/Makefile.inc

# Each include directory must be in the format -I(inc) for GCC
INCLUDES = $(addprefix -I, $(INC_DIRS))
SOURCES  = $(sort $(wildcard $(TARGET_ROOT)/src/*/*.c))

# Set up the object paths inside OBJ_ROOT and extract directories for the checkdirs target
OBJECTS  = $(subst $(TARGET_ROOT), $(OBJ_ROOT), $(addsuffix .o, $(basename $(SOURCES))))
OBJ_DIRS = $(sort $(dir $(OBJECTS)))

COMPILE = $(CC) $(CFLAGS) $(INCLUDES) -c
PACKAGE = $(AR) $(ARFLAGS)

.PHONY: all obj checkdirs clean

all: $(BINARY)
checkdirs: $(OBJ_DIRS)
obj: $(OBJECTS)

$(OBJ_ROOT)/%.o:$(TARGET_ROOT)/%.c | checkdirs
	@$(COMPILE) $< -o $@
	@echo "CC $<"

$(BINARY): $(OBJECTS)
	@$(PACKAGE) $@ $^
	@echo "AR $@"

$(OBJ_DIRS):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ_ROOT) $(BINARY)
