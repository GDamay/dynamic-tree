# Fully Dynamic Decision Tree

This project implements the algorithm proposed in *Add link to article (article not published so far)*

This algorithm presents a structure for dynamic decision trees that minimise the amortized cost of updating the tree.

## Repository overview

```
├── README.md
├── .gitignore
├── CMakeLists.txt -- Configures cmake
├── .gitignore
├── main.cpp       -- Main functions of the program
├── param_parser.h -- Small library to parse parameters provided at execution
└── Models
    ├── PointSet
	│   ├── CMakeLists.txt
	│   ├── Point.h        -- Class representing point of data
	│   ├── Point.cpp
	│   ├── PointSet.h     -- Class representing set of points
	│   └── PointSet.cpp
    └── Tree
	    ├── CMakeLists.txt
	    ├── Tree.h         -- Class representing a dynamic decision tree
	    ├── Tree.cpp
	    ├── Vertex.h       -- Class representing a vertex of a dynamic decision tree
	    └── Vertex.cpp
```

## Running instructions

You need *cmake* to build and run this program. If you are using Windows, you need a compiler and a generator installed (Visual Studio provides both).

With cmake installed and using a terminal in the project folder, run the following :

```
cmake -S . -B Build
cmake --build Build
```

An executable will be created. On Linux, it will be *Build/dynamic_tree*, on Windows using Visual Studio, it will be *Build/Debug/dynamic_tree.exe* . 

You can run an example using `[path_to_executable] --param_file Example/parameters.csv`

Run `[path_to_executable] --help` to get further instructions.

## Documentation generation

This project is documented and configured so as to use [Doxygen](https://www.doxygen.nl/index.html) for generated html or pdf documentation. To generated the documentation, with doxygen installed, open the file *Doxyfile* in the Doxygen GUI or, using a terminal in the root project folder, run the following :

```
doxygen Doxyfile
```

You can then access the html documentation by opening *Documentation/html/index.html* in a web browser. For further instructions on Doxygen use, please refer to the [Doxygen website](https://www.doxygen.nl/index.html).