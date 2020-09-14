PREFIX = /home/dan/temp
dbg = -O -g
CC=gcc
CFLAGS=-Wall
VERSION_MAJOR:=0
VERSION_MINOR:=12
VERSION_TYPE=a

all: tdta

tdta: tdta.o tdtaibbs.o
	$(CC) $(dbg) -o tdta tdta.o tdtaibbs.o libODoors.a

tdta.o: tdta.c tdta.h tdta2.h tdta3.h
	$(CC) $(CFLAGS) $(dbg) -c tdta.c

tdtaibbs.o: tdtaibbs.c tdtaibbs.h
	$(CC) $(CFLAGS) $(dbg) -c tdtaibbs.c

.PHONY: install clean help check-syntax distribution
install:
	mkdir ansi
	mkdir logs
	cp *.log logs
	rm *.log
	cp *.ans ansi
	cp *.ANS ansi
	rm *.ans
	rm *.ANS

uninstall:
	rm -f ansi
	rm -f logs

clean:
	-rm *.o tdta

distribution:
	mkdir -p dist
	zip dist/tdta$(VERSION_MAJOR)$(VERSION_MINOR)$(VERSION_TYPE).zip ansi/* configure file_id.diz install.txt libODoors.a LICENSE Makefile.in maps.asc OpenDoor.h README.md tdta tdta.c tdta.cfg tdta.h tdta2.h tdta3.h tdtaarmo.dat tdtaenem.dat tdtaibbs.c tdtaibbs.h tdtaitem.dat tdtamstr.dat tdtaweap.dat

help:
	@echo Make rules
	@echo all...........Builds Linux binaries
	@echo tdta..........Builds Linux binaries
	@echo install.......Creates sub-directories and moves files
	@echo uninstall.....Removes sub-directories and files
	@echo clean.........Removes build binaries
	@echo check-syntax..Checks the syntax of the source files
	@echo distribution..Create distribution archive
	@echo help..........You are reading it

check-syntax:
	-$(CC) tdta.c tdtaibbs.c $(CFLAGS) -o null -Wall libODoors.a
	-$(RM) null
