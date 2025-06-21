#pragma once

#include "Mesh.h"

class Cube : public Mesh {
    public:
        Cube();
        Cube(Material* mat);
    private:
        static std::vector<Vertex> generateVertices();
        static std::vector<unsigned int> generateIndices();
    
    
};