#include "AltFilter.hpp"

AltFilter::AltFilter(){
  X << 0,
       0,
       0;

  Z << 0,
       0;

  F << 1, 0.02, 0,
       0, 1, 0.02,
       0, 0, 1;

  H << 1, 0, 0,
       0, 0, 1;


  Q << 0.027, 0.0014,  0.,
       0.0014,  0.05,    0.006,
       0.,  0.006,   1.,

  P << 0, 0, 0,
       0, 0, 0,
       0, 0, 0;

  R << 100, 0,
       0, 1;

}

void AltFilter::update(SensorData data){
  kalmanPredict();
  prefilter(data);
  if(X(1) > 343*0.7) R(0) = 100000;
  else {
    R(0) = 10 + pow(abs(X(1)/20),2) + abs(X(2))*0.5;
    if(R(0)>500) R(0) = 500;
  }
  kalmanUpdate();
}
/*
 * BE WARNED!!!! THIS FUNCTION RETURNNS FEET!!!!! NOT METERS!!!!
 */
float AltFilter::getAltitude(){
  return X(0)/0.3048;
}

float AltFilter::getVelocity(){
  return X(1);
}

void AltFilter::kalmanPredict(){
  X = F * X;
  P = F * P * F.transpose() + Q;
}

void AltFilter::kalmanUpdate(){
  K = P * H.transpose() * (H * (P * H.transpose()) + R).inverse();
  X = X + K * (Z - H * X);
  P = (Matrix3f::Identity() - K * H) * P;
}

void AltFilter::prefilter(SensorData data){
  /*Z(0) = p2alt((bmp_data.pressure1 + bmp_data.pressure2)/2)*0.3048;  //LOL gotta convert to meters oops
  Z(1) = mma_data.y;*/
}
