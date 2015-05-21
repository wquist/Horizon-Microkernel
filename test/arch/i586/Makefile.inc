ASMC     = nasm
ASMFLAGS = -f elf

OBJS += $(ARCH_ROOT)/build/crt0_min.o

.PHONY: all

all: $(ARCH_ROOT)/build/crt0_min.o

$(ARCH_ROOT)/build/crt0_min.o: $(ARCH_ROOT)/crt0_min.nasm
	$(ASMC) $(ASMFLAGS) $< -o $@
