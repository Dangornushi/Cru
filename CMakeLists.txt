set(INCLUDE_DIRS  src/Lexer/ src/Node/ src/generator)
cmake_minimum_required(VERSION 3.23.1)
project(cru CXX)
include_directories(${INCLUDE_DIRS})
add_executable(cru 
	src/main.cpp
	src/Lexer/Lexer.cpp
	src/Node/NodeArgment.cpp
	src/Node/NodeSent.cpp
	src/Node/NodeFunctionDefinition.cpp
	src/Node/Node.cpp
	src/generator/generate.cpp
)
set_property(TARGET cru PROPERTY CXX_STANDARD 20)                       
