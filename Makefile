
#Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Library Paths
LDFLAGS = -Llib -lglfw3 -lgdi32 -lopengl32 -lm

SRC = main.cpp src/glad.c
OUT = engine.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) && ./$(OUT)

clean:
	del /Q $(OUT)