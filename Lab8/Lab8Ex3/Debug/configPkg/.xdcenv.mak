#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages
override XDCROOT = /Users/phatle/ti/xdctools_3_60_02_34_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/Users/phatle/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;/Users/phatle/ti/xdctools_3_60_02_34_core/packages;..
HOSTOS = MacOS
endif
