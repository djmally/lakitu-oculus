#pragma once

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"
#include "Ray.h"
#include "stubs.h"
#include "MathStuff.h"
#include "Material.h"

class Intersection
{
public:
  float distance;
  glm::vec4 tan1;
  glm::vec4 tan2;
  int matInd;

  Intersection(ShapeGen::ShapeType st, Mesh mesh, Ray r, glm::mat4 const& transformationInverse, glm::mat4 const& transformationMatrix, int matInd);
  Intersection(float testDistance);

  glm::vec4 closestNormal(glm::vec4 rayDir);

  bool hitSomething();

	~Intersection(void);
};

