include ../Make.conf

OUT := $(BUILD)/coroa
IMG := $(BUILD)/Coroa

FLAGS =\
	-fno-omit-frame-pointer\
	-mgeneral-regs-only\
	-ffreestanding\
	-fno-builtin\
	-nostdlib\
	-T linker.ld\
	-static\
	-std=c23\
	-ggdb

FILES :=\
	lib.c\
	arch/start.S\
	arch/arch.c\
	arch/fdt.c

$(IMG):
	@mkdir -p $(BUILD)
	@$(CC) $(FLAGS) $(FILES) -o $(OUT)
	@objcopy -O binary $(OUT) $(IMG)

run: $(IMG)
	@qemu-system-aarch64\
		-kernel $(IMG)\
		-cpu cortex-a57\
		-nographic\
		-M virt\
		-m 256M