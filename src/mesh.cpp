#include "mesh.hpp"

const GLuint Mesh::_NB_ELEMENT_PER_VERTICES = VboType::VERTICES
                                     + VboType::COLORS
                                     + VboType::UVS
                                     + VboType::NORMALS;