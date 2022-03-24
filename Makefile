GXX=g++
EXEC_FILE=dynamic_tree.exe

run: all
	./$(EXEC_FILE)

all: main.o Point.o
	$(GXX) -o $(EXEC_FILE) main.o Point.o

main.o: Models/PointSet/Point.h
	$(GXX) -c main.cpp

Point.o: Models/PointSet/Point.h Models/PointSet/Point.cpp
	$(GXX) -c Models/PointSet/Point.cpp
