#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <linux/in.h>
#include <netinet/in.h>
#include <sys/socket.h>


int start_server(int my_port) {
    struct in_addr all_addr;
    struct sockaddr_in my_sockaddr;

    inet_aton("0.0.0.0", &all_addr);
    memset(&my_sockaddr, 0, sizeof(my_sockaddr));
    my_sockaddr.sin_family = AF_INET;
    my_sockaddr.sin_port = htons(8080);
    my_sockaddr.sin_addr = all_addr;

    // syscall socket()
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    // syscall bind()
    if (bind(sock_fd, (struct sockaddr *) &my_sockaddr, sizeof(my_sockaddr)) != 0) {
        std::cout << "bind failed" << std::endl;
    };
    // syscall listen()
    if (listen(sock_fd, 0) != 0) {
        std::cout << "listen failed" << std::endl;
    }

    return 0;
};

int main(void) {
    std::cout << "Hello, world!" << std::endl;
}

