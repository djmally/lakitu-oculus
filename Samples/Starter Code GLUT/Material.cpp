#include "Material.h"

Material::Material(
		  glm::vec4 diffColIn
		, float specExpIn
		, float reflParamIn
		, float indexOfRefractionIn
		, float fresCoef1In
		, float fresCoef2In
  ) {
  diffCol = diffColIn;
  specExp = specExpIn;
  reflParam = reflParamIn;
  indexOfRefraction = indexOfRefraction;
  fresCoef1 = fresCoef1In;
  fresCoef2 = fresCoef2In;
}

Material::Material(
		  glm::vec3 diffColIn
		, float specExpIn
		, float reflParamIn
    , float transparency
		, float indexOfRefractionIn
		, float fresCoef1In
		, float fresCoef2In
  ) {
  diffCol = glm::vec4(diffColIn, transparency);
  specExp = specExpIn;
  reflParam = reflParamIn;
  indexOfRefraction = indexOfRefraction;
  fresCoef1 = fresCoef1In;
  fresCoef2 = fresCoef2In;
}


Material::~Material(void)
{
}
