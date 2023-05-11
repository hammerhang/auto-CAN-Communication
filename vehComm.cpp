#include <iostream>
#include <thread>
#include "Vehicle.h"

Vehicle vehicle;

//thread to write CAN frame
void CanWriteThreadHandler()
{
    while (true)
    {
        vehicle.Data2Frame();//Encapsulated data
        std::unordered_map<uint32_t, can_frame> CANFrameMap = vehicle.GetFrameMap();
        //write to can-frame
        for (std::unordered_map<uint32_t, can_frame>::iterator it = CANFrameMap.begin(); it != CANFrameMap.end(); ++it)
        {
            vehicle.Write("vcan0", CANFrameMap[it->first]);
        }
        usleep(1000 * 19);
    }
}

//thread to read CAN frame
void CanReadThreadHandler()
{
    while (true)
    {
        vehicle.Frame2Data(vehicle.Read("vcan0"));//Blocking read, and analysis can_frame
    }
}

int main()
{
    vehicle.SetCommunicateType(CAN);
    vehicle.Open({"vcan0"});

    std::thread CanWriteThread(CanWriteThreadHandler); CanWriteThread.detach();
    std::thread CanReadThread(CanReadThreadHandler); CanReadThread.detach();

    while(true)
    {
        std::cout << "running..." << std::endl;
        sleep(1);
    }
    return 0;
}