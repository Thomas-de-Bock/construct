CC = g++
CFLAGS = -std=c++11 -Wall --pedantic-errors -g
SDIR = src
BDIR = bin
_OBJS = construct_debug.o construct_flags.o deconstruct.o reconstruct.o construct.o
OBJS =  $(patsubst %,$(BDIR)/%,$(_OBJS))
PROG = construct.exe

.PHONY: all clean

all: $(OBJS) $(BDIR)/$(PROG)

$(BDIR)/$(PROG): $(OBJS)
	mkdir -p $(BDIR)
	$(CC) $(OBJS) -o $(BDIR)/$(PROG) $(CFLAGS)

$(BDIR)/construct.o: $(SDIR)/construct.cpp $(SDIR)/deconstruct.h $(SDIR)/reconstruct.h $(SDIR)/construct_flags.h $(SDIR)/construct_types.h
	mkdir -p $(BDIR)
	$(CC) -c $(SDIR)/construct.cpp -o $(BDIR)/construct.o $(CFLAGS)

$(BDIR)/construct_debug.o: $(SDIR)/construct_debug.cpp $(SDIR)/construct_debug.h $(SDIR)/construct_types.h $(SDIR)/reconstruct.h
	mkdir -p $(BDIR)
	$(CC) -c $(SDIR)/construct_debug.cpp -o $(BDIR)/construct_debug.o $(CFLAGS)

$(BDIR)/construct_flags.o: $(SDIR)/construct_flags.cpp $(SDIR)/construct_flags.h $(SDIR)/construct_types.h
	mkdir -p $(BDIR)
	$(CC) -c $(SDIR)/construct_flags.cpp -o $(BDIR)/construct_flags.o $(CFLAGS)

$(BDIR)/deconstruct.o: $(SDIR)/deconstruct.cpp $(SDIR)/deconstruct.h $(SDIR)/construct_types.h
	mkdir -p $(BDIR)
	$(CC) -c $(SDIR)/deconstruct.cpp -o $(BDIR)/deconstruct.o $(CFLAGS)

$(BDIR)/reconstruct.o: $(SDIR)/reconstruct.cpp $(SDIR)/reconstruct.h $(SDIR)/construct_types.h
	mkdir -p $(BDIR)
	$(CC) -c $(SDIR)/reconstruct.cpp -o $(BDIR)/reconstruct.o $(CFLAGS)

clean:
	rm -rf $(BDIR)
