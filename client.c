#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8110
#define BUFFER_SIZE 1024

void show_client_menu(int socket);
void get_info(int socket);
void send_message(int socket);
void send_file(int socket);
void download_file(int socket, const char* filename);
void use_terminal(int socket);
void search_file(int socket); // Add function prototype for searching files

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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server successfully!\n");

    // Display menu for client
    show_client_menu(client_socket);

    close(client_socket);
    return 0;
}

void show_client_menu(int socket) {
    int choice;

    while (1) {
        printf("\n--- Client Menu ---\n");
        printf("1. View server info\n");
        printf("2. Send message to server\n");
        printf("3. Send file to server\n");
        printf("4. Use server terminal\n");
        printf("5. Search for a file on server\n"); // Added search option
        printf("6. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Read the newline character left behind

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
                search_file(socket); // Call the search file function
                break;
            case 6:
                send(socket, "exit", 4, 0);
                printf("Disconnecting from server.\n");
                return;
            default:
                printf("Invalid choice.\n");
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
    
    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0; // Remove newline character
    
    send(socket, message, strlen(message), 0);
    
    printf("Message sent.\n");
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

    printf("Enter command to execute on server (type 'exit' to return to menu):\n");
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character

        if (strcmp(command, "exit") == 0) {
            break;
        }

        char formatted_message[BUFFER_SIZE];
        snprintf(formatted_message, sizeof(formatted_message), "command:%s", command);

        send(socket, formatted_message, strlen(formatted_message), 0);
        
        char buffer[BUFFER_SIZE];
        int bytes_received;

        while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_received] = '\0'; // Ensure null-terminated string
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

    if (strlen(filename) >= sizeof(buffer) - 13) {
        printf("Tên file quá dài.\n");
        return;
    }

    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "search_file:%s", filename);
    
    if (send(socket, command, strlen(command), 0) < 0) {
        perror("send failed");
        return;
    }

    printf("Nội dung file tìm kiếm từ server:\n");

    // Vòng lặp nhận và hiển thị nội dung file
    while ((bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        // Kiểm tra nếu file không tồn tại trên server
        if (strstr(buffer, "Không tìm thấy file.") != NULL) {
            printf("File không tồn tại trên server.\n");
            return;
        }

        file_found = 1;

        // Kiểm tra tín hiệu "END" trong buffer để đánh dấu kết thúc nội dung
        if (strstr(buffer, "END") != NULL) {
            char* end_pos = strstr(buffer, "END");
            *end_pos = '\0';  // Cắt buffer tại "END"
            printf("%s", buffer);
            break;  // Thoát vòng lặp khi đã nhận đủ nội dung
        }

        // In nội dung buffer nếu file tồn tại
        printf("%s", buffer);
        
        }

    // Nếu file đã được tìm thấy, yêu cầu người dùng tải về
    if (file_found) {
        // Kiểm tra xem file đã tồn tại trong client chưa

        printf("\nNhấn 'd' để tải file về client hoặc bất kỳ phím nào khác để thoát: ");
        char choice = getchar();
        getchar();  // Bắt newline sau khi nhập lựa chọn

        if (choice == 'd' || choice == 'D') {
    // Kiểm tra xem file đã tồn tại hay chưa
    FILE *check_file = fopen(filename, "r");
    if (check_file) {
        printf("File đã tồn tại trên client. Không thể tải về.\n");
        fclose(check_file);
        return; // Ngừng thực hiện nếu file đã tồn tại
    }

    // Mở file để lưu nội dung, ghi đè nếu file đã tồn tại
    FILE *file = fopen(filename, "w"); // Mở file với chế độ ghi văn bản
    if (!file) {
        perror("Không thể tạo file trên client");
        return;
    }

    // Ghi toàn bộ nội dung đã nhận vào file
    fputs(buffer, file); // Ghi buffer vào file dưới dạng văn bản

    printf("File đã tải thành công: %s\n", filename);
    fclose(file);
}
    }
}





