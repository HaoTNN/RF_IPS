################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../Mirf \
../Visual\ Micro/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Mirf/Mirf.cpp \
../Mirf/MirfHardwareSpiDriver.cpp \
../Mirf/MirfSpiDriver.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Mirf/Mirf.o \
Mirf/MirfHardwareSpiDriver.o \
Mirf/MirfSpiDriver.o

OBJS_AS_ARGS +=  \
Mirf/Mirf.o \
Mirf/MirfHardwareSpiDriver.o \
Mirf/MirfSpiDriver.o

C_DEPS +=  \
Mirf/Mirf.d \
Mirf/MirfHardwareSpiDriver.d \
Mirf/MirfSpiDriver.d

C_DEPS_AS_ARGS +=  \
Mirf/Mirf.d \
Mirf/MirfHardwareSpiDriver.d \
Mirf/MirfSpiDriver.d

OUTPUT_FILE_PATH +=RF_IPS.elf

OUTPUT_FILE_PATH_AS_ARGS +=RF_IPS.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler







Mirf/%.o: ../Mirf/%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 3.4.2
	$(QUOTE)J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -D__AVR_ATmega32u4__ -D__AVR_ATmega32U4__ -DARDUINO=101 -DARDUINO_MAIN -DF_CPU=16000000L -D__AVR__ -D__cplusplus  -I"J:\arduino-1.0.5-r2\hardware\arduino\cores\arduino" -I"J:\arduino-1.0.5-r2\hardware\arduino\variants\leonardo" -I"J:\arduino-1.0.5-r2\libraries\SPI" -I"J:\arduino-1.0.5-r2\libraries\SPI\utility" -I"j:\arduino-1.0.5-r2\hardware\tools\avr\avr\include" -I"j:\arduino-1.0.5-r2\hardware\tools\avr\avr\include\avr" -I"j:\arduino-1.0.5-r2\hardware\tools\avr\avr" -I"j:\arduino-1.0.5-r2\hardware\tools\avr\lib\gcc\avr\4.3.2\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega32u4 -c -include=arduino.h -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 3.4.2
	$(QUOTE)J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="RF_IPS.map" -Wl,--start-group  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega32u4 
	@echo Finished building target: $@
	"J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "RF_IPS.elf" "RF_IPS.hex"
	"J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "RF_IPS.elf" "RF_IPS.eep" || exit 0
	"J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "RF_IPS.elf" > "RF_IPS.lss"
	"J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "RF_IPS.elf" "RF_IPS.srec"
	"J:\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-size.exe" "RF_IPS.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "RF_IPS.elf" "RF_IPS.a" "RF_IPS.hex" "RF_IPS.lss" "RF_IPS.eep" "RF_IPS.map" "RF_IPS.srec"
	