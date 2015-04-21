#pragma once

#include "glm/glm.hpp"
#include "Ray.h"
#include "Scene.h"

class Raytracer
{
public:
	Raytracer(void);

  glm::vec3 color(
      Ray r
	  , int recDepth
	  , glm::vec3 backgroundColor
    , Scene scene
    , std::vector<Light> lights
	  );
  
	~Raytracer(void);
};

