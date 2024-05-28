# SerialTransfer

![License](https://img.shields.io/badge/license-MIT-blue.svg)
<img title="" src="https://img.shields.io/badge/build-passing-brightgreen.svg" alt="Build" data-align="inline">
<img alt="Tests Passing" src="https://github.com/anuraghazra/github-readme-stats/workflows/Test/badge.svg" />

## 项目简介

SerialTransfer 是一个高效的串行数据传输库，适用于C/C++ 项目,并且横跨linux与windows平台。它提供简单且灵活的 API，使得串行通信变得轻松和高效。

## 目录

- [功能](#功能)
- [安装](#安装)
- [快速开始](#快速开始)
- [使用方法](#使用方法)
- [贡献](#贡献)
- [许可](#许可)

## 功能

- 高效的串行数据传输
- 简单易用的 API
- 支持多种平台（Linux,  Windows）
- 可扩展和定制

## 安装

### 前提条件

确保已安装以下软件：

- CMake 3.22.1 及以上

- GCC 或 Clang 编译器

- Git
  
  ### 快速开始
1. 克隆仓库：
   
   ```sh
   git clone https://github.com/bartgogo/SerialTransfer.git
   cd SerialTransfer
   ```

2. 创建构建目录并构建项目：
   
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
   
   或者直接使用makefile
   
   ```shell
   make
   ```

### 使用方法

1. 编译代码：
   
   复制代码
   
   `make`

2. 运行可执行文件：
   
   sh
   
   复制代码
   
   windows端的串口程序
   
   发送用send 接收用receive
   
   ```cpp
   ./main.exe send
   #然后填写串口名和文件名即可发送
   ```
   
   ![send_tu](https://img.picgo.net/2024/05/28/-2024-05-28-1830149e3ff862df4d0191.png)
   
   3. linux开发板串口send测试用例 以num.c为发送文件示例
      
      ```cpp
      ./serial send /dev/pts/3
      #另一个程序
      ./serial receive /dev/pts/2
      ```
      
      ![send](https://img.picgo.net/2024/05/28/send751f45413c39179e.png)
      ![receive](https://img.picgo.net/2024/05/28/receivea3142c39d2d69fdf.png)
      ![num](https://img.picgo.net/2024/05/28/num6585f474450c004d.png)

#### 文件结构

- `main.c`：主程序文件，包含程序的入口函数和逻辑控制。
- `serial.c`：实现windows平台串口通信的核心函数。
- `serial.h`：串口通信函数的头文件，声明函数接口。
- `linux_serial_receive.c`：用于在Linux环境下串口发送与接收程序。
- `CMakeLists.txt`：CMake配置文件，用于构建项目。
- `Makefile`：Makefile文件，用于在Linux或者在windows gcc环境下编译项目。

### 贡献

### 许可

该项目基于 MIT 许可证，详情请参阅 LICENSE 文件。
