// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>

#define PORT 8110
#define BUFFER_SIZE 1024
void *handle_client(void* arg);
void get_system_info(int client_socket);
void receive_message(int client_socket, const char* client_id, int client_port);
void receive_file(int client_socket);
void execute_command(int client_socket, const char* command);
void search_file(int client_socket, const char* filename); // Hàm tìm kiếm file
void send_file_content(int client_socket, const char* filename);
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
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Get client address info
    getpeername(client_socket, (struct sockaddr*)&client_addr, &client_len);
    char client_id[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_id, sizeof(client_id));
    int client_port = ntohs(client_addr.sin_port);

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break; // Client has disconnected
        }
        
        buffer[bytes_received] = '\0';
        
        if (strcmp(buffer, "exit") == 0) {
            break; // End connection
        }
        
        if (strcmp(buffer, "send_message") == 0) {
            receive_message(client_socket, client_id, client_port);
        } else if (strcmp(buffer, "get_info") == 0) {
            get_system_info(client_socket);
        } else if (strcmp(buffer, "send_file") == 0) {
            receive_file(client_socket);
        } else if (strncmp(buffer, "command:", 8) == 0) {
            execute_command(client_socket, buffer + 8); 
        } else if (strncmp(buffer, "search_file:", 12) == 0) {
           search_file(client_socket, buffer + 12); // Gọi hàm tìm kiếm file
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
    
      
}


void receive_message(int client_socket, const char* client_id, int client_port) {
    char message[BUFFER_SIZE];
    int bytes_received = recv(client_socket, message, sizeof(message) - 1, 0);
    
    if (bytes_received > 0) {
        message[bytes_received] = '\0'; // Null-terminate the string

        // Get current time
        time_t rawtime;
        struct tm *timeinfo;
        char time_buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Print client ID, time, and message
        printf("Client(%s:%d) gửi tin nhắn: %s (%s)\n", client_id, client_port, message, time_buffer);
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

void send_file_content(int client_socket, const char* filename) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Mở file để đọc
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        char error_msg[] = "Lỗi mở file.\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
        return;
    }

    // Đọc và gửi nội dung file theo từng khối
    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    send(client_socket, "END", 3, 0);
}

    void search_file(int client_socket, const char* filename) {
	printf("Server nhận yêu cầu tìm kiếm file:%s\n",filename);
    if (filename == NULL || strlen(filename) == 0) {
        char invalid_msg[] = "Lệnh không hợp lệ: filename trống.\n";
        send(client_socket, invalid_msg, strlen(invalid_msg), 0);
        return;
    }
    
    DIR *dir;
    struct dirent *entry;
    int found = 0;
    // Mở thư mục hiện tại
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    

    // Tìm kiếm file trong thư mục hiện tại
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, filename) == 0) {
            found = 1;
            send_file_content(client_socket, entry->d_name);
            break; // Thoát sau khi xử lý xong filen
        }
    }

    
    closedir(dir);
       // Nếu không tìm thấy, gửi thông báo "file not found"
    if (!found) {
        char not_found_msg[] = "Không tìm thấy file.\n";
        send(client_socket, not_found_msg, strlen(not_found_msg), 0);
    }
    send(client_socket, "END", 3, 0);
}
void handle_sigint(int sig) {
    printf("\nServer shutting down...\n");
    exit(0);
}


