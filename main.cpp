#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <arpa/inet.h>
#include <linux/in.h>
#include <netinet/in.h>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//! begins listening on my_port
int start_server(std::uint16_t my_port) {
    struct sockaddr_in my_sockaddr {
        .sin_family = AF_INET,      // Clang gives a false warning here if you
        .sin_port = htons(my_port), // use brace-initialisation.
        .sin_addr{static_cast<struct in_addr>(INADDR_ANY)},
        .sin_zero{}
    };
    my_sockaddr.sin_family = AF_INET;
    my_sockaddr.sin_port = htons(my_port);
    my_sockaddr.sin_addr = static_cast<struct in_addr>(INADDR_ANY);     // 0.0.0.0

    // syscall socket()
    int sock_fd {socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    // syscall bind()
    // WARNING: Move away from C-style cast here. static cast won't work
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
    constexpr std::string_view getReq { "GET " };

    // TODO: remove the [[maybe_unused]] when we eventually check our read_length
    [[maybe_unused]] long read_length {read(clientfd, &buf, sizeof(buf))};
    std::string_view req { buf };
    if (req.substr(0, 4) == getReq) {

        write(4, "HTTP/1.1 200 OK\r\n\r\n", 19);
    }
    close(clientfd);
    return 0;
}

int main(int argc, char* argv[]) {
    std::uint16_t my_port {};
    if (argc < 2) {
        std::cout << "Please enter a port number ( >1024 ): \n";
        std::cin >> my_port;
    } else {
        long my_long_port {strtol(argv[1], NULL, 10)};
        if (my_long_port < 1024 || my_long_port > 60999) {
            std::cout << "Given port must be between 1024 and 60999\n";
            return -1;
        }
        my_port = static_cast<std::uint16_t>(my_long_port);
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

