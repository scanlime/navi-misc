ORIGINAL=~/COOKINGCOACH_VCKE00-clean.dsv
OUTPUT=VCKE.sav

patch: exploit.bin
	./cookinject.py $(ORIGINAL) $(OUTPUT) && hd $(OUTPUT) | head -n 20

exploit.o: exploit.s
	arm-eabi-as exploit.s -o exploit.o

exploit.bin: exploit.o
	arm-eabi-objdump -d exploit.o
	arm-eabi-objcopy exploit.o -O binary exploit.bin
