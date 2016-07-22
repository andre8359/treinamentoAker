DIR =.
CC = gcc

ODIR = obj
IDIR = src

CFLAGS=-Wall -Wextra

ifdef RELEASE
	CFLAGS += -O2
else
#	CFLAGS += -ggdb3 -fsanitize=address
  CFLAGS += -ggdb3 
endif

LIBS=-lm

_DEPS = server_lib.c request_lib.c
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o server_lib.o request_lib.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
$(ODIR)/%.o: $(IDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

prog: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
