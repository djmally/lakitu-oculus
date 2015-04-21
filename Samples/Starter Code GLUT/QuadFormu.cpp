#include "QuadFormu.h"

QuadFormu::QuadFormu(float a, float b, float c)
{
  minusB = (-b);
  invers2A = 1/(2 * a);
  discriminant = b * b - 4 * a * c;
}

float QuadFormu::minF(float left, float right) {
    return left < right ? left : right;
}

float QuadFormu::maxF(float left, float right) {
    return left > right ? left : right;
}

float QuadFormu::minF3(float a, float b, float c) {
    return minF(a, minF(b, c));
}

float QuadFormu::maxF3(float a, float b, float c) {
    return maxF(a, maxF(b, c));
}

float QuadFormu::minusAnswer() {
  if (discriminant > 0) {
    return (minusB - sqrt(discriminant)) * invers2A;
  } else {
    std::cout << "negative discriminant detected.  Returning -1";
    return -1;
  }
}

float QuadFormu::plusAnswer() {
  if (discriminant > 0) {
    return (minusB + sqrt(discriminant)) * invers2A;
  } else {
    std::cout << "negative discriminant detected.  Returning -1";
    return -1;
  }
}


QuadFormu::~QuadFormu(void)
{
}
