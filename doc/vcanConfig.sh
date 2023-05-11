#! /bin/bash
# 在没有CAN设备的机器上添加虚拟CAN设备以进行软件实验
sudo modprobe vcan #载入vcan设备
sudo ip link add dev vcan0 type vcan
sudo ip link set dev vcan0 up #开启vcan0
sudo ip link add dev vcan1 type vcan
sudo ip link set dev vcan1 up #开启vcan1
# 虽然现实世界中的一个简单解决方案是通过将设备物理连接在一起来解决的，但我需要一个可以在计算机上运行的程序化解决方案。
sudo modprobe can-gw #载入can网关
sudo cangw -A -s vcan0 -d vcan1 -e #将消息从 vcan0 路由到 vcan1
sudo cangw -A -s vcan1 -d vcan0 -e #将消息从 vcan1 路由到 vcan0 到这里就可以
# 现在，能够在 vcan1 上也看到在 vcan0 上发送的所有消息，反之亦然。

# sudo ip link set dev vcan0 down #关闭vcan0
# sudo ip link del dev vcan0 #删除vcan0
