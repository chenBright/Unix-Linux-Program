# C++回声服务器_6-多进程pipe版本服务器

在服务器多进程版本的基础上，使用管道来向一个子进程发送接收到的数据，该子进程将接收到的数据保存到文件中。客户端代码不变。

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
    int fds[2]; // 管道描述符

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

    pipe(fds);
    pid = fork();
    if (pid == 0) { // 负责保存数据到文件的子进程
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int len;

        for (int i = 0; i < 10; ++i) {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
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
                write(fds[1], buf, str_len); // 发送数据给负责保存文件的子进程
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

## 参考

[《TCP/IP网络编程》](https://book.douban.com/subject/25911735/)
