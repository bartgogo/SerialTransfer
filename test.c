#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    int fd;
    struct termios serial;

    fd = open("/dev/ttySAC2", O_RDWR | O_NOCTTY|O_NDELAY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    memset(&serial, 0, sizeof(serial));
    serial.c_iflag = 0;
    serial.c_oflag = 0;
    serial.c_cflag&=~CSIZE;
    serial.c_cflag = CS8 | CREAD | CLOCAL;
    serial.c_lflag = 0;
    serial.c_cc[VMIN] = 1;
    serial.c_cc[VTIME] = 0;
    cfmakeraw(&serial);
    cfsetospeed(&serial, B115200);
    cfsetispeed(&serial, B115200);
    serial.c_cflag&=~CSTOPB;
    tcflush(fd,TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &serial) != 0) {
        perror("tcsetattr");
        return 1;
    }
    if(fcntl(fd,F_SETFL,0)<0)

    {
    perror("fcntl F_SETFL\n");

    }
    while (1) {
        char buf[1024];
        ssize_t len = read(fd, buf, sizeof(buf));
        printf("Received: %s", buf);// Sleep for 100ms
    }

    close(fd);
    return 0;
}