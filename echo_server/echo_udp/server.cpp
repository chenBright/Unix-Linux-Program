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
