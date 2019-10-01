CC = gcc
CFLAGS = -D_XOPEN_SOURCE_EXTENDED -lncurses 
FILES = e-cel.c

all: e-cel excelparse


e-cel: $(OBJECTS)
	$(CC) $(FILES) -o e-cel $(CFLAGS)
	flex excelparse.l
	bison excelparse.y
	$(CC) excelparse.tab.c lex.yy.c -lm -o excelparse

clean:
	rm -f *.o e-cel
