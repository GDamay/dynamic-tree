EXEC_FILE=dynamic_tree.exe

run: all
	./$(EXEC_FILE)

all: main.o Point.o
	g++ -o $(EXEC_FILE) main.o Point.o

main.o: Models/PointSet/Point.h
	g++ -c main.cpp

Point.o: Models/PointSet/Point.h Models/PointSet/Point.cpp
	g++ -c Models/PointSet/Point.cpp
