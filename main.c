#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // 默认要发送的文件名
int com=1;
// 函数声明
int receiveFileFromLinux(PORT com_port);
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
        printf("Enter the com port radix");
        scanf("%d",&com);
        PORT com_port = serial_init(com, 115200, 8, ONESTOPBIT, NOPARITY);
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
        printf("Enter the com port radix\n");
        scanf("%d",&com);
        PORT com_port = serial_init(com, 115200, 8, ONESTOPBIT, NOPARITY);
        if (com_port == NULL) {
            printf("Failed to open serial port.\n");
            return 1;
        }
        int ret = receiveFileFromLinux(com_port);
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
int receiveFileFromLinux(PORT com_port) {
    FILE *file;
    char buffer[200];
    size_t bytes_read;
    int bytes_written;
    // 读取文件名
    bytes_read = ReciveData(com_port, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        printf("Failed to receive filename.\n");
        return 1;
    }
    buffer[bytes_read] = '\0'; // Add null terminator
    printf("bytes_read=%d,Received filename: %s\n",bytes_read, buffer);

    // 打开文件
    file = fopen(buffer, "wb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", buffer);
        fclose(file);
        return 1;
    }
	printf("创建文件完成");
    memset(buffer, 0, sizeof(buffer));
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
    memset(buffer, 0, sizeof(buffer));
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
        memset(buffer, 0, sizeof(buffer));
    }

    // 关闭文件
    fclose(file);

    return 0;
}

// 函数定义：发送文件到 Linux 开发板
int sendFileToLinux(const char *filename, PORT com_port) {
    FILE *file;
    char buffer[200];
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
	Sleep(1000);
	
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
	long b_sent = 0;
    // 发送文件大小到 Linux 开发板
    char size_buffer[20];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    bytes_sent = SendData(com_port, size_buffer, strlen(size_buffer));
    if (bytes_sent <= 0) {
        printf("Failed to send file size.\n");
        fclose(file);
        return 1;
    }
	Sleep(1000);
    // 发送文件内容到 Linux 开发板
	while (!feof(file)){
		memset(buffer, 0, sizeof(buffer));
		// buffer : 将文件读取到内存的位置
		// sizeof(char) : 读取的基本单元字节长度
		// sizeof(buffer) : 读取的基本单元个数,
		//       读取字节个数是 sizeof(buffer) * sizeof(char)
		// p : 文件指针
		//fread(buffer, sizeof(char), sizeof(buffer), p);
		if ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
		{
			bytes_sent = SendData(com_port, buffer, bytes_read);
			b_sent += bytes_sent;
		}
		// 打印读取的内容
	}


	printf("send file size.%ld\n,real_size=%ld", b_sent, file_size);
    // 关闭文件
    fclose(file);

    return 0;
}
