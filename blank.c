#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int serial_init(const char *device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("open_port: Unable to open serial device");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int sendFileToLinux(const char *filename, int fd);
int receiveFileFromLinux(int fd);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <send | receive> <serial_port>\n", argv[0]);
        return 1;
    }

    int baudrate = B115200;
    const char *serial_port = argv[2];
    int fd = serial_init(serial_port, baudrate);
    if (fd == -1) {
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        char filename[256];
        printf("Enter the file name to send: ");
        scanf("%s", filename);
        if (sendFileToLinux(filename, fd) != 0) {
            printf("Failed to send file to Linux board.\n");
        } else {
            printf("File sent successfully.\n");
        }
    } else if (strcmp(argv[1], "receive") == 0) {
        if (receiveFileFromLinux(fd) != 0) {
            printf("Failed to receive file from Linux board.\n");
        } else {
            printf("File received successfully.\n");
        }
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    close(fd);
    return 0;
}

int receiveFileFromLinux(int fd) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;

    fd_set readfds;
    struct timeval timeout;

    // Read filename
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int retval = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (retval == -1) {
        perror("select()");
        return 1;
    } else if (retval) {
        bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Failed to receive filename.\n");
            return 1;
        }
        buffer[bytes_read] = '\0';
    } else {
        printf("No data within five seconds.\n");
        return 1;
    }

    printf("Received filename: %s\n", buffer);

    // Open file
    file = fopen(buffer, "wb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", buffer);
        return 1;
    }

    // Read file size
    memset(buffer, 0, sizeof(buffer));
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    retval = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (retval == -1) {
        perror("select()");
        fclose(file);
        return 1;
    } else if (retval) {
        bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Failed to receive file size.\n");
            fclose(file);
            return 1;
        }
        buffer[bytes_read] = '\0';
    } else {
        printf("No data within five seconds.\n");
        fclose(file);
        return 1;
    }

    long file_size = strtol(buffer, NULL, 10);
    printf("Received file size: %ld\n", file_size);

    // Receive file content
    long bytes_received = 0;
    while (bytes_received < file_size) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        retval = select(fd + 1, &readfds, NULL, NULL, &timeout);
        if (retval == -1) {
            perror("select()");
            fclose(file);
            return 1;
        } else if (retval) {
            bytes_read = read(fd, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                printf("Failed to receive file data.\n");
                fclose(file);
                return 1;
            }
            fwrite(buffer, 1, bytes_read, file);
            bytes_received += bytes_read;
        } else {
            printf("No data within five seconds.\n");
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int sendFileToLinux(const char *filename, int fd) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_sent;

    // Open file
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", filename);
        return 1;
    }

    // Send filename
    bytes_sent = write(fd, filename, strlen(filename));
    if (bytes_sent <= 0) {
        printf("Failed to send filename.\n");
        fclose(file);
        return 1;
    }
    sleep(2);

    // Send file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char size_buffer[32];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = write(fd, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        printf("Failed to send file size.\n");
        fclose(file);
        return 1;
    }
    sleep(2);

    // Send file content
    while (!feof(file)) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = fread(buffer, 1, sizeof(buffer), file);
        if (bytes_read > 0) {
            bytes_sent = write(fd, buffer, bytes_read);
            if (bytes_sent < 0) {
                printf("Failed to send file data.\n");
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}
