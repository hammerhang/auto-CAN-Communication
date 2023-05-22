# auto-CAN-Communication
This project CAN solve the problem of CAN automatic parsing and packaging, reducing the code work of handwritten CAN parsing and packaging.

# 1. Features
- Languages: C/C++ CMake
- OS: linux
- Users can generate libraries into their own projects

# 2. Getting the Source
~~~shell
git clone https://github.com/hammerhang/auto-CAN-Communication.git
~~~

# 3. Building
This project supports CMake out of the box.

## 3.1 Quick start:
Before start, make sure your computer has the following library dependencies.

- boost
- yaml-cpp

Compile and execute:

~~~bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make
./auto-CAN-Communication
~~~

If you are using a computer that does not have a CAN interface, you CAN optionally execute this script and generate a virtual CAN.

~~~bash
auto-CAN-Communication/doc/vcanConfig.sh
~~~

## 3.2 Project migration:
Transfer the folder to your own project and connect to the library

# 4. manual
In auto-CAN-Communication/doc/manual.md, you can see the architecture of the system
![Architecture](https://github.com/hammerhang/auto-CAN-Communication/tree/main/doc/pictrue/Architecture.png)
