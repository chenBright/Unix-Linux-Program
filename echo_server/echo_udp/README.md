# C++回声服务器_3-UDP版本

这次我们实现一个UDP版本的回声服务器。

## 用于传输数据的函数

UDP套接字不会像TCP套接字那样保持连接状态，因此每次传输数据都要添加目标地址信息。

用于传输数据的函数：

1. 发送数据到目标服务器。

```c++
#include <sys/socket.h>

ssize_t sendto(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *to, socklen_t addrlen);
```

​	其中to为存有目标服务器地址信息的sockaddr结构体变量的地址值。

2. 接收来自服务器的数据。

```c++
#include <sys/socket.h>

ssize_t recvfrom(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);
```

​	其中from为存有发送端地址信息的sockaddr结构体变量的地址值

## 服务器代码

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 30;

void error_handling(const char *message);

// 接收一个参数，argv[1]为端口号
int main(int argc, char *argv[]) {
    int server_socket;

    char message[BUF_SIZE];
    ssize_t str_len;
    socklen_t client_addr_size;
    int i;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    server_socket = socket(PF_INET, SOCK_DGRAM, 0); // 创建IPv4 TCP socket
    if (server_socket == -1) {
        error_handling("UDP socket create error");
    }

    // 地址信息初始化
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPV4 地址族
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 使用INADDR_ANY分配服务器的IP地址
    server_addr.sin_port = htons(atoi(argv[1])); // 端口号由第一个参数设置

    // 分配地址信息
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(sockaddr)) == -1) {
        error_handling("bind() error");
    }

    while (1) {
        client_addr_size = sizeof(client_addr);
        // 读取来自客户端的数据
        str_len = recvfrom(server_socket, message, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_size);
        // 发送数据给客户端
        sendto(server_socket, message, str_len, 0, (struct sockaddr*)&client_addr, client_addr_size);
    }
    printf("echo server\n");

    return 0;
}
```

注：while循环内没有break语句，因此是无限循环，close函数不会执行。

## 客户端代码

```c++
#include <cstdio>
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
    socklen_t addr_size;

    struct sockaddr_in server_addr, from_addr;

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

        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(sockaddr)); // 发送数据到服务器
        addr_size = sizeof(from_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_addr, &addr_size); // 接收数据
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);

    return 0;
}
```

## UDP地址分配

UDP地址分配应在sendto函数调用前完成：

1. 调用bind函数。
2. 如果调用sendto函数是发现尚未分配地址信息，则在首次调用sendto函数时给相应的套接字自动分配IP和端口。

## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)

