#include "GLVertexArray.hpp"

GLVertexArray::GLVertexArray()
{
    glGenVertexArrays(1, &id);
    glGenBuffers(1, &position_id);
}

void GLVertexArray::fill_position_buffer(std::vector<glm::vec3> vertices)
{

}
