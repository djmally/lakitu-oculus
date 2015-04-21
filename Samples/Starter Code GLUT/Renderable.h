#pragma once

#include "Ray.h"

class Renderable 
{
public:
  virtual void render() =0; 

  virtual float testIntersection(Ray r) =0;
};

