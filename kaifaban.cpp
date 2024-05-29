/**
  ******************************************************************************
  * @file           : kaifaban.cpp
  * @author         : JackZhao
  * @brief          : None
  * @attention      : None
  * @date           : 2024/5/29
  ******************************************************************************
  */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>

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

// 设置串口属性
int setSerialAttributes(int fd, int speed, int parity) {
    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0; // no signaling chars, no echo,
                     // no canonical processing
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_cc[VMIN] = 1; // non read blocks
    tty.c_cc[VTIME] = 0; // no timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
                                     // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
   // tcflush(fd,TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

// 函数定义：接收文件从 PC
int receiveFileFromPC(const char *serial_path) {
    int serial_port;
    FILE *file;
    char filename[256];
    char size_buffer[32];
    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    long file_size;
    long bytes_written = 0;


    // 打开串口
    serial_port = open(serial_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // 设置串口属性
    if (setSerialAttributes(serial_port, B115200, 0) != 0) {
        close(serial_port);
        return 1;
    }

    // 读取文件名
    memset(filename, 0, sizeof(filename));
    char a;
    while(1)
    {
    read(serial_port,&a,1);
    if(a=='a')
        break;
    }

    bytes_read = read(serial_port, filename, sizeof(filename));
    printf("文件名为%s,读取字节为%d",filename,bytes_read);
    if (bytes_read <= 0) {
        perror("Error reading filename");
        close(serial_port);
        return 1;
    }

    // 读取文件大小
    memset(size_buffer, 0, sizeof(size_buffer));
    bytes_read = read(serial_port, size_buffer, sizeof(size_buffer));
    if (bytes_read <= 0) {
        perror("Error reading file size");
        close(serial_port);
        return 1;
    }
    file_size = strtol(size_buffer, NULL, 10);
    printf("文件大小%d",file_size);
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
    printf("总共收入字节:%d",bytes_written);
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
    serial_port = open(serial_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // 设置串口属性
    if (setSerialAttributes(serial_port, B115200, 0) != 0) {
        close(serial_port);
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
    sleep(1);
    char size_buffer[32];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = write(serial_port, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        perror("Error sending file size");
        fclose(file);
        close(serial_port);
        return 1;
    }
    sleep(1);

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
