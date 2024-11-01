//server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handle_client(void* arg);
void get_system_info(int client_socket);
void receive_message(int client_socket);
void receive_file(int client_socket);
void execute_command(int client_socket, const char* command);
void handle_sigint(int sig);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    // Tạo socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server đang lắng nghe trên cổng %d\n", PORT);
    signal(SIGINT, handle_sigint);

    while (1) {
        // Chấp nhận kết nối từ client
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) == -1) {
            perror("accept");
            continue;
        }

        printf("Client kết nối: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Tạo một thread cho client
        pthread_create(&tid, NULL, handle_client, (void*)&client_socket);
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break; // Client đã ngắt kết nối
        }
        
        buffer[bytes_received] = '\0';
        
        // Kiểm tra xem buffer có chứa lệnh 'exit' không
        if (strcmp(buffer, "exit") == 0) {
            break; // Kết thúc kết nối
        }
        
        // Kiểm tra xem buffer có phải là một tin nhắn không
        if (strcmp(buffer, "send_message") == 0) {
            receive_message(client_socket);
        } else if (strcmp(buffer, "get_info") == 0) {
            get_system_info(client_socket);
        } else if (strcmp(buffer, "send_file") == 0) {
            receive_file(client_socket);
        }
        else if (strncmp(buffer, "command:", 8) == 0) {
            execute_command(client_socket, buffer + 8); 
        } else {
            printf("Lệnh không hợp lệ: %s\n", buffer);
        }
    }

    close(client_socket);
    printf("Kết nối client đã bị ngắt.\n");
    return NULL;
}

void execute_command(int client_socket, const char* command) {
    FILE *fp;
    char result[BUFFER_SIZE];
    
    // Mở lệnh để thực hiện
    if ((fp = popen(command, "r")) == NULL) {
        perror("popen");
        return;
    }

    // Đọc kết quả
    while (fgets(result, sizeof(result), fp) != NULL) {
        send(client_socket, result, strlen(result), 0);
    }
    
    // Gửi chuỗi "END" để báo hiệu đã kết thúc
    send(client_socket, "END", 3, 0);

    pclose(fp);
}

void receive_file(int client_socket) {
    char filename[BUFFER_SIZE];
    int bytes_received;

    // Nhận tên file từ client
    bytes_received = recv(client_socket, filename, sizeof(filename) - 1, 0);
    
    filename[bytes_received] = '\0'; 
    printf("Nhận file: %s\n", filename);
        
    // Mở file để ghi
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("fopen");
        return;
    }
        
    char buffer[BUFFER_SIZE];
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    fwrite(buffer, sizeof(char), bytes_received, file);
    fclose(file);  
      
    printf("%s\n", buffer);
}

void receive_message(int client_socket) {
    char message[BUFFER_SIZE];
    int bytes_received = recv(client_socket, message, sizeof(message) - 1, 0);
    
    if (bytes_received > 0) {
        message[bytes_received] = '\0'; // Kết thúc chuỗi
        printf("Nhận tin nhắn: %s\n", message);
    } else {
        printf("Lỗi khi nhận tin nhắn từ client.\n");
    }
}

void get_system_info(int client_socket) {
    char info[BUFFER_SIZE];
    FILE* fp = popen("uname -a; free -m; lscpu", "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }

    while (fgets(info, sizeof(info), fp) != NULL) {
        send(client_socket, info, strlen(info), 0);
    }
    
    // Gửi chuỗi "END" để báo hiệu đã kết thúc
    send(client_socket, "END", 3, 0);
    
    pclose(fp);
}

void handle_sigint(int sig) {
    printf("\nServer shutting down...\n");
    exit(0);
}