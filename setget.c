#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Thông tin cá nhân
#define NAME "John Doe"
#define ADDRESS "123 Main St, Springfield, USA"
#define EMAIL "john.doe@example.com"
#define PHONE "123-456-7890"
#define AGE 30

// Định nghĩa cấu trúc cho lớp
typedef struct {
    char name[50];
    int credits;
} Course;

// Định nghĩa cấu trúc cho sinh viên
typedef struct {
    char name[50];
    int age;
    Course courses[10]; // Giả sử mỗi sinh viên có tối đa 10 môn học
    int courseCount;
} Student;

void greetUser(const char *name) {
    printf("Hello, %s! Welcome to the program.\n", name);
}

// Hàm để hiển thị thông tin sinh viên
void printStudentInfo(Student *student) {
    printf("Student Name: %s\n", student->name);
    printf("Age: %d\n", student->age);
    printf("Courses enrolled:\n");
    for (int i = 0; i < student->courseCount; i++) {
        printf("  - %s (%d credits)\n", student->courses[i].name, student->courses[i].credits);
    }
}

int main(int argc, char **argv) {
    // Hiển thị tên chương trình
    printf("My name is 'setget'.\n");

    // Hiển thị thông tin cá nhân
    printf("Personal Information:\n");
    printf("Name: %s\n", NAME);
    printf("Address: %s\n", ADDRESS);
    printf("Email: %s\n", EMAIL);
    printf("Phone: %s\n", PHONE);
    printf("Age: %d\n\n", AGE);

    // Kiểm tra xem có đối số tên nào được cung cấp không
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <your_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Chào người dùng sử dụng tên được cung cấp
    greetUser(argv[1]);

    // Khởi tạo thông tin sinh viên
    Student student;
    strcpy(student.name, argv[1]);
    student.age = AGE;
    student.courseCount = 0; // Khởi tạo số môn học

    // Thêm một số môn học vào sinh viên
    strcpy(student.courses[0].name, "Mathematics");
    student.courses[0].credits = 3;
    student.courseCount++;

    strcpy(student.courses[1].name, "Computer Science");
    student.courses[1].credits = 4;
    student.courseCount++;

    // Hiển thị thông tin sinh viên
    printStudentInfo(&student);

    return EXIT_SUCCESS;
}
