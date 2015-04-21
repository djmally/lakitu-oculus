#pragma once

#include "glm\glm.hpp"
#include "QuadFormu.h"

class MathStuff
{
public:
	MathStuff(void);

  //static glm::vec3 reflectionDir(glm::vec3 incident, glm::vec3 surfaceNormal);

  static glm::vec3 refractionDir(glm::vec3 incident, glm::vec3 surfaceNormal, float refractionCoefficient);

	~MathStuff(void);
};

