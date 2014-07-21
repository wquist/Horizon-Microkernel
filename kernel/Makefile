PROJECT_ROOT = ..

# Set up some path variables
ARCH_ROOT = arch/$(ARCH)
LIB_ROOT  = $(PROJECT_ROOT)/lib
BUILD_ROOT = build
OBJ_ROOT = $(BUILD_ROOT)/obj-$(ARCH)
DEP_ROOT = $(BUILD_ROOT)/dep-$(ARCH)
LIB_BUILD_ROOT = $(LIB_ROOT)/build

BINARY = $(BUILD_ROOT)/kernel-$(ARCH).elf

# Define the cross compiler and archiver - should be in format of ARCH-FORMAT-TOOL
# Only using elf for now, though, so it is not a variable
CROSS_CHAIN = /usr/local/cross/bin/$(ARCH)-elf
CC = $(CROSS_CHAIN)-gcc
LD = $(CROSS_CHAIN)-ld

# Enable highest optimization and the newest C standard, disable stdlib, and use strict error checking
# Also disable some unnecessary warnings (bitfield packing and initializer brace counts)
CFLAGS =                        \
	-O3                         \
	-std=c11                    \
	-fno-builtin                \
	-ffreestanding              \
	-nostdlib                   \
	-nostartfiles               \
	-nodefaultlibs              \
	-Wall                       \
	-Werror	                    \
	-Wno-missing-braces         \
	-Wno-unused-variable        \
	-Wno-unused-function        \
	-Wno-packed-bitfield-compat

# Each kernel architecture should define its own linker script
LDSCRIPT = $(ARCH_ROOT)/link.ld
LDFLAGS  = -T $(LDSCRIPT)

# Define global macros
DEFINES = _KERNEL_SOURCE _DEBUG

# The raw include directories - more can be added in an arch's "Makefile.inc"
INC_DIRS =                        \
	.                             \
	core                          \
	$(ARCH_ROOT)                  \
	$(LIB_ROOT)/libh/include      \
	$(LIB_ROOT)/libh/arch/$(ARCH)

# Each directory contains C or assembly files - "Makefile.inc" adds to this as well
SRC_DIRS =         \
	core           \
	core/ipc       \
	core/memory	   \
	core/multitask \
	core/system    \
	debug          \
	spec/elf       \
	spec/multiboot \
	spec/pic8259   \
	util           \
	$(ARCH_ROOT)

# The names of all libraries to link
LIBRARIES = h

# Filled in by "Makefile.inc" if needed
ASMC     =
ASMFLAGS =
ASMEXT   =

# Get any extra directories or options
-include $(ARCH_ROOT)/Makefile.inc

# Modify the raw macros to the GCC format
MACROS = $(addprefix -D, $(DEFINES))

# Modify the raw include directories to the format GCC expects
INCLUDES = $(addprefix -I, $(INC_DIRS))

# Setup a list of libraries for search directories and arch-specific library names
# Also get a list of the actual archive names, so the kernel rebuilds if a lib changes
LIB_NAMES  = $(addsuffix -$(ARCH), $(LIBRARIES))
LIB_PATHS  = $(addprefix $(LIB_BUILD_ROOT)/lib, $(LIBRARIES))
LIB_SEARCH = $(addprefix -L, $(LIB_PATHS))
LIBS       = $(addprefix -l, $(LIB_NAMES))
OBJLIB     = $(join $(LIB_PATHS), $(addprefix /lib, $(addsuffix .a, $(LIB_NAMES))))

# Mirror the directory structure in the build/ folder
OBJ_DIRS = $(addprefix $(OBJ_ROOT)/, $(SRC_DIRS))
DEP_DIRS = $(addprefix $(DEP_ROOT)/, $(SRC_DIRS))

# Find all C files, and create the corresponding O and D names
SRCC = $(foreach d,$(SRC_DIRS), $(wildcard $(d)/*.c))
OBJC = $(addprefix $(OBJ_ROOT)/, $(addsuffix .o, $(basename $(SRCC))))
DEPC = $(addprefix $(DEP_ROOT)/, $(addsuffix .d, $(basename $(SRCC))))

# Do the same with assembly files, if needed by the architecture
SRCASM =
OBJASM =
ifneq ($(ASMC),)
SRCASM = $(foreach d,$(SRC_DIRS), $(wildcard $(d)/*.$(ASMEXT)))
OBJASM = $(addprefix $(OBJ_ROOT)/, $(addsuffix .o, $(basename $(SRCASM))))
endif

DEPEND      = $(CC)   $(CFLAGS)  $(INCLUDES) $(MACROS) -MM -MP
COMPILE.c   = $(CC)   $(CFLAGS)  $(INCLUDES) $(MACROS) -c
COMPILE.asm = $(ASMC) $(ASMFLAGS)
LINK        = $(LD)   $(LDFLAGS)

.PHONY: all checkdirs depend obj clean distclean

all: depend $(BINARY)
checkdirs: $(DEP_DIRS) $(OBJ_DIRS)
depend: $(DEPC)
obj: $(OBJC) $(OBJASM)
distclean: clean

$(DEP_ROOT)/%.d:%.c | checkdirs
	@$(DEPEND) -MT build/obj-$(ARCH)/$*.o $< > $@
	@echo "CC [DEP] $<"

$(OBJ_ROOT)/%.o:%.c | checkdirs
	@$(COMPILE.c) $< -o $@
	@echo "CC $<"
$(OBJ_ROOT)/%.o:%.$(ASMEXT) | checkdirs
	@$(COMPILE.asm) $< -o $@
	@echo "CC [ASM] $<"

$(BINARY): $(OBJC) $(OBJASM) $(OBJLIB) $(LDSCRIPT)
	@$(LINK) $(OBJC) $(OBJASM) $(OBJLIB) -o $@ $(LIB_SEARCH) $(LIBS)
	@echo "LD $@"

$(OBJ_DIRS):
	@mkdir -p $@
$(DEP_DIRS):
	@mkdir -p $@

-include $(DEPC)

clean:
	@rm -rf $(OBJ_ROOT) $(BINARY)
	@echo "[RM] obj-$(ARCH) $(BINARY)"

distclean:
	@rm -rf $(DEP_ROOT)
	@echo "[RM] dep-$(ARCH)"
