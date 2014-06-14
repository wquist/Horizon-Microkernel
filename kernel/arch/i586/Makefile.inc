ASMC     = nasm
ASMFLAGS = -f elf
ASMEXT   = nasm

SRC_DIRS +=                  \
	$(ARCH_ROOT)/boot        \
	$(ARCH_ROOT)/sw          \
	$(ARCH_ROOT)/sw/cpu      \
	$(ARCH_ROOT)/sw/int      \
	$(ARCH_ROOT)/hw          \
	$(ARCH_ROOT)/hw/int      \
	$(ARCH_ROOT)/hw/int/apic \
	$(ARCH_ROOT)/hal         \
	$(ARCH_ROOT)/stub
