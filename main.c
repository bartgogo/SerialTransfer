#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // 要发送的文件名

// 函数声明
int sendFileToLinux(const char *filename, PORT com_port);

int main() {
    PORT com_port;
    int ret;

    // 初始化串口
    com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
    if (com_port == NULL) {
        printf("Failed to open serial port.\n");
        return 1;
    }

    // 发送文件名和文件字节数到 Linux 开发板
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", FILENAME);
        ClosePort(com_port);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    ret = SendData(com_port, FILENAME, strlen(FILENAME));
    if (ret != strlen(FILENAME)) {
        printf("Failed to send file name.\n");
        ClosePort(com_port);
        fclose(file);
        return 1;
    }

    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    ret = SendData(com_port, size_buffer, strlen(size_buffer));
    if (ret != strlen(size_buffer)) {
        printf("Failed to send file size.\n");
        ClosePort(com_port);
        fclose(file);
        return 1;
    }

    // 发送文件内容到 Linux 开发板
    ret = sendFileToLinux(FILENAME, com_port);
    if (ret != 0) {
        printf("Failed to send file to Linux board.\n");
    } else {
        printf("File sent successfully.\n");
    }

    // 关闭串口和文件
    ClosePort(com_port);
    fclose(file);

    return 0;
}

// 函数定义：发送文件到 Linux 开发板
int sendFileToLinux(const char *filename, PORT com_port) {
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    int bytes_sent;

    // 打开文件
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", filename);
        return 1;
    }

    // 读取文件内容并发送到串口
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = SendData(com_port, buffer, bytes_read);
        if (bytes_sent != bytes_read) {
            printf("Failed to send file data.\n");
            fclose(file);
            return 1;
        }
        // 等待一段时间，以便 Linux 开发板能够处理接收的数据
        Sleep(10);
    }

    // 关闭文件
    fclose(file);

    return 0;
}
