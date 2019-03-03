# C++回声服务器_7-select版本服务器

这次使用select函数实现服务器，而客户端直接使用[完美回声服务器的客户端](../echo_perfect/README.md)。

## 服务器代码

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

const int BUF_SIZE = 100;

void error_handling(const char *buf);

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    sockaddr_in server_addr, client_addr;
    timeval timeout;
    fd_set reads, copy_reads;

    socklen_t addr_size;
    int fd_max, fd_num, i;
    ssize_t str_len;
    char buf[BUF_SIZE];

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind() error");
        exit(1);
    }

    if (listen(server_sock, 5) == -1) {
        error_handling("listen() error");
    }

    FD_ZERO(&reads);
    FD_SET(server_sock, &reads);
    fd_max = server_sock;

    while (1) {
        copy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max + 1, &copy_reads, 0, 0, &timeout)) == -1) {
            break;
        }

        if (fd_num == 0) {
            continue;
        }

        for (int j = 0; j < fd_max + 1; ++j) {
            if (FD_ISSET(j, &copy_reads)) { // 文件描述符在监听的范围内
                if (j == server_sock) { // 服务器套接字发生变化
                    addr_size = sizeof(client_addr);
                    client_sock = accept(server_sock, (sockaddr*)&client_addr, &addr_size);
                    FD_SET(client_sock, &reads);
                    if (fd_max < client_sock) {
                        fd_max = client_sock;
                    }
                    printf("connected client: %d\n", client_sock);
                } else { // 非套接字，即有需要接收的数据
                    str_len = read(j, buf, BUF_SIZE);
                    if (str_len == 0) {
                        FD_CLR(j, &reads);
                        close(i);
                        printf("closed client: %d\n", j);
                    } else {
                        write(j, buf, (size_t)str_len);
                    }
                }
            }
        }
    }
    close(server_sock);

    return 0;
}
```

## 缺点

- 需要编写以监视状态变化为目的的针对所有文件描述符的循环语句；
- 每次调用select函数时需要传递监视对象信息。

## 优点

- 系统兼容性好，几乎所有的平台都支持。

## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)
