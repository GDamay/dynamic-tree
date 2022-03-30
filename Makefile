RM=rm -f
GXX=g++
LINKER=g++
EXEC_FILE=dynamic_tree.exe
EXEC_COMMAND="./"
LINK_OUT="-o "
OBJ_EXT=.o

run: all
	$(EXEC_COMMAND)$(EXEC_FILE)

all: $(EXEC_FILE)

$(EXEC_FILE): main$(OBJ_EXT) Point$(OBJ_EXT) PointSet$(OBJ_EXT) Vertex$(OBJ_EXT) Tree$(OBJ_EXT)
	$(LINKER) $(LINK_OUT)$(EXEC_FILE) main$(OBJ_EXT) Point$(OBJ_EXT) PointSet$(OBJ_EXT) Vertex$(OBJ_EXT) Tree$(OBJ_EXT)

main$(OBJ_EXT): main.cpp Models/PointSet/Point.h Models/PointSet/PointSet.h Models/Tree/Tree.h
	$(GXX) -c main.cpp

Point$(OBJ_EXT): Models/PointSet/Point.h Models/PointSet/Point.cpp
	$(GXX) -c Models/PointSet/Point.cpp

PointSet$(OBJ_EXT): Models/PointSet/Point.h Models/PointSet/PointSet.h Models/PointSet/PointSet.cpp
	$(GXX) -c Models/PointSet/PointSet.cpp

Vertex$(OBJ_EXT): Models/Tree/Vertex.cpp Models/Tree/Vertex.h Models/PointSet/Point.h Models/PointSet/PointSet.h
	$(GXX) -c Models/Tree/Vertex.cpp

Tree$(OBJ_EXT): Models/Tree/Tree.cpp Models/Tree/Tree.h Models/Tree/Vertex.h Models/PointSet/PointSet.h Models/PointSet/Point.h
	$(GXX) -c Models/Tree/Tree.cpp

clean:
	$(RM) *$(OBJ_EXT) *.exe
