include ../Make.conf

OUT := Coroa

CFLAGS =\
	--target=aarch64-none-elf\
	-fno-omit-frame-pointer\
	-mgeneral-regs-only\
	-ffreestanding\
	-fno-builtin\
	-std=c23\
	-Wall\
	-Wextra\
	-ggdb

LFLAGS :=\
	-T linker.ld\
	-nostdlib\
	-static

FILES :=\
	lib.c\
	arch/start.S\
	arch/arch.c\
	arch/fdt.c

HFILES :=\
	k.h\
	Coroa.txt\
	arch/arch.h

OFILES := $(addsuffix .o,$(basename $(FILES)))

$(OUT): $(OFILES)
	@mkdir -p $(BUILD)
	@$(LD) $(LFLAGS) $(OFILES) -o $(OUT)
	@objcopy -O binary $(OUT)

$(OFILES): $(HFILES)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	@$(CC) -c $< -o $@

run: $(OUT)
	@qemu-system-aarch64\
		-kernel $(OUT)\
		-cpu cortex-a57\
		-nographic\
		-M virt\
		-m 256M

clean:
	@rm -f $(OUT) $(OFILES)