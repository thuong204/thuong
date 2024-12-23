// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void show_client_menu(int socket);
void get_info(int socket);
void send_message(int socket);
void send_file(int socket);
void download_file(int socket, const char* filename);
void use_terminal(int socket);
void search_file(int socket);
void add_user(int socket);
void delete_user(int socket);
void list_users(int socket);

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //192.168.1.3

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Kết nối đến server thành công!\n");

    // Display menu for client
    show_client_menu(client_socket);

    close(client_socket);
    return 0;
}

void show_client_menu(int socket) {
    int choice;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\n--- Menu Client ---\n");
        printf("1. Xem thông tin server\n");
        printf("2. Gửi tin nhắn đến server\n");
        printf("3. Gửi file đến server\n");
        printf("4. Sử dụng terminal của server\n");
        printf("5. Tìm kiếm và tải file từ server\n");
        printf("6. Thêm người dùng\n");
        printf("7. Xóa người dùng\n");
        printf("8. Xem danh sách người dùng\n");
        printf("9. Thoát\n");
        printf("Chọn chức năng: ");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Chuyển đổi chuỗi thành số nguyên, kiểm tra lỗi
            if (sscanf(buffer, "%d", &choice) != 1) {
                printf("Lựa chọn không hợp lệ.\n");
                continue;
            }
        }

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
                search_file(socket);
                break;
            case 6:
                add_user(socket);
                break;
            case 7:
                delete_user(socket);
                break;
            case 8:
                list_users(socket);
                break;
            case 9:
                send(socket, "exit", 4, 0);
                printf("Hủy kết nối với server.\n");
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
    message[strcspn(message, "\n")] = 0;

    send(socket, message, strlen(message), 0);

    printf("Tin nhắn đã gửi.\n");
    return;
}

void send_file(int socket) {
    char filename[BUFFER_SIZE];

    send(socket, "send_file", strlen("send_file"), 0);

    printf("Nhập tên file để gửi: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        char error_msg[] = "file_not_found";
        send(socket, error_msg, strlen(error_msg), 0);
        return;
    }

    send(socket, filename, strlen(filename), 0);

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }

    fclose(file);

    printf("Đã gửi file: %s\n", filename);
}


void use_terminal(int socket) {
    char command[BUFFER_SIZE - 9];

    printf("Nhập lệnh để thực hiện trên server (gõ ‘exit’ để trở lại menu):\n");
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            break;
        }

        char formatted_message[BUFFER_SIZE];
        snprintf(formatted_message, sizeof(formatted_message), "command:%s", command);

        send(socket, formatted_message, strlen(formatted_message), 0);

        char buffer[BUFFER_SIZE];
        int bytes_received;

        while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            if (strcmp(buffer, "END") == 0) {
                break;
            }
            printf("%s", buffer);
        }
        printf("\n");
    }
}

void search_file(int socket) {
    char filename[BUFFER_SIZE - 12];
    char buffer[BUFFER_SIZE];
    int bytes_received;
    int file_found = 0;

    printf("Nhập tên file để tìm kiếm: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "search_file:%s", filename);

    if (send(socket, command, strlen(command), 0) < 0) {
        perror("send failed");
        return;
    }

    printf("\nNội dung file tìm kiếm từ server:\n");

    char file_content[BUFFER_SIZE * 10] = "";

    while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        if (strstr(buffer, "file not found") != NULL) {
            printf("File không tồn tại trên server.\n");
            return;
        }

        file_found = 1;

        if (strstr(buffer, "END") != NULL) {
            char* end_pos = strstr(buffer, "END");
            *end_pos = '\0';
            printf("%s", buffer);
            strncat(file_content, buffer, sizeof(file_content) - strlen(file_content) - 1);
            break;
        }

        printf("%s", buffer);
        strncat(file_content, buffer, sizeof(file_content) - strlen(file_content) - 1);
    }

    if (file_found) {
        printf("\nNhấn 'd' để tải file về client hoặc bất kỳ phím nào khác để thoát: ");
        char choice = getchar();
        getchar();

        if (choice == 'd' || choice == 'D') {
            FILE *check_file = fopen(filename, "r");
            if (check_file) {
                printf("File đã tồn tại trên client. Không thể tải về.\n");
                fclose(check_file);
                return;
            }

            FILE *file = fopen(filename, "w");
            if (!file) {
                perror("Không thể tạo file trên client");
                return;
            }

            fputs(file_content, file);
            printf("File đã tải thành công: %s\n", filename);
            fclose(file);
        }
    }
}

void add_user(int socket) {
    char username[BUFFER_SIZE - 8];
    char buffer[BUFFER_SIZE];
    int bytes_received;

    printf("Nhập tên người dùng để thêm: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "add_user%s", username);
    
    if (send(socket, command, strlen(command), 0) < 0) {
        perror("send failed");
        return;
    }
    
    bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
}

void delete_user(int socket) {
    char username[BUFFER_SIZE - 11];
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    printf("Nhập tên người dùng để xóa: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "delete_user%s", username);
    
     if (send(socket, command, strlen(command), 0) < 0) {
        perror("send failed");
        return;
    }

    bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
}

void list_users(int socket) {
    char buffer[BUFFER_SIZE];
    
    send(socket, "list_users", 10, 0);

    printf("Danh sách người dùng từ server:\n");

    int bytes_received;
    while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; 
        if (strcmp(buffer, "END") == 0) {
            break;
        }
        printf("%s", buffer);
    }
}
