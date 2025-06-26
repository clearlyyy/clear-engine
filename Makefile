# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Iimgui

# Library Paths
LDFLAGS = -Llib -lglfw -lGL -lX11 -lXrandr -lXi -lXcursor -lXinerama -ldl -lpthread -lm -lassimp -Wl,-rpath='$$ORIGIN'

# Sources
SRC = main.cpp src/glad.c Shader.cpp Texture.cpp include/stb_image.h Camera.cpp Light.cpp PointLight.cpp Mesh.cpp Primitives.cpp Globals.cpp Material.cpp Model.cpp Renderer.cpp DirectionalLight.cpp SpotLight.cpp GUI.cpp imgui/imgui.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp Skybox.cpp GBuffer.cpp SSAO.cpp ShadowMap.cpp
OUT = engine

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) && ./$(OUT)

clean:
	rm -f $(OUT)
