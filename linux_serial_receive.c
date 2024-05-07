#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>

#define SERIAL_PORT "/dev/ttyS0" // 串口设备文件路径
#define OUTPUT_FILE "received_file.txt" // 接收到的文件名

int main() {
    int serial_port;
    FILE *output_file;
    char buffer[1024];
    ssize_t bytes_read;
    struct termios tty;
    struct pollfd fds[1];

    // 打开串口
    serial_port = open(SERIAL_PORT, O_RDWR);
    if (serial_port < 0) {
        perror("Failed to open serial port");
        return 1;
    }

    // 设置串口属性
    tcgetattr(serial_port, &tty);
    cfsetospeed(&tty, B115200); // 设置波特率为 115200
    tty.c_cflag &= ~PARENB; // 无校验
    tty.c_cflag &= ~CSTOPB; // 1位停止位
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8位数据位
    tcsetattr(serial_port, TCSANOW, &tty);

    // 打开输出文件
    output_file = fopen(OUTPUT_FILE, "wb");
    if (output_file == NULL) {
        perror("Failed to open output file");
        close(serial_port);
        return 1;
    }

    // 设置 pollfd 结构体
    fds[0].fd = serial_port;
    fds[0].events = POLLIN;

    // 主循环：等待串口数据并写入文件
    while (1) {
        // 使用 poll 函数等待串口数据就绪
        if (poll(fds, 1, -1) > 0) {
            if (fds[0].revents & POLLIN) {
                bytes_read = read(serial_port, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    fwrite(buffer, 1, bytes_read, output_file);
                }
            }
        }
    }

    // 关闭文件和串口
    fclose(output_file);
    close(serial_port);

    return 0;
}
