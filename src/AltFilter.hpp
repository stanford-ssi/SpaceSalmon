#pragma once

class AltFilter;

#include "AltFilterTask.hpp"

#include <Eigen30.h>
#include <Eigen/Dense>

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
