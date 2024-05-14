#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/select.h>

#define BUF_SIZE 1024 // 缓冲区大小

// 函数声明
int receiveFileFromPC(const char *serial_path);
int sendFileToPC(const char *filename, const char *serial_path);

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 3) {
        printf("Usage: %s <send | receive> <serial_port_path>\n", argv[0]);
        return 1;
    }

    char *serial_port_path = argv[2];

    // 根据参数选择接收程序或发送程序
    if (strcmp(argv[1], "send") == 0) {
        // 发送程序
        char filename[256];
        printf("Enter the file name to send: ");
        scanf("%s", filename);
        int result = sendFileToPC(filename, serial_port_path);
        if (result != 0) {
            printf("Error sending file to PC.\n");
            return 1;
        } else {
            printf("File sent successfully.\n");
        }
    } else if (strcmp(argv[1], "receive") == 0) {
        // 接收程序
        int result = receiveFileFromPC(serial_port_path);
        if (result != 0) {
            printf("Error receiving file from PC.\n");
            return 1;
        } else {
            printf("File received successfully.\n");
        }
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

// 函数定义：接收文件从 PC
int receiveFileFromPC(const char *serial_path) {
    int serial_port;
    FILE *file;
    char filename[256];
    char size_buffer[20];
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    long file_size;
    long bytes_written = 0;
    fd_set read_fds;
    struct timeval timeout;

    // 打开串口
    serial_port = open(serial_path, O_RDWR);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // 使用 select 监视串口是否可读
    FD_ZERO(&read_fds);
    FD_SET(serial_port, &read_fds);

    // 读取文件名
    memset(filename, 0, sizeof(filename));
    if (select(serial_port + 1, &read_fds, NULL, NULL, NULL) > 0) {
        bytes_read = read(serial_port, filename, sizeof(filename) - 1);
        if (bytes_read <= 0) {
            perror("Error reading filename");
            close(serial_port);
            return 1;
        }
    } else {
        perror("Error or timeout waiting for filename");
        close(serial_port);
        return 1;
    }

    // 读取文件大小
    memset(size_buffer, 0, sizeof(size_buffer));
    if (select(serial_port + 1, &read_fds, NULL, NULL, NULL) > 0) {
        bytes_read = read(serial_port, size_buffer, sizeof(size_buffer) - 1);
        if (bytes_read <= 0) {
            perror("Error reading file size");
            close(serial_port);
            return 1;
        }
    } else {
        perror("Error or timeout waiting for file size");
        close(serial_port);
        return 1;
    }
    file_size = strtol(size_buffer, NULL, 10);

    // 打开文件
    file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        close(serial_port);
        return 1;
    }

    // 使用 select 监视串口是否可读，读取文件内容并写入文件
    while (bytes_written < file_size) {
        FD_ZERO(&read_fds);
        FD_SET(serial_port, &read_fds);

        if (select(serial_port + 1, &read_fds, NULL, NULL, NULL) > 0) {
            bytes_read = read(serial_port, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                perror("Error reading from serial port");
                fclose(file);
                close(serial_port);
                return 1;
            }
            fwrite(buffer, 1, bytes_read, file);
            bytes_written += bytes_read;
        } else {
            perror("Error or timeout waiting for file content");
            fclose(file);
            close(serial_port);
            return 1;
        }
    }

    // 关闭文件和串口
    fclose(file);
    close(serial_port);

    return 0;
}

// 函数定义：发送文件到 PC
int sendFileToPC(const char *filename, const char *serial_path) {
    int serial_port;
    FILE *file;
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    long file_size;
    int bytes_sent;

    // 打开串口
    serial_port = open(serial_path, O_RDWR);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // 打开文件
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        close(serial_port);
        return 1;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 发送文件名和文件大小到 PC
    bytes_sent = write(serial_port, filename, strlen(filename));
    if (bytes_sent <= 0) {
        perror("Error sending filename");
        fclose(file);
        close(serial_port);
        return 1;
    }

    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = write(serial_port, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        perror("Error sending file size");
        fclose(file);
        close(serial_port);
        return 1;
    }

    // 发送文件内容到 PC
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = write(serial_port, buffer, bytes_read);
        if (bytes_sent <= 0) {
            perror("Error sending file content");
            fclose(file);
            close(serial_port);
            return 1;
        }
    }

    // 关闭文件和串口
    fclose(file);
    close(serial_port);

    return 0;
}
