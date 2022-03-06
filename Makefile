UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS = -lGL -lGLEW -lglfw -ldl
	INCLUDES=-I ./include
endif
ifeq ($(UNAME_S),Darwin)
FRAMEWORKS=-framework OpenGL -framework Cocoa -framework IOKit

LIBS= -L/opt/glfw/lib-arm64 \
	  -L/System/Library/Frameworks \
	  -lglfw3
INCLUDES=-I./include -I./TPL/imgui -I./TPL/imgui/backends
SRC=src/glad.c
OBJ=bin/glad.o
endif

SRC += src/ShaderProgram.cpp \
	   src/Texture2D.cpp \
	   src/Md2.cpp \
	   src/OpenGLInit.cpp \
	   TPL/imgui/imgui.cpp \
	   TPL/imgui/imgui_demo.cpp \
	   TPL/imgui/imgui_draw.cpp \
	   TPL/imgui/imgui_tables.cpp \
	   TPL/imgui/imgui_widgets.cpp \
	   TPL/imgui/backends/imgui_impl_glfw.cpp \
	   TPL/imgui/backends/imgui_impl_opengl3.cpp \
	   src/main.cpp

OBJ += bin/ShaderProgram.o \
	   bin/Texture2D.o \
	   bin/Md2.o \
	   bin/OpenGLInit.o \
	   bin/imgui.o \
	   bin/imgui_demo.o \
	   bin/imgui_draw.o \
	   bin/imgui_tables.o \
	   bin/imgui_widgets.o \
	   bin/imgui_impl_glfw.o \
	   bin/imgui_impl_opengl3.o \
	   bin/main.o

WARNINGS=-Wall

FLAGS=-std=c++14

all:
	g++ -c -g $(SRC) $(INCLUDES) $(WARNINGS) $(FLAGS)
	ls bin>/dev/null||mkdir bin
	mv *.o ./bin
	g++ -g $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o bin/maingpp $(WARNINGS) $(FLAGS)

release:
	g++ -c $(SRC) $(INCLUDES) $(WARNINGS) $(FLAGS)
	ls bin>/dev/null||mkdir bin
	mv *.o ./bin
	g++ $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o bin/main $(WARNINGS) $(FLAGS)

clang:
	clang++ -c $(SRC) $(INCLUDES) $(WARNINGS) $(FLAGS) -Ofast
	ls bin>/dev/null||mkdir bin
	mv *.o ./bin
	clang++ $(OBJ) $(FRAMEWORKS) $(LIBS) $(INCLUDES) -o bin/main $(WARNINGS) $(FLAGS) -Ofast

