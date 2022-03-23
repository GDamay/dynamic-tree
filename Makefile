EXEC_FILE=dynamic_tree.exe

run: all
	./$(EXEC_FILE)

all: main.o
	g++ -o $(EXEC_FILE) main.o

main.o:
	g++ -c main.cpp
