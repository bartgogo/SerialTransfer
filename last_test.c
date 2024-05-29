#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    int fd;
    struct termios options;
    char buffer[255];
    int bytes_read;

    // 打开串口设备
    fd = open("/dev/ttySAC2", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("open_port: Unable to open serial port - ");
        return(-1);
    }

    // 获取并设置串口选项
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200); // 输入波特率
    cfsetospeed(&options, B115200); // 输出波特率

    // 设置为非阻塞模式
    options.c_cc[VMIN]  = 0; // 最小字符数
    options.c_cc[VTIME] = 0; // 等待时间
    tcsetattr(fd, TCSANOW, &options);

    // 非阻塞读取
    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // 确保字符串以 null 结尾
        printf("Read: %s\n", buffer);
    } else if (bytes_read == 0) {
        printf("No data available yet\n");
    } else {
        // 错误处理
        perror("read: ");
    }

    // 关闭串口
    close(fd);

    return 0;
}