DIR =.
CC = gcc

ODIR = obj
IDIR = src


CFLAGS=-Wall -Wextra

ifdef RELEASE
  CFLAGS += -O2
else
  #CFLAGS += -ggdb3 -fsanitize=address
 CFLAGS += -ggdb3
endif

LIBS=-lm -lpthread

_DEPS_C = server_lib.c request_lib.c http_utils.c socket_utils.c thread_utils.c
_DEPS = $(_DEPS_C) $(patsubst %.c,%.h,$(_DEPS_C))
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_OBJ = main.o $(patsubst %.c,%.o,$(_DEPS_C)) 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(IDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

prog: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir -p $(ODIR)
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

