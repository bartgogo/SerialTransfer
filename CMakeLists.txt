cmake_minimum_required(VERSION 3.2)
project(SerialTransfer)

set(CMAKE_C_STANDARD 11)
if(WIN32)
    # Windows-specific settings
    add_executable(SerialTransfer main.c
            serial.c)
elseif(UNIX)
    # Linux-specific settings
	set(CMAKE_SYSTEM_PROCESSOR arm)
	set(CMAKE_SYSTEM_NAME linux)
	set(CROSS_CHAIN_PATH /usr/bin/)
    	
set(CMAKE_C_COMPILER "${CROSS_CHAIN_PATH}arm-linux-gnueabi-gcc")
	add_executable(SerialTransfer linux_serial_receive.c)
endif()

