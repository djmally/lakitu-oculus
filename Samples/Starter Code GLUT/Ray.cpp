#include "Ray.h"


Ray::Ray(glm::vec4 startIn, glm::vec4 dirIn)
{
    start = startIn;
    dir = dirIn;
} 

Ray::~Ray(void)
{
}
