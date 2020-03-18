#pragma once

#include "SensorData.h"
//#include <WProgram.h>
#include <Eigen.h>
#include <Eigen/Dense>


using namespace Eigen;
class AltFilter{
public:
  AltFilter();
  void update(SensorData& data);
  void init(SensorData& data);
  float getAltitude();
  float getVelocity();
  void logState();

private:

  uint32_t print_timer;

  Vector3f X;
  Vector2f Z;
  Matrix3f F;
  Matrix3f Q;
  Matrix3f P;
  Matrix<float, 3, 2> K;
  Matrix<float, 2, 3> H;
  Matrix2f R;

  uint32_t data_time;

  void prefilter(SensorData& data);
  void kalmanPredict();
  void kalmanUpdate();
  float p2alt(float p);
};
