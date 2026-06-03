#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int fd, sockfd, newsockfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[4096];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Error on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    printf("SERVER waiting for CLIENT connection on port %d...\n", portno);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("Error on accept");

        bzero(buffer, sizeof(buffer));
        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("Error reading filename from client");
            close(newsockfd);
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline if sent
        printf("Client requested file: %s\n", buffer);

        fd = open(buffer, O_RDONLY);
        if (fd < 0) {
            perror("File open error");
            close(newsockfd);
            continue;
        }

        printf("Transferring file: %s\n", buffer);
        while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
            write(newsockfd, buffer, n);
        }

        printf("Transfer Completed.\n");
        close(fd);
        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
