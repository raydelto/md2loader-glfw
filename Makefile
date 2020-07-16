UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS = -lGL -lGLEW -lglfw -ldl
	INCLUDES=-I ./include
endif
ifeq ($(UNAME_S),Darwin)
FRAMEWORKS=-framework OpenGL

LIBS= -L/System/Library/Frameworks \
	  -lglfw
INCLUDES=-I./include
SRC=src/glad.c
OBJ=bin/glad.o
endif

SRC += src/ShaderProgram.cpp \
	   src/Texture2D.cpp \
	   src/Md2.cpp \
	   src/OpenGLInit.cpp \
	   src/main.cpp

OBJ += bin/ShaderProgram.o \
	   bin/Texture2D.o \
	   bin/Md2.o \
	   bin/OpenGLInit.o \
	   bin/main.o

WARNINGS=-Wall

FLAGS=-std=c++14

all:
	g++ -c -g $(SRC) $(INCLUDES) $(WARNINGS) $(FLAGS)
	ls bin>/dev/null||mkdir bin
	mv *.o ./bin
	g++ -g $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o bin/main $(WARNINGS) $(FLAGS)
