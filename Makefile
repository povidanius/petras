OWNER = bin
BINDIR=/usr/local/bin
DOCDIR=/usr/share/doc/petras

petras	: main.o mlp.o parser.o bitmaps.o 
	@echo Compiling neural network software:
	c++ -Wall -o petras main.cpp -ffast-math #-mcpu=pentiumpro
	@echo Removing temporary files:    
	rm -rf *.o *.core
	@echo Process complete. See docs.

clean	:
	@echo Cleaning ...
	rm -rf *.o *.core
config  :
	@echo You must have perl interpreter installed to run configuration script
	./configure.pl	
install : 
	
	@echo [*] Installing neural network software Petras ..
	@echo [*] System core to $(BINDIR):
	$(INSTALL) -c -o $(OWNER) -m 755 petras $(BINDIR)
	@echo [*]Documentation to $(DOCDIR):
	$(INSTALL) -c -o $(OWNER) -m 655 ./doc-lt/*.* $(DOCDIR)



