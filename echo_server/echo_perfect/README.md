# C++回声服务器_2-修复客户端问题

[C++回声服务器_1-简单版本](https://segmentfault.com/a/1190000018259461)中的问题出在客户端。客户端通过write函数一次性发送数据，过一段时间再调用一次read函数，期望接收传输的数据。问题在于这段时间到底是多久？理想的客户端应在接收到数据时立即读取数据。

## 改进

客户端发送数据时，可以知道数据的大小（长度）。客户端接收数据的时候，可以知道接收到数据的大小（长度）。所以，客户端循环调用read函数，直到接收到的数据总大小（长度）等于发送的数据的大小（长度）时，已完成所有数据的接收。

修改的代码：

```c++
// 发送的字符串长度、接收字符串的长度、每次read函数接受到字符串的长度
ssize_t str_len, recv_len, recv_cnt;

str_len = write(sock, message, strlen(message)); // 向服务器发送数据
recv_len = 0;
// 循环调用read函数，直到接收到所有数据为止
while (recv_len < str_len) {
	recv_cnt = read(sock, message, BUF_SIZE); // 读取来自客户端的服务器
	if (recv_cnt == -1) {
		error_handling("read() error");
	}
    recv_len += recv_cnt;
}
message[recv_len] = 0;
```

完整的客户端代码：

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;

void error_handling(const char *message);

// 接收两个参数，argv[1]为IP地址，argv[2]为端口号
int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUF_SIZE];
    // 发送的字符串长度、接收字符串的长度、每次read函数接受到字符串的长度
    ssize_t str_len, recv_len, recv_cnt;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    // 地址信息初始化
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPV4 地址族
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // 服务器IP地址
    server_addr.sin_port = htons(atoi(argv[2])); // 服务器端口号

    // 向服务器发送连接请求
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("connect() error");
    } else {
        printf("Connect...");
    }

    while (1) {
        printf("Input message( Q to quit ): ");
        fgets(message, BUF_SIZE, stdin);

        // 如果输入q或者Q，则退出
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }

        str_len = write(sock, message, strlen(message)); // 向服务器发送数据
        recv_len = 0;
        // 循环调用read函数，直到接收到所有数据为止
        while (recv_len < str_len) {
            recv_cnt = read(sock, message, BUF_SIZE); // 读取来自客户端的服务器
            if (recv_cnt == -1) {
                error_handling("read() error");
            }
            recv_len += recv_cnt;
        }
        message[recv_len] = 0;

        printf("Message from server: %s \n", message);
    }
    // 关闭连接
    close(sock);

    return 0;
}
```

## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)
