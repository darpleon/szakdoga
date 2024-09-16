#pragma once
#include "glad.h"
#include <glm/ext/vector_float3.hpp>
#include <vector>

class GLVertexArray
{
public:
    GLVertexArray();

    void fill_position_buffer(std::vector<glm::vec3> vertices);

private:
    GLuint id;
    GLuint position_id;
};
