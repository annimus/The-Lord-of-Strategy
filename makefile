CC = gcc

SDIR = src
IDIR = include
ODIR = obj

_OBJ1 = basis.o logic.o interface.o main.o
_OBJ2 = basis.o logic.o interface.o cunit_test.o cunit_main.o
OBJ1 = $(patsubst %,$(ODIR)/%,$(_OBJ1))
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

CFLAGS = -I$(IDIR) -Wall -Wextra -g
LIBS = -lncurses -lm -pthread -lpanel -lcunit

SPLINT = splint
SPLINTFLAGS = -weak -type +sysdirerrors -retvalother\
               +line-len 80 -warnposix +infloops -fixedformalarray \
	      -annotationerror -varuse
SPLINT_SOURCE = src/basis.c src/logic.c src/interface.c src/main.c

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p saves
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: splinttest
splinttest:
	$(SPLINT) $(SPLINTFLAGS) -I $(IDIR) $(SPLINT_SOURCE)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o core game test
