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
