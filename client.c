#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PERMS 0666

#define MAX_BUF_SIZE 32
#define MAX_NAME_SIZE 10
#define MAX_PWD_SIZE 10

// 로그인, 성공시 1, 실패시 0반환
// 입력한 정보를 서버에 저장된 정보화 비교
int signIn();

// 회원가입，성공시 1, 실패시　0반환
// 입력한 정보를 서버에 전달
void signUp();

// 메뉴 선택시 서버에 sign 보냄
// 서버는 sign에 맞는 함수 호출
// 로그인 성공 시 종료됨.
void signChoice();

// 메인메뉴
// 실행할 작업 선택
void menu();

/***********메인함수***********/
int main() {
    printf("광운 숫자야구게임에 오신 걸 환영합니다\n");

    // 로그인 성공 시 종료됨.
    signChoice();

    puts("메인화면 구현");
    printf("\n");

    menu();

    return 0;
}

/***********로그인 UI***********/
void signChoice() {
    int choice;
    int fd[2] = {
        0,
    };
    char sign[1] = {0};

    // 서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_RDONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_WRONLY);

    while (1) {
        printf("\n");
        puts(" ************************* 로그인 선택창");
        puts("로그인 1번，회원가입 2번");
        printf(">>> ");
        scanf("%d", &choice);
        printf("\n");

        // 로그인 : 0
        // 회원가입 : 1
        switch (choice) {
        case 1:
            sign[0] = '0';
            write(fd[1], (char *)sign, sizeof(char));
            if (signIn() == 1) {
                return;
            }
            break;
        case 2:
            sign[0] = '1';
            write(fd[1], (char *)sign, sizeof(char));
            signUp();
            break;
        default:
            break;
        }
        memset(sign, 0x00, sizeof(char));
        lseek(fd[1], (off_t)0, SEEK_SET);
    }
}

/***********로그인***********/
int signIn() {
    int fd[2] = {
        0,
    };
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign;

    // 서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_RDONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_WRONLY);

    puts(" ************************* 로그인");
    printf("ID : ");
    if (scanf("%s", buf) > 0) {
        write(fd[1], &buf, strlen(buf));
        memset(buf, 0x00, MAX_BUF_SIZE);
        lseek(fd[1], (off_t)0, SEEK_SET);
        read(fd[0], &sign, sizeof(char));
    } else {
        printf("아이디가 너무 깁니다.\n");
        return 0;
    }

    switch (sign) {
    case '0':
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        puts("아이디를 생성해주세요");
        printf("\n");
        return 0;
    case '1':
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        puts("일치하는 아이디가 없습니다.");
        printf("\n");
        return 0;
    case '2':
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        printf("PASSWORD : ");
        if (scanf("%s", buf) > 0) {
            write(fd[1], &buf, strlen(buf));
            memset(buf, 0x00, MAX_BUF_SIZE);
            lseek(fd[1], (off_t)0, SEEK_SET);
            read(fd[0], &sign, sizeof(char));
        } else {
            printf("비밀번호가 너무 깁니다.\n");
            return 0;
        }
        break;
    default:
        break;
    }
    // Password에 대한 sign
    switch (sign) {
    case '0':
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        puts("비밀번호가　틀렸습니다.");
        printf("\n");
        return 0;
    case '1':
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        puts(" ************************* 로그인 완료");
        printf("\n");
        return 1;
    default:
        break;
    }
}

/***********회원가입***********/
void signUp() {
    // 중복체크(미완성)
    // 아이디 생성시 기존구조체와 동일아이디 중복체크 해야됨
    int fd[2] = {
        0,
    };
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign;

    // 서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_RDONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_WRONLY);
    puts(" ************************* 회원 가입");
    printf("ID : ");
    if (scanf("%s", buf) > 0) {
        write(fd[1], &buf, strlen(buf));
        memset(buf, 0x00, MAX_BUF_SIZE);
        lseek(fd[1], (off_t)0, SEEK_SET);
    } else {
        printf("아이디가 너무 깁니다.\n");
    }

    // server로부터 입력된 buf에 따른 sign을 받아옴 0 중복, 1 미중복
    read(fd[0], &sign, sizeof(char));

    if (sign == '0') { // 중복이면
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);

        puts("이미 존재하는 ID입니다.");
        return;
    } else if (sign == '1') {
        sign = '\0';
        lseek(fd[0], (off_t)0, SEEK_SET);

        printf("PASSWORD : ");
        if (scanf("%s", buf) > 0) {
            write(fd[1], &buf, strlen(buf));
            memset(buf, 0x00, MAX_BUF_SIZE);
            lseek(fd[1], (off_t)0, SEEK_SET);
        } else {
            printf("비밀번호가 너무 깁니다.\n");
        }

        puts(" ************************* 회원가입 완료");
        printf("\n");
    }
}

void menu() { printf("메인메뉴\n"); }
