#pragma once

class AltFilter;

#include "AltFilterTask.hpp"

//#include <WProgram.h>
#include <Eigen.h>
#include <Eigen/Dense>
//#include "Flight_Configuration.h"
//#include "Utils.h"

//configuration options
#define VAR_BMP 0.5
#define VAR_BNO 0.5
#define VAR_MMA 0.5
using namespace Eigen;
class AltFilter{
public:
  AltFilter();
  void update(SensorData data);
  float getAltitude();
  float getVelocity();
private:

  Vector3f X;
  Vector2f Z;
  Matrix3f F;
  Matrix3f Q;
  Matrix3f P;
  Matrix<float, 3, 2> K;
  Matrix<float, 2, 3> H;
  Matrix2f R;

  void prefilter(SensorData data);
  void kalmanPredict();
  void kalmanUpdate();
};
