#pragma once

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"
#include "Light.h"

class Ray
{
public:
  glm::vec4 start;
  glm::vec4 dir;

	Ray(glm::vec4 start, glm::vec4 dir);
 

	~Ray(void);
};

