#pragma once

#include <iostream>

class QuadFormu
{

public:
  float discriminant;

  float minusB;
  
  float invers2A;

	QuadFormu(float a, float b, float c);

  static float maxF(float left, float right);

  static float minF(float left, float right);

  static float maxF3(float a, float b, float c);

  static float minF3(float a, float b, float c);

  float minusAnswer();

  float plusAnswer();
  
	~QuadFormu(void);
};

