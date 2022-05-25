MCU=cortex-m3
OBJCOPY=arm-none-eabi-objcopy
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
SIZE=arm-none-eabi-size
INC=-I../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
INC+=-I../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
INC+=-I../../SPL_stm32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
DEF=-DSTM32F10X_MD
DEF +=-DSYSCLK_FREQ_72MHz
CFLAGS=-mthumb -mcpu=$(MCU) -g -O0 -Wall $(DEF) $(INC)
ASFLAGS=-mthumb -mcpu=$(MCU) -g -Wall
LDFLAGS=-Tscript.ld
OBJ=main.o init.o startup.o
TARGET=blink
.PHONY: all clean

%.o:	%.c
	$(CC) -c -o $@ $< $(CFLAGS)
%.o:	src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
%.o:	asm/%.s
	$(CC) -c -o $@ $< $(ASFLAGS)
all:	$(OBJ)
	$(LD) $(LDFLAGS) -g  -o $(TARGET).elf  $(OBJ)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	$(SIZE)  $(TARGET).elf
install:
	st-flash  --reset write $(TARGET).bin 0x08000000
clean:
	@rm -v $(TARGET).elf $(TARGET).bin $(OBJ)

