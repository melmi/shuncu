SRCDIR = src/
BUILDDIR = build/
OBJS = $(BUILDDIR)domain.o $(BUILDDIR)mesh.o $(BUILDDIR)main.o $(BUILDDIR)vector.o $(BUILDDIR)edge_coloring.o
CC = g++
CFLAGS = -Wall -c -o $@
LFLAGS = -Wall 

main: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BUILDDIR)main

$(BUILDDIR)domain.o: $(SRCDIR)domain.h $(SRCDIR)domain.cpp $(SRCDIR)mesh.h
	$(CC) $(CFLAGS) $(SRCDIR)domain.cpp

$(BUILDDIR)mesh.o: $(SRCDIR)mesh.h $(SRCDIR)mesh.cpp $(SRCDIR)vector.h
	$(CC) $(CFLAGS) $(SRCDIR)mesh.cpp

$(BUILDDIR)edge_coloring.o: $(SRCDIR)edge_coloring.h $(SRCDIR)edge_coloring.cpp $(SRCDIR)mesh.h
	$(CC) $(CFLAGS) $(SRCDIR)edge_coloring.cpp

$(BUILDDIR)vector.o: $(SRCDIR)vector.h $(SRCDIR)vector.cpp
	$(CC) $(CFLAGS) $(SRCDIR)vector.cpp

$(BUILDDIR)main.o: $(SRCDIR)main.cpp $(SRCDIR)domain.h $(SRCDIR)edge_coloring.h
	$(CC) $(CFLAGS) $(SRCDIR)main.cpp

clean:
	\rm $(BUILDDIR)*.o

run: main
	mkdir -p output
	$(BUILDDIR)main