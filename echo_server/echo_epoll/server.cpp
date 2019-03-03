#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>

const int BUF_SIZE = 100;
const int EPOLL_SIZE = 50;

void error_handling(const char *buf);

int main(int argc, char *argv[]) {
    int server_sock, client_sock;
    sockaddr_in server_addr, client_addr;

    socklen_t addr_size;
    ssize_t str_len;
    int i;
    char buf[BUF_SIZE];

    epoll_event *ep_events;
    epoll_event event;
    int epfd, event_cnt;

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


    epfd = epoll_create(EPOLL_SIZE);
    ep_events = (epoll_event*)malloc(sizeof(epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = server_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);

    while (1) {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            puts("epoll_wait() error");
            break;
        }

        for (int i = 0; i < event_cnt; ++i) {
            if (ep_events[i].data.fd == server_sock) {
                addr_size = sizeof(client_addr);
                client_sock = accept(server_sock, (sockaddr*)&client_addr, &addr_size);
                event.events = EPOLLIN;
                event.data.fd = client_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                printf("connect client: %d\n", client_sock);
            } else {
                str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if (str_len == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("closed client: %d\n", ep_events[i].data.fd);
                } else {
                    write(ep_events[i].data.fd, buf,str_len);
                }
            }
        }
    }
    close(server_sock);
    close(epfd);

    return 0;
}
