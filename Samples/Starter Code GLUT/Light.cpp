#include "Light.h"


Light::Light(glm::vec4 posIn, glm::vec3 colIn)
{
  pos = posIn;
  col = colIn;
}


Light::~Light(void)
{
}
