#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024 // 缓冲区大小

int main() {
    int serial_port;
    FILE *file;
    char filename[256];
    char size_buffer[20];
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    long file_size;
    long bytes_written = 0;

    // 打开串口
    serial_port = open("/dev/ttyS0", O_RDWR);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // 读取文件名
    memset(filename, 0, sizeof(filename));
    bytes_read = read(serial_port, filename, sizeof(filename) - 1);
    if (bytes_read <= 0) {
        perror("Error reading filename");
        close(serial_port);
        return 1;
    }

    // 读取文件大小
    memset(size_buffer, 0, sizeof(size_buffer));
    bytes_read = read(serial_port, size_buffer, sizeof(size_buffer) - 1);
    if (bytes_read <= 0) {
        perror("Error reading file size");
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

    // 读取文件内容并写入文件
    while (bytes_written < file_size) {
        bytes_read = read(serial_port, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            perror("Error reading from serial port");
            fclose(file);
            close(serial_port);
            return 1;
        }
        fwrite(buffer, 1, bytes_read, file);
        bytes_written += bytes_read;
    }

    // 关闭文件和串口
    fclose(file);
    close(serial_port);

    return 0;
}
