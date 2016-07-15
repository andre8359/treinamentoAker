DIR =.
CC=gcc

ODIR=.
LDIR =.

CFLAGS=-Wall -Wextra

ifdef RELEASE
	CFLAGS += -O2
else
	CFLAGS += -ggdb3 -fsanitize=address
endif

LIBS=-lm

_DEPS = recupera_pag_web_lib.h recupera_pag_web_lib.c

OBJ = main.o recupera_pag_web_lib.o 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

prog: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
