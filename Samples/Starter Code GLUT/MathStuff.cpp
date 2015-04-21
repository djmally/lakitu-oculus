#include "MathStuff.h"


MathStuff::MathStuff(void)
{
}
 
/*
glm::vec3 reflectionDir(glm::vec3 incident, glm::vec3 normal) {
  incident - (normal * (2 * (glm::dot(normal, incident))));
} */

glm::vec3 refractionDir(glm::vec3 incident, glm::vec3 normal, float refrac) {
  const float c = glm::dot(normal, (-1.0f * incident));
  const float refrac2 = c < 0 ? refrac : 1 / refrac;
  const float v = 1 + refrac2 * refrac2 * (c * c - 1);
	  
  //refractDir i n r = if (v < 0) 
  //  then (0.0, 0.0, 0.0) 
  //  else norm $ (i *> r_c) <+> (n *> (r_c*(abs c) - sqrt v))
	return v < 0 
	    ? glm::vec3(0.0, 0.0, 0.0) 
		  : glm::normalize(incident * refrac2 + normal * (refrac2 * abs(c) - sqrt(v)));
}


MathStuff::~MathStuff(void)
{
}
