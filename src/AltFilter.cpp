#include "AltFilter.hpp"
#include "main.hpp"

AltFilter::AltFilter(){

  X << 0,
       0,
       0;

  Z << 0,
       0;

  F << 1, 0.02, 0, //this depends on time step???
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

void AltFilter::init(SensorData& data){
  prefilter(data);
  X(0) = Z(0);
}

void AltFilter::update(SensorData& data){
  kalmanPredict();
  prefilter(data);
  if(X(1) > 343*0.7) R(0) = 100000;
  else {
    R(0) = 10 + pow(abs(X(1)/20),2) + abs(X(2))*0.5;
    if(R(0)>500) R(0) = 500;
  }
  kalmanUpdate();
  
  if(xTaskGetTickCount() - print_timer > 500){
    print_timer = xTaskGetTickCount();
    logState();
  }
}

float AltFilter::getAltitude(){ //meters, ASL
  return X(0);
}

float AltFilter::getVelocity(){ //meters/s
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
  Z(0) = p2alt((data.bmp1_data.pressure + data.bmp2_data.pressure)/2);  //LOL gotta convert to meters oops
  Z(1) = data.adxl1_data.y * -1.0 ; //NEGATIVE! If the accelerometers read -9.8 when the rocket is vertical,
  //then you should have a -1.0 term here, so that at rest the filter sees normal force accleratrion upwards (positive).
}

float AltFilter::p2alt(float p){
  return (1.0-(pow(((double)p/101350.0),0.190284)))*145366.45*0.3048;
}

void AltFilter::logState(){
  StaticJsonDocument<500> json;
  json["tick"] = xTaskGetTickCount();
  JsonArray x_json = json.createNestedArray("x");
  x_json.add(X(0));
  x_json.add(X(1));
  x_json.add(X(2));
  sys.tasks.logger.logJSON(json, "filter_state");
}