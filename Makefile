RM=rm -rf
GXX=g++
LINKER=g++
EXEC_FILE=dynamic_tree.exe
LINK_OUT=-o 
OBJ_EXT=.o
BIN_ROOT=build/

ifdef DEBUG
	CFLAGS=-Wall -g -c
	BIN_FOLDER=$(BIN_ROOT)debug/
	EXEC_COMMAND=gdb 
else
	CFLAGS=-Wall -c
	BIN_FOLDER=$(BIN_ROOT)release/
	EXEC_COMMAND="./"
endif

ifdef BIN_FOLDER
	MK_FOLDER=mkdir -p $(BIN_FOLDER)
endif

run: all
	$(EXEC_COMMAND)$(BIN_FOLDER)$(EXEC_FILE)

rebuild: clean all

all: pre_build $(BIN_FOLDER)$(EXEC_FILE)

$(BIN_FOLDER)$(EXEC_FILE): $(BIN_FOLDER)main$(OBJ_EXT) $(BIN_FOLDER)Point$(OBJ_EXT) $(BIN_FOLDER)PointSet$(OBJ_EXT) $(BIN_FOLDER)Vertex$(OBJ_EXT) $(BIN_FOLDER)Tree$(OBJ_EXT)
	$(LINKER) $(LINK_OUT)$@ $^

$(BIN_FOLDER)main$(OBJ_EXT): main.cpp Models/PointSet/Point.h Models/PointSet/PointSet.h Models/Tree/Tree.h
	$(GXX) $(LINK_OUT)$@ $(CFLAGS) $<

$(BIN_FOLDER)Point$(OBJ_EXT): Models/PointSet/Point.cpp Models/PointSet/Point.h
	$(GXX) $(LINK_OUT)$@ $(CFLAGS) $<

$(BIN_FOLDER)PointSet$(OBJ_EXT): Models/PointSet/PointSet.cpp Models/PointSet/PointSet.h Models/PointSet/Point.h
	$(GXX) $(LINK_OUT)$@ $(CFLAGS) $<

$(BIN_FOLDER)Vertex$(OBJ_EXT): Models/Tree/Vertex.cpp Models/Tree/Vertex.h Models/PointSet/Point.h Models/PointSet/PointSet.h
	$(GXX) $(LINK_OUT)$@ $(CFLAGS) $<

$(BIN_FOLDER)Tree$(OBJ_EXT): Models/Tree/Tree.cpp Models/Tree/Tree.h Models/Tree/Vertex.h Models/PointSet/PointSet.h Models/PointSet/Point.h
	$(GXX) $(LINK_OUT)$@ $(CFLAGS) $<

clean:
	$(RM) $(BIN_ROOT)

pre_build:
		$(MK_FOLDER)
