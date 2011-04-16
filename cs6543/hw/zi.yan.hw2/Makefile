CC=gcc
CFLAG=-lpthread


all: bank hotel


bank: bank.c
	$(CC) -o bank bank.c $(CFLAG)

hotel: hotel.c
	$(CC) -o hotel hotel.c $(CFLAG)


clean:
	rm -rf bank hotel
