# SerialTransfer

PC机与开发板串口通信传输程序

## 概述

SerialTransfer 是一个用于在PC和开发板之间进行串口通信的程序库。它提供了必要的函数来高效地处理串口数据传输。

## 功能

- **串口通信**：支持通过串口发送和接收数据。
- **跨平台**：设计为可在多个操作系统上运行。
- **易于集成**：可以简单地集成到各种需要串口通信的项目中。

## 安装

1. 克隆仓库：
   
   sh
   
   复制代码
   
   `git clone https://github.com/bartgogo/SerialTransfer.git`

2. 进入目录：
   
   sh
   
   复制代码
   
   `cd SerialTransfer`

## 使用

1. 编译代码：
   
   sh
   
   复制代码
   
   `make`

2. 运行可执行文件：
   
   sh
   
   复制代码
   
   `./main.exe`

## 文件结构

- `main.c`：主程序文件，包含程序的入口函数和逻辑控制。
- `serial.c`：实现串口通信的核心函数。
- `serial.h`：串口通信函数的头文件，声明函数接口。
- `linux_serial_receive.c`：用于在Linux环境下接收串口数据的程序。
- `CMakeLists.txt`：CMake配置文件，用于构建项目。
- `Makefile`：Makefile文件，用于在Linux或者在windows gcc环境下编译项目。