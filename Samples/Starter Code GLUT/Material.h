#pragma once

#include "glm/glm.hpp"

class Material
{
public:
  glm::vec4 diffCol;
  float specExp;
  float reflParam;
  float indexOfRefraction;
  float fresCoef1;
  float fresCoef2;
  
  Material();

	Material(
		  glm::vec4 diffColIn
		, float specExpIn
		, float reflParamIn
		, float indexOfRefractionIn
		, float fresCoef1In
		, float fresCoef2In
  );

  Material(
        glm::vec3 diffColIn
      , float specExpIn
      , float reflParamIn
      , float transparency
      , float indexOfRefractionIn
      , float fresCoef1In
      , float fresCoef2In
    );

	~Material(void);
};

