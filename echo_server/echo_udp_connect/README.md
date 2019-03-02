# C++回声服务器_4-UDP connect版本客户端

针对UDP套接字调用connect函数不会与对方UDP套接字建立连接，只是向UDP套接字注册目标IP和端口信息。

## 修改客户端代码

服务器代码不需要修改，只需修改客户端代码。调用connect函数之后，可以调用write函数和read函数来发送、接收数据，而不需要调用sendto函数和recvfrom函数。

```c++
include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 30;

void error_handling(const char *message);

// 接收两个参数，argv[1]为IP地址，argv[2]为端口号
int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    ssize_t str_len;

    struct sockaddr_in server_addr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    // 地址信息初始化
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPV4 地址族
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // 服务器IP地址
    server_addr.sin_port = htons(atoi(argv[2])); // 服务器端口号

    while (1) {
        fputs("Insert message(q or Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        // 如果输入q或者Q，则退出
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }

        write(sock, message, strlen(message));
        str_len = read(sock, message, sizeof(message) - 1);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);

    return 0;
}
```

## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)

