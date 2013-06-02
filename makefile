SRCDIR = src/
BUILDDIR = build/
OBJS = $(BUILDDIR)domain.o $(BUILDDIR)mesh.o $(BUILDDIR)main.o $(BUILDDIR)vector.o $(BUILDDIR)edge_coloring.o $(BUILDDIR)kernels.o
CC = nvcc
CFLAGS = -c -o $@
LFLAGS =  

main: mkbuilddir $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BUILDDIR)main

mkbuilddir:
	mkdir -p build

$(BUILDDIR)domain.o: $(SRCDIR)domain.h $(SRCDIR)domain.cpp $(SRCDIR)mesh.h
	$(CC) $(CFLAGS) $(SRCDIR)domain.cpp

$(BUILDDIR)mesh.o: $(SRCDIR)mesh.h $(SRCDIR)mesh.cpp $(SRCDIR)vector.h $(SRCDIR)edge_coloring.h
	$(CC) $(CFLAGS) $(SRCDIR)mesh.cpp

$(BUILDDIR)edge_coloring.o: $(SRCDIR)edge_coloring.h $(SRCDIR)edge_coloring.cpp $(SRCDIR)mesh.h
	$(CC) $(CFLAGS) $(SRCDIR)edge_coloring.cpp

$(BUILDDIR)vector.o: $(SRCDIR)vector.h $(SRCDIR)vector.cpp
	$(CC) $(CFLAGS) $(SRCDIR)vector.cpp

$(BUILDDIR)main.o: $(SRCDIR)main.cpp $(SRCDIR)domain.h
	$(CC) $(CFLAGS) $(SRCDIR)main.cpp

$(BUILDDIR)kernels.o: $(SRCDIR)kernels.h $(SRCDIR)kernels.cu $(SRCDIR)mesh.h $(SRCDIR)domain.h
	$(CC) $(CFLAGS) $(SRCDIR)kernels.cu

clean:
	\rm $(BUILDDIR)*.o

run: main
	mkdir -p output
	$(BUILDDIR)main