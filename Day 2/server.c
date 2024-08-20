#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char client_name[BUFFER_SIZE];

    // Receive client's name
    memset(client_name, 0, BUFFER_SIZE);
    if (recv(client_socket, client_name, BUFFER_SIZE, 0) < 0) {
        perror("Error receiving client name");
        exit(1);
    }

    printf("%s has connected.\n", client_name);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("%s has disconnected.\n", client_name);
            break;
        }

        printf("%s: %s\n", client_name, buffer);

        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            printf("%s has requested to exit.\n", client_name);
            break;
        }

        // Send acknowledgment back to client
        printf("Server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // remove trailing newline

        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    exit(0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char server_name[BUFFER_SIZE];

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Input server name
    printf("Enter server name: ");
    fgets(server_name, BUFFER_SIZE, stdin);
    server_name[strcspn(server_name, "\n")] = 0; // remove trailing newline

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("%s listening on port %d...\n", server_name, PORT);

    while (1) {
        // Accept incoming connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Fork a new process to handle the client
        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
        } else {
            close(client_socket);
        }
    }

    return 0;
}
