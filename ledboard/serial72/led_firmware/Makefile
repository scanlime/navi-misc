NAME=ledboard

target: download

$(NAME).hex: $(NAME).asm
	gpasm $(NAME).asm

download: $(NAME).hex set_cfg_bits
	picp /dev/ttyS1 PIC16F84 -s 40 -wp $(NAME).hex

set_cfg_bits:
	picp /dev/ttyS1 PIC16F84 -wc 0x3FF2
	touch set_cfg_bits

clean:
	rm -f $(NAME).hex $(NAME).cod $(NAME).lst set_cfg_bits
