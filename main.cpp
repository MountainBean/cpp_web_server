#include <cstdlib>
#include <format>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <linux/in.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//! begins listening on my_port
int start_server(unsigned short my_port) {
    struct sockaddr_in my_sockaddr;
    memset(&my_sockaddr, 0, sizeof(my_sockaddr));           // Zero out the struct memory
    my_sockaddr.sin_family = AF_INET;
    my_sockaddr.sin_port = htons(my_port);
    my_sockaddr.sin_addr = (struct in_addr) INADDR_ANY;     // 0.0.0.0

    // syscall socket()
    int sock_fd {socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    // syscall bind()
    if (bind(sock_fd, (struct sockaddr *) &my_sockaddr, sizeof(my_sockaddr)) != 0) {
        std::cout << "bind failed" << std::endl;
        return -1;
    };
    // syscall listen()
    if (listen(sock_fd, 0) != 0) {
        std::cout << "listen failed" << std::endl;
        return -1;
    }

    return sock_fd;
};

// child process handle function
int handle_request(int clientfd) {
    char buf[1024];
    char req[5];

    memset(&buf, 0, sizeof(buf));
    memset(&req, 0, sizeof(req));

    long read_length {read(clientfd, &buf, sizeof(buf))};
    memcpy(&req, &buf, 4);      // copy first 4 bytes into req
    if ((std::string)req == "GET ") {

        write(4, "HTTP/1.1 200 OK\r\n\r\n", 19);
    }
    close(clientfd);
    return 0;
}

int main(int argc, char* argv[]) {
    unsigned short my_port {};
    if (argc < 2) {
        std::cout << "no arg given. using port 80\n";
        // assign default port 80
        my_port = 80;
    } else {
        long my_long_port {strtol(argv[1], NULL, 10)};
        if (my_long_port < 1024 || my_long_port > 60999) {
            std::cout << "Given port must be between 1024 and 60999\n";
            return -1;
        }
        my_port = (unsigned short) my_long_port;
    }
    std::cout << std::format("Starting server on port {}\n", my_port);
    int sock_fd = {start_server(my_port)};
    if (sock_fd < 0) {
        return -1;
    }

    while (true) {
        int client_fd {accept(sock_fd, NULL, NULL)};
        int fork_id {fork()};

        if (fork_id != 0) {
            close(client_fd);
        } else {
            close(sock_fd);
            handle_request(client_fd);
            return 0;
        }
    }
}

