#pragma once

#include "SensorData.h"
#include "Poster.hpp"

//This needs to be in this order. We need to include Arduino.h (if not already included), then undefine the Arduino `abs` macro, then pull in Eigen. Pray Rust comes quickly.
#include "Arduino.h"
#undef abs
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
  Poster<float> p_alt;
  Poster<float> p_vel;

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
