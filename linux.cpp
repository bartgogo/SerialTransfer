/**
  ******************************************************************************
  * @file           : linux.cpp
  * @author         : JackZhao
  * @brief          : None
  * @attention      : None
  * @date           : 2024/5/29
  ******************************************************************************
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int set_serial_attributes(int fd, int speed, int parity) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

int receive_file(int fd) {
    char buffer[1024];
    int bytes_read;

    // 接收文件名
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        printf("Failed to receive filename.\n");
        return 1;
    }
    buffer[bytes_read] = '\0';
    printf("Received filename: %s\n", buffer);

    // 打开文件
    FILE *file = fopen(buffer, "wb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", buffer);
        return 1;
    }

    // 接收文件大小
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        printf("Failed to receive file size.\n");
        fclose(file);
        return 1;
    }
    buffer[bytes_read] = '\0';
    long file_size = strtol(buffer, NULL, 10);
    printf("Received file size: %ld\n", file_size);

    // 接收文件内容并写入文件
    long bytes_received = 0;
    while (bytes_received < file_size) {
        bytes_read = read(fd, buffer, sizeof(buffer));
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
    printf("File received successfully.\n");
    return 0;
}

int send_file(int fd, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", filename);
        return 1;
    }

    // 发送文件名
    write(fd, filename, strlen(filename) + 1);
    sleep(2);

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 发送文件大小
    char size_buffer[32];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    write(fd, size_buffer, strlen(size_buffer) + 1);
    sleep(2);

    // 发送文件内容
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(fd, buffer, bytes_read);
    }

    fclose(file);
    printf("File sent successfully.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <send | receive> <serial_port>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *portname = argv[2];

    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (set_serial_attributes(fd, B115200, 0) != 0) {
        close(fd);
        return 1;
    }

    if (strcmp(mode, "send") == 0) {
        char filename[256];
        printf("Enter the file name to send: ");
        scanf("%s", filename);
        if (send_file(fd, filename) != 0) {
            close(fd);
            return 1;
        }
    } else if (strcmp(mode, "receive") == 0) {
        if (receive_file(fd) != 0) {
            close(fd);
            return 1;
        }
    } else {
        printf("Invalid option: %s\n", mode);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
