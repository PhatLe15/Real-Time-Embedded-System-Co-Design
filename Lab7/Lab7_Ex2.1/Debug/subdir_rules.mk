################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1" --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1/Debug" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/posix/ccs" --include_path="/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-420987681:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-420987681-inproc

build-420987681-inproc: ../hello.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/Users/phatle/ti/xdctools_3_60_02_34_core/xs" --xdcpath="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.msp432:MSP432P401R -r release -c "/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1\" --include_path=\"/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1/Debug\" --include_path=\"/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source\" --include_path=\"/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include\" --include_path=\"/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/posix/ccs\" --include_path=\"/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include\" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-420987681 ../hello.cfg
configPkg/compiler.opt: build-420987681
configPkg/: build-420987681

build-1161551919: ../hello.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Applications/ti/ccs1010/ccs/utils/sysconfig_1.5.0/sysconfig_cli.sh" -s "/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-1161551919 ../hello.syscfg
syscfg/ti_drivers_config.h: build-1161551919
syscfg/syscfg_c.rov.xs: build-1161551919
syscfg/: build-1161551919

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1" --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1/Debug" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/posix/ccs" --include_path="/Applications/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="/Users/phatle/CMPE146LabProjects/Lab7_Ex2.1/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


