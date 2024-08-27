#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr); // Bind to all available network interfaces

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept error");
            continue;
        }

        printf("Connection accepted from client IP address %s and port %d...\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1) {
            // Receive message from client
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                printf("Client disconnected.\n");
                break;
            }

            printf("Client: %s\n", buffer);

            // Send response back to client
            char response[BUFFER_SIZE];
            printf("Server: ");
            fgets(response, BUFFER_SIZE, stdin);
            response[strcspn(response, "\n")] = 0; // remove trailing newline
            send(client_socket, response, strlen(response), 0);

            // Check for exit command
            if (strcmp(response, "exit") == 0) {
                printf("Disconnecting from client...\n");
                break;
            }
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
