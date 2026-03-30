##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=ucosR
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=/Users/sergey/projloc/ucosR
ProjectPath            :=/Users/sergey/projloc/ucosR
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=sergey
Date                   :=29/03/2026
CodeLitePath           :=/Users/sergey/.codelite
LinkerName             :=/Applications/arm/bin/arm-none-eabi-gcc
SharedObjectLinkerName :=/Applications/arm/bin/arm-none-eabi-g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)__SAMD51J20A__ 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="ucosR.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            := -mthumb -Wl,-Map=$(ProjectName).map --specs=nosys.specs -Wl,--gc-sections -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -TucosR_1M_256k.ld
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)bios $(IncludeSwitch)bios/cmsis $(IncludeSwitch)kernel $(IncludeSwitch)kernel/include $(IncludeSwitch)rshell $(IncludeSwitch)llfs $(IncludeSwitch)ff $(IncludeSwitch)sd_mmc $(IncludeSwitch)fonts $(IncludeSwitch)tusb $(IncludeSwitch)bios/samd51 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)m 
ArLibs                 :=  "m" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := /Applications/arm/bin/arm-none-eabi-ar rcu
CXX      := /Applications/arm/bin/arm-none-eabi-g++
CC       := /Applications/arm/bin/arm-none-eabi-gcc
CXXFLAGS :=  -O1 -Wall $(Preprocessors)
CFLAGS   := -mthumb -ffunction-sections -fdata-sections -mcpu=cortex-m4 -std=gnu17 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O1 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /Applications/arm/bin/arm-none-eabi-as


