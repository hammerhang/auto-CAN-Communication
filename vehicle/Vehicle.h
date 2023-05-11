#ifndef VEHICLE_H
#define VEHICLE_H

#include "_Communicate.h"
#include "_VehicleAction.h"

// typedef struct VehicleAttribute{
//     float Lenth; //车长
//     float Width; //车宽
//     float WheelTrack; //轮距
//     float WheelBase; //轴距
// } VehicleAttribute_t;

// typedef struct VehicleStates{
    
// } VehicleStates_t;

class Vehicle : virtual public Communicate, virtual public VehicleAction
{
private:
    // VehicleAttribute_t VehicleAtti; //车辆属性
public:
    // VehicleAttribute_t GetVehicleAttibute(); //获取车辆属性
};


#endif