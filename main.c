#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // 默认要发送的文件名

// 函数声明
int receiveFileFromLinux(const char *outputFilename, PORT com_port);
int sendFileToLinux(const char *filename, PORT com_port);

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 2) {
        printf("Usage: %s <send | receive>\n", argv[0]);
        return 1;
    }

    // 根据参数选择接收程序或发送程序
    if (strcmp(argv[1], "send") == 0) {
        // 发送程序
        char filename[256];
        printf("Enter the file name to send: ");
        scanf("%s", filename);
        PORT com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
        if (com_port == NULL) {
            printf("Failed to open serial port.\n");
            return 1;
        }
        int ret = sendFileToLinux(filename, com_port);
        if (ret != 0) {
            printf("Failed to send file to Linux board.\n");
        } else {
            printf("File sent successfully.\n");
        }
        ClosePort(com_port);
    } else if (strcmp(argv[1], "receive") == 0) {
        // 接收程序
        PORT com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
        if (com_port == NULL) {
            printf("Failed to open serial port.\n");
            return 1;
        }
        int ret = receiveFileFromLinux("received_file.txt", com_port);
        if (ret != 0) {
            printf("Failed to receive file from Linux board.\n");
            ClosePort(com_port);
            return 1;
        } else {
            printf("File received successfully.\n");
        }
        ClosePort(com_port);
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

// 函数定义：接收文件从 Linux 开发板
int receiveFileFromLinux(const char *outputFilename, PORT com_port) {
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    int bytes_written;

    // 打开文件
    file = fopen(outputFilename, "wb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", outputFilename);
        return 1;
    }

    // 读取文件名
    bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        printf("Failed to receive filename.\n");
        fclose(file);
        return 1;
    }
    buffer[bytes_read] = '\0'; // Add null terminator
    printf("Received filename: %s\n", buffer);

    // 读取文件大小
    bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        printf("Failed to receive file size.\n");
        fclose(file);
        return 1;
    }
    buffer[bytes_read] = '\0'; // Add null terminator
    long file_size = strtol(buffer, NULL, 10);
    printf("Received file size: %ld\n", file_size);

    // 接收文件内容并写入文件
    long bytes_received = 0;
    while (bytes_received < file_size) {
        bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Failed to receive file data.\n");
            fclose(file);
            return 1;
        }
        fwrite(buffer, 1, bytes_read, file);
        bytes_received += bytes_read;
    }

    // 关闭文件
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

    // 发送文件名到 Linux 开发板
    bytes_sent = SendData(com_port, filename, strlen(filename));
    if (bytes_sent <= 0) {
        printf("Failed to send filename.\n");
        fclose(file);
        return 1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 发送文件大小到 Linux 开发板
    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = SendData(com_port, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        printf("Failed to send file size.\n");
        fclose(file);
        return 1;
    }

    // 发送文件内容到 Linux 开发板
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = SendData(com_port, buffer, bytes_read);
        if (bytes_sent != bytes_read) {
            printf("Failed to send file data.\n");
            fclose(file);
            return 1;
        }
    }

    // 关闭文件
    fclose(file);

    return 0;
}
