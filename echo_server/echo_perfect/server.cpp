#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;

void error_handling(const char *message);

// 接收一个参数，argv[1]为端口号
int main(int argc, char *argv[]) {
    int server_socket;
    int client_sock;

    char message[BUF_SIZE];
    ssize_t str_len;
    int i;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    server_socket = socket(PF_INET, SOCK_STREAM, 0); // 创建IPv4 TCP socket
    if (server_socket == -1) {
        error_handling("socket() error");
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

    // 监听连接请求，最大同时连接数为5
    if (listen(server_socket, 5) == -1) {
        error_handling("listen() error");
    }

    client_addr_size = sizeof(client_addr);
    for (i = 0; i < 5; ++i) {
        // 受理客户端连接请求
        client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_sock == -1) {
            error_handling("accept() error");
        } else {
            printf("Connect client %d\n", i + 1);
        }

        // 读取来自客户端的数据
        while ((str_len = read(client_sock, message, BUF_SIZE)) != 0) {
            // 向客户端传输数据
            write(client_sock, message, (size_t)str_len);
            message[str_len] = '\0';
            printf("client %d: message %s", i + 1, message);
        }
    }
    // 关闭连接
    close(client_sock);

    printf("echo server\n");
    return 0;
}

