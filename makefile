CC=gcc
CFLAGS=-Wall

all: tdta

tdta: tdta.o tdtaibbs.o
	$(CC) -o tdta tdta.o tdtaibbs.o libODoors.a

tdta.o: tdta.c tdta.h tdta2.h tdta3.h
	$(CC) $(CFLAGS) -c tdta.c

tdtaibbs.o: tdtaibbs.c tdtaibbs.h
	$(CC) $(CFLAGS) -c tdtaibbs.c

clean:
	rm *.o tdta


