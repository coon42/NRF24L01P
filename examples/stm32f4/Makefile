BINPATH=/usr/bin

CC=$(BINPATH)/arm-none-eabi-gcc
AR=$(BINPATH)/arm-none-eabi-ar
LD=$(BINPATH)/arm-none-eabi-ld
OBJCOPY=$(BINPATH)/arm-none-eabi-objcopy
SIZE=$(BINPATH)/arm-none-eabi-size

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

vpath %.c . src lib/src/stdperiph
vpath %.a lib

CFLAGS  = -std=gnu99 -g -O2 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4
CFLAGS += -Iinc -Ilib -Ilib/inc
CFLAGS += -Ilib/inc/core -Ilib/inc/stdperiph
CFLAGS += -ffreestanding # -nostdlib # nostdlib parameter should be set!?

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

###################################################

OBJDIR=obj
BINDIR=bin
MAIN_SRCS  = main.c stm32f4xx_it.c system_stm32f4xx.c syscalls.c utils.c \
	lib/startup_stm32f4xx.s # add startup file to build

LIB_SRCS = misc.c stm32f4xx_dma.c stm32f4xx_rcc.c stm32f4xx_adc.c \
	stm32f4xx_exti.c stm32f4xx_rng.c stm32f4xx_can.c stm32f4xx_flash.c \
	stm32f4xx_rtc.c stm32f4xx_crc.c stm32f4xx_fsmc.c stm32f4xx_sdio.c \
	stm32f4xx_cryp_aes.c stm32f4xx_gpio.c stm32f4xx_spi.c \
	stm32f4xx_cryp.c stm32f4xx_hash.c stm32f4xx_syscfg.c \
	stm32f4xx_cryp_des.c stm32f4xx_hash_md5.c stm32f4xx_tim.c \
	stm32f4xx_cryp_tdes.c stm32f4xx_hash_sha1.c  stm32f4xx_usart.c \
	stm32f4xx_dac.c stm32f4xx_i2c.c stm32f4xx_wwdg.c \
	stm32f4xx_dbgmcu.c stm32f4xx_iwdg.c \
	stm32f4xx_dcmi.c stm32f4xx_pwr.c

PROJ_NAME=orgel
OUTPATH=bin

###################################################

MAIN_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(MAIN_SRCS))
LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(LIB_SRCS))

###################################################

.PHONY: proj flash clean

all: $(OUTPATH)/$(PROJ_NAME).elf

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -L$(OBJDIR) -lstm32f4 -lm

$(OBJDIR)/libstm32f4.a: $(LIB_OBJS)
	$(AR) -r $@ $(LIB_OBJS)

$(OUTPATH)/$(PROJ_NAME).elf: $(OBJDIR)/libstm32f4.a $(MAIN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -L. -Lobj -lstm32f4 -lm
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	$(SIZE) $@

flash: all
	st-flash write $(BINDIR)/$(PROJ_NAME).bin 0x8000000

proj: $(OUTPATH)/$(PROJ_NAME).elf

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)
	
