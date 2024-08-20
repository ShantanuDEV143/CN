#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char client_name[BUFFER_SIZE];

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Input client name
    printf("Enter your name: ");
    fgets(client_name, BUFFER_SIZE, stdin);
    client_name[strcspn(client_name, "\n")] = 0; // remove trailing newline

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send client name to server
    send(client_socket, client_name, strlen(client_name), 0);

    printf("Connected to server. Type your messages below (type 'exit' to quit):\n");

    while (1) {
        // Read message from user
        memset(buffer, 0, BUFFER_SIZE);
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // remove trailing newline

        // Send message to server
        send(client_socket, buffer, strlen(buffer), 0);

        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            printf("Disconnecting from server...\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);

        // Receive response from server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server: %s\n", buffer);
    }

    close(client_socket);
    return 0;
}
