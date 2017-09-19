
# tools
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

# flags
CPU = arm926ej-s
CFLAGS = -mcpu=$(CPU) -gstabs -marm \
         -std=c99 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf -fno-builtin-strcpy -Wno-overlength-strings \
         -fno-builtin-exit -I.
ASFLAGS = -mcpu=$(CPU) -g

OBJS = startup.o test.o

include sdcardlib/build.mk

test.bin: $(OBJS) test.ld
	arm-none-eabi-ld -T test.ld $(OBJS) -o test.elf
	arm-none-eabi-objcopy -O binary test.elf test.bin

qemu:
	qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin

clean:
	rm -f $(OBJS) test.elf test.bin
