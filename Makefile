UNAME_S := $(shell uname -s)

OBJ =  bin/TgaLoader.o \
	   bin/ShaderProgram.o \
	   bin/Texture2D.o \
	   bin/Md2.o \
	   bin/main.o

all: bin/main

ifeq ($(UNAME_S),Linux)
	LIBS = -lGL -lGLEW -ldl -lSDL3
	INCLUDES=-I ./include
endif
ifeq ($(UNAME_S),Darwin)
OBJ += bin/glad.o

LIBS= -L/opt/homebrew/opt/SDL3/lib \
	  -lSDL3

INCLUDES=-I./include \
		-I/opt/homebrew/opt/SDL3/include \
		-I/opt/homebrew/include \
		-I./include \
		-I/usr/local/include

else
# Windows (MinGW64)
LIBS = -lglew32 -lopengl32 -lgdi32 -lSDL3\
	   -LC:\msys64\mingw64\lib -LC:\sdl3\lib
INCLUDES=-IC:\msys64\mingw64\include -IC:\sdl3\include -I./include 

clean: 
	del bin\*.o
	del bin\main.exe

endif

WARNINGS=-Wall

FLAGS=-std=c++17

bin/%.o: src/%.cpp | bin
	g++ -c $< -o $@ $(INCLUDES) $(WARNINGS) $(FLAGS) $(SDL_INCLUDES)

bin/main: $(OBJ)
	g++ $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o $@ $(WARNINGS) $(FLAGS)

bin:
	mkdir bin
