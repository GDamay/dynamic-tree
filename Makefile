RM=rm -f
GXX=g++
EXEC_FILE=dynamic_tree.exe

run: all
	./$(EXEC_FILE)

all: $(EXEC_FILE)

$(EXEC_FILE): main.o Point.o PointSet.o
	$(GXX) -o $(EXEC_FILE) main.o Point.o PointSet.o

main.o: main.cpp Models/PointSet/Point.h Models/PointSet/PointSet.h
	$(GXX) -c main.cpp

Point.o: Models/PointSet/Point.h Models/PointSet/Point.cpp
	$(GXX) -c Models/PointSet/Point.cpp

PointSet.o: Models/PointSet/Point.h Models/PointSet/PointSet.h Models/PointSet/PointSet.cpp
	$(GXX) -c Models/PointSet/PointSet.cpp
	
clean:
	$(RM) *.o *.exe
