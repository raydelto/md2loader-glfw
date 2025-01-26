UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS = -lGL -lGLEW -lglfw -ldl
	INCLUDES=-I ./include
endif
ifeq ($(UNAME_S),Darwin)
SRC=src/glad.c
OBJ=bin/glad.o

LIBS= -L/opt/homebrew/opt/glfw/lib \
	  -lglfw

INCLUDES=-I./include \
		-I/opt/homebrew/opt/glfw/include \
		-I/opt/homebrew/include \
		-I./include \
		-I/usr/local/include

else
# Windows (MinGW64)
LIBS = -lglew32 -lglfw3 -lopengl32 -lgdi32 \
	   -LC:\msys64\mingw64\lib
INCLUDES=-IC:\msys64\mingw64\include -I./include

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

WARNINGS=-w

FLAGS=-std=c++17

all:
	g++ -c $(SRC) $(INCLUDES) $(WARNINGS) $(FLAGS)
	g++ $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o bin/main $(WARNINGS) $(FLAGS)
