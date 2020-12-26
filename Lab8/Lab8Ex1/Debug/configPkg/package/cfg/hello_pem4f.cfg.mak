# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f /Users/phatle/CMPE146LabProjects/hello_MSP_EXP432P401R_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f /Users/phatle/CMPE146LabProjects/hello_MSP_EXP432P401R_tirtos_ccs/src/makefile.libs clean

