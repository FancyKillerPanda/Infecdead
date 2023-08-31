#pragma once

#include "common.hpp"
#include "graphics/vertex.hpp"

#include <vector>

class VertexArray {
public:
    static u32 from_data(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);
};