##
## User defined environment variables
##
CodeLiteDir:=/Applications/codelite.app/Contents/SharedSupport/
shell:=zsh
Objects0=$(IntermediateDirectory)/bios_dmactrl.c$(ObjectSuffix) $(IntermediateDirectory)/bios_tstring.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_usbd_control.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_tasks.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_usb_descriptors.c$(ObjectSuffix) $(IntermediateDirectory)/fonts_font_zx8x8.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_port.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_list.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_stream_buffer.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_event_groups.c$(ObjectSuffix) \
	$(IntermediateDirectory)/kernel_croutine.c$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/fonts_font_speedway8x8.c$(ObjectSuffix) $(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(ObjectSuffix) $(IntermediateDirectory)/llfs_llfs.c$(ObjectSuffix) $(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_uterm.c$(ObjectSuffix) $(IntermediateDirectory)/bios_i2c_port.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_editline.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_iface_sys.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_msc_disk.c$(ObjectSuffix) \
	$(IntermediateDirectory)/rshell_iface_util.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_sys_config.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_cdc_device.c$(ObjectSuffix) $(IntermediateDirectory)/bios_bsp.c$(ObjectSuffix) $(IntermediateDirectory)/bios_sys_sercom.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_commandline.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_rshell.c$(ObjectSuffix) $(IntermediateDirectory)/bios_lcd.c$(ObjectSuffix) $(IntermediateDirectory)/bios_syscalls.c$(ObjectSuffix) $(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(ObjectSuffix) \
	$(IntermediateDirectory)/tusb_usbd.c$(ObjectSuffix) $(IntermediateDirectory)/src_ucosR.c$(ObjectSuffix) $(IntermediateDirectory)/bios_icons.c$(ObjectSuffix) $(IntermediateDirectory)/ff_ffsystem.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_queue.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_tusb.c$(ObjectSuffix) $(IntermediateDirectory)/ff_ffunicode.c$(ObjectSuffix) $(IntermediateDirectory)/ff_fat.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_timers.c$(ObjectSuffix) $(IntermediateDirectory)/src_main.c$(ObjectSuffix) \
	$(IntermediateDirectory)/src_iface_sd.c$(ObjectSuffix) $(IntermediateDirectory)/src_startup.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_colours.c$(ObjectSuffix) $(IntermediateDirectory)/rshell_red.c$(ObjectSuffix) $(IntermediateDirectory)/ff_diskio.c$(ObjectSuffix) $(IntermediateDirectory)/sd_mmc_sd_mmc.c$(ObjectSuffix) $(IntermediateDirectory)/fonts_font_rimer6x8.c$(ObjectSuffix) 

Objects2=$(IntermediateDirectory)/tusb_dcd_samd.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_tusb_fifo.c$(ObjectSuffix) $(IntermediateDirectory)/kernel_heap_4r.c$(ObjectSuffix) \
	$(IntermediateDirectory)/tusb_msc_device.c$(ObjectSuffix) $(IntermediateDirectory)/ff_ff.c$(ObjectSuffix) $(IntermediateDirectory)/tusb_tusb_samd51.c$(ObjectSuffix) $(IntermediateDirectory)/fonts_font_serif8x8.c$(ObjectSuffix) $(IntermediateDirectory)/bios_graph.c$(ObjectSuffix) $(IntermediateDirectory)/bios_keyboard.c$(ObjectSuffix) $(IntermediateDirectory)/fonts_font_rimer8x12.c$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) $(Objects2) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	@echo $(Objects2) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	/Applications/ARM/bin/arm-none-eabi-size Release/ucosR
	/Applications/ARM/bin/arm-none-eabi-objcopy -O ihex Release/ucosR Release/ucosR.hex
	./makelib
	@echo Done

MakeIntermediateDirs:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)


$(IntermediateDirectory)/.d:
	@test -d $(ConfigurationName) || $(MakeDirCommand) $(ConfigurationName)

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/bios_dmactrl.c$(ObjectSuffix): bios/dmactrl.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_dmactrl.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_dmactrl.c$(DependSuffix) -MM bios/dmactrl.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/dmactrl.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_dmactrl.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_dmactrl.c$(PreprocessSuffix): bios/dmactrl.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_dmactrl.c$(PreprocessSuffix) bios/dmactrl.c

$(IntermediateDirectory)/bios_tstring.c$(ObjectSuffix): bios/tstring.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_tstring.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_tstring.c$(DependSuffix) -MM bios/tstring.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/tstring.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_tstring.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_tstring.c$(PreprocessSuffix): bios/tstring.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_tstring.c$(PreprocessSuffix) bios/tstring.c

$(IntermediateDirectory)/tusb_usbd_control.c$(ObjectSuffix): tusb/usbd_control.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_usbd_control.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_usbd_control.c$(DependSuffix) -MM tusb/usbd_control.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/usbd_control.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_usbd_control.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_usbd_control.c$(PreprocessSuffix): tusb/usbd_control.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_usbd_control.c$(PreprocessSuffix) tusb/usbd_control.c

$(IntermediateDirectory)/kernel_tasks.c$(ObjectSuffix): kernel/tasks.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_tasks.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_tasks.c$(DependSuffix) -MM kernel/tasks.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/tasks.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_tasks.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_tasks.c$(PreprocessSuffix): kernel/tasks.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_tasks.c$(PreprocessSuffix) kernel/tasks.c

$(IntermediateDirectory)/tusb_usb_descriptors.c$(ObjectSuffix): tusb/usb_descriptors.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_usb_descriptors.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_usb_descriptors.c$(DependSuffix) -MM tusb/usb_descriptors.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/usb_descriptors.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_usb_descriptors.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_usb_descriptors.c$(PreprocessSuffix): tusb/usb_descriptors.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_usb_descriptors.c$(PreprocessSuffix) tusb/usb_descriptors.c

$(IntermediateDirectory)/fonts_font_zx8x8.c$(ObjectSuffix): fonts/font_zx8x8.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fonts_font_zx8x8.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fonts_font_zx8x8.c$(DependSuffix) -MM fonts/font_zx8x8.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/fonts/font_zx8x8.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fonts_font_zx8x8.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fonts_font_zx8x8.c$(PreprocessSuffix): fonts/font_zx8x8.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fonts_font_zx8x8.c$(PreprocessSuffix) fonts/font_zx8x8.c

$(IntermediateDirectory)/kernel_port.c$(ObjectSuffix): kernel/port.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_port.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_port.c$(DependSuffix) -MM kernel/port.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/port.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_port.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_port.c$(PreprocessSuffix): kernel/port.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_port.c$(PreprocessSuffix) kernel/port.c

$(IntermediateDirectory)/kernel_list.c$(ObjectSuffix): kernel/list.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_list.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_list.c$(DependSuffix) -MM kernel/list.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/list.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_list.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_list.c$(PreprocessSuffix): kernel/list.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_list.c$(PreprocessSuffix) kernel/list.c

$(IntermediateDirectory)/kernel_stream_buffer.c$(ObjectSuffix): kernel/stream_buffer.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_stream_buffer.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_stream_buffer.c$(DependSuffix) -MM kernel/stream_buffer.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/stream_buffer.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_stream_buffer.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_stream_buffer.c$(PreprocessSuffix): kernel/stream_buffer.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_stream_buffer.c$(PreprocessSuffix) kernel/stream_buffer.c

$(IntermediateDirectory)/kernel_event_groups.c$(ObjectSuffix): kernel/event_groups.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_event_groups.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_event_groups.c$(DependSuffix) -MM kernel/event_groups.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/event_groups.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_event_groups.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_event_groups.c$(PreprocessSuffix): kernel/event_groups.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_event_groups.c$(PreprocessSuffix) kernel/event_groups.c

$(IntermediateDirectory)/kernel_croutine.c$(ObjectSuffix): kernel/croutine.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_croutine.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_croutine.c$(DependSuffix) -MM kernel/croutine.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/croutine.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_croutine.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_croutine.c$(PreprocessSuffix): kernel/croutine.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_croutine.c$(PreprocessSuffix) kernel/croutine.c

$(IntermediateDirectory)/fonts_font_speedway8x8.c$(ObjectSuffix): fonts/font_speedway8x8.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fonts_font_speedway8x8.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fonts_font_speedway8x8.c$(DependSuffix) -MM fonts/font_speedway8x8.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/fonts/font_speedway8x8.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fonts_font_speedway8x8.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fonts_font_speedway8x8.c$(PreprocessSuffix): fonts/font_speedway8x8.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fonts_font_speedway8x8.c$(PreprocessSuffix) fonts/font_speedway8x8.c

$(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(ObjectSuffix): llfs/llfs_vol_eeprom.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(ObjectSuffix) -MF$(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(DependSuffix) -MM llfs/llfs_vol_eeprom.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/llfs/llfs_vol_eeprom.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(PreprocessSuffix): llfs/llfs_vol_eeprom.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/llfs_llfs_vol_eeprom.c$(PreprocessSuffix) llfs/llfs_vol_eeprom.c

$(IntermediateDirectory)/llfs_llfs.c$(ObjectSuffix): llfs/llfs.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/llfs_llfs.c$(ObjectSuffix) -MF$(IntermediateDirectory)/llfs_llfs.c$(DependSuffix) -MM llfs/llfs.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/llfs/llfs.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/llfs_llfs.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/llfs_llfs.c$(PreprocessSuffix): llfs/llfs.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/llfs_llfs.c$(PreprocessSuffix) llfs/llfs.c

$(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(ObjectSuffix): sd_mmc/hal_mci_sync.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(DependSuffix) -MM sd_mmc/hal_mci_sync.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/sd_mmc/hal_mci_sync.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(PreprocessSuffix): sd_mmc/hal_mci_sync.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sd_mmc_hal_mci_sync.c$(PreprocessSuffix) sd_mmc/hal_mci_sync.c

$(IntermediateDirectory)/rshell_uterm.c$(ObjectSuffix): rshell/uterm.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_uterm.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_uterm.c$(DependSuffix) -MM rshell/uterm.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/uterm.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_uterm.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_uterm.c$(PreprocessSuffix): rshell/uterm.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_uterm.c$(PreprocessSuffix) rshell/uterm.c

$(IntermediateDirectory)/bios_i2c_port.c$(ObjectSuffix): bios/i2c_port.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_i2c_port.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_i2c_port.c$(DependSuffix) -MM bios/i2c_port.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/i2c_port.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_i2c_port.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_i2c_port.c$(PreprocessSuffix): bios/i2c_port.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_i2c_port.c$(PreprocessSuffix) bios/i2c_port.c

$(IntermediateDirectory)/rshell_editline.c$(ObjectSuffix): rshell/editline.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_editline.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_editline.c$(DependSuffix) -MM rshell/editline.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/editline.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_editline.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_editline.c$(PreprocessSuffix): rshell/editline.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_editline.c$(PreprocessSuffix) rshell/editline.c

$(IntermediateDirectory)/rshell_iface_sys.c$(ObjectSuffix): rshell/iface_sys.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_iface_sys.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_iface_sys.c$(DependSuffix) -MM rshell/iface_sys.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/iface_sys.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_iface_sys.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_iface_sys.c$(PreprocessSuffix): rshell/iface_sys.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_iface_sys.c$(PreprocessSuffix) rshell/iface_sys.c

$(IntermediateDirectory)/tusb_msc_disk.c$(ObjectSuffix): tusb/msc_disk.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_msc_disk.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_msc_disk.c$(DependSuffix) -MM tusb/msc_disk.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/msc_disk.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_msc_disk.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_msc_disk.c$(PreprocessSuffix): tusb/msc_disk.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_msc_disk.c$(PreprocessSuffix) tusb/msc_disk.c

$(IntermediateDirectory)/rshell_iface_util.c$(ObjectSuffix): rshell/iface_util.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_iface_util.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_iface_util.c$(DependSuffix) -MM rshell/iface_util.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/iface_util.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_iface_util.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_iface_util.c$(PreprocessSuffix): rshell/iface_util.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_iface_util.c$(PreprocessSuffix) rshell/iface_util.c

$(IntermediateDirectory)/rshell_sys_config.c$(ObjectSuffix): rshell/sys_config.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_sys_config.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_sys_config.c$(DependSuffix) -MM rshell/sys_config.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/sys_config.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_sys_config.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_sys_config.c$(PreprocessSuffix): rshell/sys_config.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_sys_config.c$(PreprocessSuffix) rshell/sys_config.c

$(IntermediateDirectory)/tusb_cdc_device.c$(ObjectSuffix): tusb/cdc_device.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_cdc_device.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_cdc_device.c$(DependSuffix) -MM tusb/cdc_device.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/cdc_device.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_cdc_device.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_cdc_device.c$(PreprocessSuffix): tusb/cdc_device.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_cdc_device.c$(PreprocessSuffix) tusb/cdc_device.c

$(IntermediateDirectory)/bios_bsp.c$(ObjectSuffix): bios/bsp.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_bsp.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_bsp.c$(DependSuffix) -MM bios/bsp.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/bsp.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_bsp.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_bsp.c$(PreprocessSuffix): bios/bsp.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_bsp.c$(PreprocessSuffix) bios/bsp.c

$(IntermediateDirectory)/bios_sys_sercom.c$(ObjectSuffix): bios/sys_sercom.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_sys_sercom.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_sys_sercom.c$(DependSuffix) -MM bios/sys_sercom.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/sys_sercom.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_sys_sercom.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_sys_sercom.c$(PreprocessSuffix): bios/sys_sercom.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_sys_sercom.c$(PreprocessSuffix) bios/sys_sercom.c

$(IntermediateDirectory)/rshell_commandline.c$(ObjectSuffix): rshell/commandline.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_commandline.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_commandline.c$(DependSuffix) -MM rshell/commandline.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/commandline.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_commandline.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_commandline.c$(PreprocessSuffix): rshell/commandline.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_commandline.c$(PreprocessSuffix) rshell/commandline.c

$(IntermediateDirectory)/rshell_rshell.c$(ObjectSuffix): rshell/rshell.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_rshell.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_rshell.c$(DependSuffix) -MM rshell/rshell.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/rshell.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_rshell.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_rshell.c$(PreprocessSuffix): rshell/rshell.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_rshell.c$(PreprocessSuffix) rshell/rshell.c

$(IntermediateDirectory)/bios_lcd.c$(ObjectSuffix): bios/lcd.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_lcd.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_lcd.c$(DependSuffix) -MM bios/lcd.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/lcd.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_lcd.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_lcd.c$(PreprocessSuffix): bios/lcd.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_lcd.c$(PreprocessSuffix) bios/lcd.c

$(IntermediateDirectory)/bios_syscalls.c$(ObjectSuffix): bios/syscalls.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_syscalls.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_syscalls.c$(DependSuffix) -MM bios/syscalls.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/syscalls.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_syscalls.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_syscalls.c$(PreprocessSuffix): bios/syscalls.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_syscalls.c$(PreprocessSuffix) bios/syscalls.c

$(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(ObjectSuffix): sd_mmc/hpl_sdhc.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(DependSuffix) -MM sd_mmc/hpl_sdhc.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/sd_mmc/hpl_sdhc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(PreprocessSuffix): sd_mmc/hpl_sdhc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sd_mmc_hpl_sdhc.c$(PreprocessSuffix) sd_mmc/hpl_sdhc.c

$(IntermediateDirectory)/tusb_usbd.c$(ObjectSuffix): tusb/usbd.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_usbd.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_usbd.c$(DependSuffix) -MM tusb/usbd.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/usbd.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_usbd.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_usbd.c$(PreprocessSuffix): tusb/usbd.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_usbd.c$(PreprocessSuffix) tusb/usbd.c

$(IntermediateDirectory)/src_ucosR.c$(ObjectSuffix): src/ucosR.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ucosR.c$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ucosR.c$(DependSuffix) -MM src/ucosR.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/src/ucosR.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ucosR.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ucosR.c$(PreprocessSuffix): src/ucosR.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ucosR.c$(PreprocessSuffix) src/ucosR.c

$(IntermediateDirectory)/bios_icons.c$(ObjectSuffix): bios/icons.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_icons.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_icons.c$(DependSuffix) -MM bios/icons.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/icons.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_icons.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_icons.c$(PreprocessSuffix): bios/icons.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_icons.c$(PreprocessSuffix) bios/icons.c

$(IntermediateDirectory)/ff_ffsystem.c$(ObjectSuffix): ff/ffsystem.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ff_ffsystem.c$(ObjectSuffix) -MF$(IntermediateDirectory)/ff_ffsystem.c$(DependSuffix) -MM ff/ffsystem.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/ff/ffsystem.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ff_ffsystem.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ff_ffsystem.c$(PreprocessSuffix): ff/ffsystem.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ff_ffsystem.c$(PreprocessSuffix) ff/ffsystem.c

$(IntermediateDirectory)/kernel_queue.c$(ObjectSuffix): kernel/queue.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_queue.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_queue.c$(DependSuffix) -MM kernel/queue.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/queue.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_queue.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_queue.c$(PreprocessSuffix): kernel/queue.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_queue.c$(PreprocessSuffix) kernel/queue.c

$(IntermediateDirectory)/tusb_tusb.c$(ObjectSuffix): tusb/tusb.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_tusb.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_tusb.c$(DependSuffix) -MM tusb/tusb.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/tusb.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_tusb.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_tusb.c$(PreprocessSuffix): tusb/tusb.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_tusb.c$(PreprocessSuffix) tusb/tusb.c

$(IntermediateDirectory)/ff_ffunicode.c$(ObjectSuffix): ff/ffunicode.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ff_ffunicode.c$(ObjectSuffix) -MF$(IntermediateDirectory)/ff_ffunicode.c$(DependSuffix) -MM ff/ffunicode.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/ff/ffunicode.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ff_ffunicode.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ff_ffunicode.c$(PreprocessSuffix): ff/ffunicode.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ff_ffunicode.c$(PreprocessSuffix) ff/ffunicode.c

$(IntermediateDirectory)/ff_fat.c$(ObjectSuffix): ff/fat.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ff_fat.c$(ObjectSuffix) -MF$(IntermediateDirectory)/ff_fat.c$(DependSuffix) -MM ff/fat.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/ff/fat.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ff_fat.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ff_fat.c$(PreprocessSuffix): ff/fat.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ff_fat.c$(PreprocessSuffix) ff/fat.c

$(IntermediateDirectory)/kernel_timers.c$(ObjectSuffix): kernel/timers.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_timers.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_timers.c$(DependSuffix) -MM kernel/timers.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/timers.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_timers.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_timers.c$(PreprocessSuffix): kernel/timers.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_timers.c$(PreprocessSuffix) kernel/timers.c

$(IntermediateDirectory)/src_main.c$(ObjectSuffix): src/main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.c$(DependSuffix) -MM src/main.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/src/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.c$(PreprocessSuffix): src/main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.c$(PreprocessSuffix) src/main.c

$(IntermediateDirectory)/src_iface_sd.c$(ObjectSuffix): src/iface_sd.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_iface_sd.c$(ObjectSuffix) -MF$(IntermediateDirectory)/src_iface_sd.c$(DependSuffix) -MM src/iface_sd.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/src/iface_sd.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_iface_sd.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_iface_sd.c$(PreprocessSuffix): src/iface_sd.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_iface_sd.c$(PreprocessSuffix) src/iface_sd.c

$(IntermediateDirectory)/src_startup.c$(ObjectSuffix): src/startup.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_startup.c$(ObjectSuffix) -MF$(IntermediateDirectory)/src_startup.c$(DependSuffix) -MM src/startup.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/src/startup.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_startup.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_startup.c$(PreprocessSuffix): src/startup.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_startup.c$(PreprocessSuffix) src/startup.c

$(IntermediateDirectory)/rshell_colours.c$(ObjectSuffix): rshell/colours.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_colours.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_colours.c$(DependSuffix) -MM rshell/colours.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/colours.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_colours.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_colours.c$(PreprocessSuffix): rshell/colours.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_colours.c$(PreprocessSuffix) rshell/colours.c

$(IntermediateDirectory)/rshell_red.c$(ObjectSuffix): rshell/red.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rshell_red.c$(ObjectSuffix) -MF$(IntermediateDirectory)/rshell_red.c$(DependSuffix) -MM rshell/red.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/rshell/red.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rshell_red.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rshell_red.c$(PreprocessSuffix): rshell/red.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rshell_red.c$(PreprocessSuffix) rshell/red.c

$(IntermediateDirectory)/ff_diskio.c$(ObjectSuffix): ff/diskio.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ff_diskio.c$(ObjectSuffix) -MF$(IntermediateDirectory)/ff_diskio.c$(DependSuffix) -MM ff/diskio.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/ff/diskio.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ff_diskio.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ff_diskio.c$(PreprocessSuffix): ff/diskio.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ff_diskio.c$(PreprocessSuffix) ff/diskio.c

$(IntermediateDirectory)/sd_mmc_sd_mmc.c$(ObjectSuffix): sd_mmc/sd_mmc.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sd_mmc_sd_mmc.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sd_mmc_sd_mmc.c$(DependSuffix) -MM sd_mmc/sd_mmc.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/sd_mmc/sd_mmc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sd_mmc_sd_mmc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sd_mmc_sd_mmc.c$(PreprocessSuffix): sd_mmc/sd_mmc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sd_mmc_sd_mmc.c$(PreprocessSuffix) sd_mmc/sd_mmc.c

$(IntermediateDirectory)/fonts_font_rimer6x8.c$(ObjectSuffix): fonts/font_rimer6x8.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fonts_font_rimer6x8.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fonts_font_rimer6x8.c$(DependSuffix) -MM fonts/font_rimer6x8.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/fonts/font_rimer6x8.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fonts_font_rimer6x8.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fonts_font_rimer6x8.c$(PreprocessSuffix): fonts/font_rimer6x8.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fonts_font_rimer6x8.c$(PreprocessSuffix) fonts/font_rimer6x8.c

$(IntermediateDirectory)/tusb_dcd_samd.c$(ObjectSuffix): tusb/dcd_samd.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_dcd_samd.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_dcd_samd.c$(DependSuffix) -MM tusb/dcd_samd.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/dcd_samd.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_dcd_samd.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_dcd_samd.c$(PreprocessSuffix): tusb/dcd_samd.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_dcd_samd.c$(PreprocessSuffix) tusb/dcd_samd.c

$(IntermediateDirectory)/tusb_tusb_fifo.c$(ObjectSuffix): tusb/tusb_fifo.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_tusb_fifo.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_tusb_fifo.c$(DependSuffix) -MM tusb/tusb_fifo.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/tusb_fifo.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_tusb_fifo.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_tusb_fifo.c$(PreprocessSuffix): tusb/tusb_fifo.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_tusb_fifo.c$(PreprocessSuffix) tusb/tusb_fifo.c

$(IntermediateDirectory)/kernel_heap_4r.c$(ObjectSuffix): kernel/heap_4r.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/kernel_heap_4r.c$(ObjectSuffix) -MF$(IntermediateDirectory)/kernel_heap_4r.c$(DependSuffix) -MM kernel/heap_4r.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/kernel/heap_4r.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/kernel_heap_4r.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/kernel_heap_4r.c$(PreprocessSuffix): kernel/heap_4r.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/kernel_heap_4r.c$(PreprocessSuffix) kernel/heap_4r.c

$(IntermediateDirectory)/tusb_msc_device.c$(ObjectSuffix): tusb/msc_device.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_msc_device.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_msc_device.c$(DependSuffix) -MM tusb/msc_device.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/msc_device.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_msc_device.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_msc_device.c$(PreprocessSuffix): tusb/msc_device.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_msc_device.c$(PreprocessSuffix) tusb/msc_device.c

$(IntermediateDirectory)/ff_ff.c$(ObjectSuffix): ff/ff.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ff_ff.c$(ObjectSuffix) -MF$(IntermediateDirectory)/ff_ff.c$(DependSuffix) -MM ff/ff.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/ff/ff.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ff_ff.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ff_ff.c$(PreprocessSuffix): ff/ff.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ff_ff.c$(PreprocessSuffix) ff/ff.c

$(IntermediateDirectory)/tusb_tusb_samd51.c$(ObjectSuffix): tusb/tusb_samd51.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tusb_tusb_samd51.c$(ObjectSuffix) -MF$(IntermediateDirectory)/tusb_tusb_samd51.c$(DependSuffix) -MM tusb/tusb_samd51.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/tusb/tusb_samd51.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tusb_tusb_samd51.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tusb_tusb_samd51.c$(PreprocessSuffix): tusb/tusb_samd51.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tusb_tusb_samd51.c$(PreprocessSuffix) tusb/tusb_samd51.c

$(IntermediateDirectory)/fonts_font_serif8x8.c$(ObjectSuffix): fonts/font_serif8x8.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fonts_font_serif8x8.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fonts_font_serif8x8.c$(DependSuffix) -MM fonts/font_serif8x8.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/fonts/font_serif8x8.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fonts_font_serif8x8.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fonts_font_serif8x8.c$(PreprocessSuffix): fonts/font_serif8x8.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fonts_font_serif8x8.c$(PreprocessSuffix) fonts/font_serif8x8.c

$(IntermediateDirectory)/bios_graph.c$(ObjectSuffix): bios/graph.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_graph.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_graph.c$(DependSuffix) -MM bios/graph.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/graph.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_graph.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_graph.c$(PreprocessSuffix): bios/graph.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_graph.c$(PreprocessSuffix) bios/graph.c

$(IntermediateDirectory)/bios_keyboard.c$(ObjectSuffix): bios/keyboard.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bios_keyboard.c$(ObjectSuffix) -MF$(IntermediateDirectory)/bios_keyboard.c$(DependSuffix) -MM bios/keyboard.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/bios/keyboard.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bios_keyboard.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bios_keyboard.c$(PreprocessSuffix): bios/keyboard.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bios_keyboard.c$(PreprocessSuffix) bios/keyboard.c

$(IntermediateDirectory)/fonts_font_rimer8x12.c$(ObjectSuffix): fonts/font_rimer8x12.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fonts_font_rimer8x12.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fonts_font_rimer8x12.c$(DependSuffix) -MM fonts/font_rimer8x12.c
	$(CC) $(SourceSwitch) "/Users/sergey/projloc/ucosR/fonts/font_rimer8x12.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fonts_font_rimer8x12.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fonts_font_rimer8x12.c$(PreprocessSuffix): fonts/font_rimer8x12.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fonts_font_rimer8x12.c$(PreprocessSuffix) fonts/font_rimer8x12.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


