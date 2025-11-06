CC=gcc
CFLAGS=-Wall -O2 
LIBS=-lcurl -lssl -lcrypto -larchive -Iinclude 

SRC=src/main.c src/download.c src/verify.c src/extract.c src/db.c src/repo.c src/cJSON.c
OBJ=$(SRC:.c=.o)

fpm: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f $(OBJ) mypm
