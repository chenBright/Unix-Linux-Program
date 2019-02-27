# C++回声服务器_5-多进程版本

服务器和客户端都是用多进程来接收和发送数据。

## 服务器代码

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 30;

void error_handling(const char *messgae);
void read_childproc(int sig);

// 接收一个参数，argv[1]为端口号
int main(int argc, char *argcv[]) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;

    pid_t pid;
    struct sigaction act;
    socklen_t addr_size;
    int str_len, state;
    char buf[BUF_SIZE];

    if (argc != 2) {
        printf("Usgae : %s <port>\n", argcv[0]);
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argcv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(server_sock, 5) == -1) {
        error_handling("listen() error");
    }

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock =accept(server_sock, (struct sockaddr*)&server_addr, &addr_size);
        if (client_sock == -1) {
            continue;
        } else {
            puts("new client connected...");
        }

        pid = fork();
        if (pid == 0) {
            close(server_sock);
            while ((str_len = read(client_sock, buf, BUF_SIZE)) != 0) {
                write(client_sock, buf, str_len);
            }
            close(client_sock);
            puts("client disconnected...");
            return 0;
        } else {
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}
```

## 客户端代码

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 30;

void error_handling(const char *messgae);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

// 接收两个参数，argv[1]为IP地址，argv[2]为端口号
int main(int argc, char *argv[]) {
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in server_addr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        error_handling("connect() error");
    }

    pid = fork();
    if (pid == 0) {
        write_routine(sock, buf);
    } else {
        read_routine(sock, buf);
    }
    close(sock);
    return 0;
}
```

## 辅助函数

```c++
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>

const int BUF_SIZE = 30;

// 处理错误
void error_handling(const char *message) {
    printf("%s", message);
    exit(1);
}

// 读取进程退出状态
void read_childproc(int sig) {
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d\n", pid);
}

// 客户端接收数据
void read_routine(int sock, char *buf) {
    while (1) {
        ssize_t str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0) {
            return;
        }
        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}

// 客户端发送数据
void write_routine(int sock, char *buf) {
    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}
```



## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)

