#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[4096], *servip;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server_ip> <filename> <port>\n", argv[0]);
        exit(1);
    }

    servip = argv[1];
    portno = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    printf("CLIENT started. Connecting to server %s:%d ...\n", servip, portno);

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(servip);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        close(sockfd);
        exit(1);
    }

    // Send requested filename to the server
    write(sockfd, argv[2], strlen(argv[2]) + 1);

    printf("\n--- File Contents ---\n\n");

    // Read file data from server and print to terminal
    while ((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';  // Null-terminate the received data
        printf("%s", buffer);
        fflush(stdout);    // Ensure immediate display
    }

    if (n < 0)
        perror("Error reading from socket");
    else
        printf("\n\n--- End of File ---\n");

    close(sockfd);
    return 0;
}
