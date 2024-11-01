//client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3306
#define BUFFER_SIZE 1024

void show_client_menu(int socket);
void get_info(int socket);
void send_message(int socket);
void use_terminal(int socket);
void send_file(int socket);

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Tạo socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Địa chỉ IP của server

    // Kết nối đến server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Kết nối đến server thành công!\n");

    // Hiển thị menu cho client
    show_client_menu(client_socket);

    close(client_socket);
    return 0;
}

void show_client_menu(int socket) {
    int choice;

    while (1) {
        printf("\n--- Menu Client ---\n");
        printf("1. Xem thông tin server\n");
        printf("2. Gửi tin nhắn đến server\n");
        printf("3. Gửi file đến server\n");
        printf("4. Sử dụng terminal của server\n");
        printf("5. Thoát\n");
        printf("Chọn chức năng: ");
        scanf("%d", &choice);
        getchar(); // Đọc ký tự newline còn lại

        switch (choice) {
            case 1:
                get_info(socket);
                break;
            case 2:
                send_message(socket);
                break;
            case 3:
                send_file(socket);
                break;
            case 4:
                use_terminal(socket);
                break;
            case 5:
                send(socket, "exit", 4, 0);
                printf("Hủy kết nối đến server.\n");
                return;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }
    }
}

void get_info(int socket) {
    char buffer[BUFFER_SIZE];
    send(socket, "get_info", 8, 0);

    int bytes_received;
    // Nhận kết quả từ server
    while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strcmp(buffer, "END") == 0) {
            break;
        }
        printf("%s", buffer);
    }
}


void send_message(int socket) {
    char message[BUFFER_SIZE];
    send(socket, "send_message", 12, 0);
    
    printf("Nhập tin nhắn: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0; // Xóa ký tự xuối dòng
    
    // Gửi tin nhắn tới server
    send(socket, message, strlen(message), 0);
    
    printf("Đã gửi tin nhắn.\n"); 
    return;
}


void send_file(int socket) {
    char filename[BUFFER_SIZE];
    send(socket, "send_file", strlen("send_file"), 0);
    
    printf("Nhập tên file để gửi: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // Xóa ký tự newline

    // Mở file ở chế độ đọc nhị phân
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    // Gửi tên file đến server
    send(socket, filename, strlen(filename), 0);
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    // Đọc và gửi từng phần nội dung của file
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }

    fclose(file);
    
    printf("Đã gửi file: %s\n", filename);
}

void use_terminal(int socket) {
    char command[BUFFER_SIZE - 9];

    printf("Nhập lệnh để thực hiện trên server (gõ 'exit' để trở lại menu):\n");
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Xóa ký tự xuối dòng

        if (strcmp(command, "exit") == 0) {
            break;
        }

        char formatted_message[BUFFER_SIZE];
        snprintf(formatted_message, sizeof(formatted_message), "command:%s", command);

        send(socket, formatted_message, strlen(formatted_message), 0);
        
        char buffer[BUFFER_SIZE];
        int bytes_received;

        // Nhận kết quả của lệnh
        while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0'; // Đảm bảo kết thúc chuỗi
            // Nếu nhận kết thúc dòng
            if (strcmp(buffer, "END") == 0) {
                break;
            }
            printf("%s", buffer);
        }
        printf("\n"); 
    }
}
