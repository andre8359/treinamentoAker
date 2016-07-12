DIR =.
CC=gcc
CFLAGS=-g 

ODIR=.
LDIR =.

LIBS=-lm

_DEPS = recupera_pag_web_lib.h

OBJ = main.o recupera_pag_web_lib.o 


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prog: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
