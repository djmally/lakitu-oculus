#pragma once

#include "glm/glm.hpp"

class Light
{
public:
  glm::vec4 pos;
  glm::vec3 col;

	Light(glm::vec4 posIn, glm::vec3 colIn); 

	~Light(void);
};

