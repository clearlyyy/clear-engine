
#Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Library Paths
LDFLAGS = -Llib -lglfw3 -lgdi32 -lopengl32 -lm -lassimp

SRC = main.cpp src/glad.c Shader.cpp Texture.cpp include/stb_image.h Camera.cpp Light.cpp PointLight.cpp Mesh.cpp Primitives.cpp Globals.cpp Material.cpp Model.cpp Renderer.cpp DirectionalLight.cpp SpotLight.cpp
OUT = engine.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) && ./$(OUT)

clean:
	del /Q $(OUT)