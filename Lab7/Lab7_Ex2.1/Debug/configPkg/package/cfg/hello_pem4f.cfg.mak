# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f /Users/phatle/CMPE146LabProjects/Lab7_Ex2/src/makefile.libs

clean::
	$(MAKE) -f /Users/phatle/CMPE146LabProjects/Lab7_Ex2/src/makefile.libs clean

