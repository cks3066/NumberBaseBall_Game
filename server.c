#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PERMS 0666
#define MAX_BUF_SIZE 32
#define MAX_NAME_SIZE 32
#define MAX_PWD_SIZE 32
#define MAX_USER_CAPACITY 32

typedef struct _User {
    char name[MAX_NAME_SIZE + 1];
    char pwd[MAX_PWD_SIZE + 1];
    int win;
    int lose;
} User;

/***********정의할 함수***********/
void signUp();
int signIn();
void signChoice();
void menu();
// void game();
// void help();
// void checkRank();
// void end();

/***********전역변수***********/
size_t num = 0;                    // 생성된 User 구조체 갯수
User users[MAX_USER_CAPACITY + 1]; // User 구조체
int loginNum; // 로그인에 이용된 구조체 배열 key

/***********메인 함수***********/
int main() {
    int test;

    mkfifo("serverWR", PERMS); //서버 입력용 파이프라인(유저 출력)
    mkfifo("clientWR", PERMS); //유저 입력용 파이프라인(서버 출력)

    while (1) {
        signChoice(); // 로그인 성공 시 종료됨.

        puts("메인화면 구현");
        printf("\n");

        menu();
    }
    return 0;
}

/***********회원가입***********/
void signUp() {
    int fd[2] = {
        0,
    };
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign[1] = {0};
    int overlap = 0;

    puts("회원가입");
    //서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_WRONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_RDONLY);

    printf("입력받은 ID는 : ");
    read(fd[1], (char *)buf, MAX_BUF_SIZE);
    printf("%s\n", buf);

    ////////////////////////////////////////////
    //기존　users.name 과 중복체크
    ///////////////////////////////////////////
    if (num != 0) { // 등록된 유저 정보가 있다면
        for (int i = 0; i < num; i++) {
            if (strcmp(users[i].name, buf) == 0) //중복 존재시
                overlap = 1;
        }
    }

    // 중복 존재 시 0   sign
    // 중복 없을 시 1   sign
    if (overlap == 1) {
        // 유저에게 overlap sign 전달
        sign[0] = '0';
        write(fd[0], (char *)sign, sizeof(char));

        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));
        lseek(fd[0], (off_t)0, SEEK_SET);
        lseek(fd[1], (off_t)0, SEEK_SET);

        puts("회원가입 실패");
        printf("\n");
    } else { // num이 0인 경우도 overlap이 0이기 때문에 포함
        // 유저에게 non-overlap sign 전달
        sign[0] = '1';
        write(fd[0], (char *)sign, sizeof(char));

        strcpy(users[num].name, buf);
        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));
        lseek(fd[0], (off_t)0, SEEK_SET);
        lseek(fd[1], (off_t)0, SEEK_SET);

        // Password
        printf("입력받은 PASSWORD는 : ");
        read(fd[1], (char *)buf, MAX_BUF_SIZE);
        printf("%s\n", buf);
        strcpy(users[num].pwd, buf);

        memset(buf, 0x00, MAX_BUF_SIZE);
        lseek(fd[1], (off_t)0, SEEK_SET);

        puts("회원가입 성공");
        printf("\n");

        num++;
    }
}

/***********로그인***********/
// 로그인 실패 : 0 반환
// 로그인 성공 : 1 반환
int signIn() {
    // num 만큼　구조체를　돌면서　name 체크 - saveNum에 저장
    // name　발견시　해당　num의　pwd 체크
    int fd[2] = {
        0,
    };
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign[1] = {0};
    int saveNum = -1;

    //서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_WRONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_RDONLY);

    puts("로그인");
    printf("입력받은 ID는 : ");
    read(fd[1], (char *)buf, MAX_BUF_SIZE);
    printf("%s\n", buf);
    // 아이디가 아예　없는 경우 : 0
    // 해당하는 아이디가 없는 경우 : 1
    // 해당하는 아이디가 있는 경우 : 2
    if (num == 0) {
        sign[0] = '0';
        write(fd[0], (char *)sign, sizeof(char));
        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));
        lseek(fd[0], (off_t)0, SEEK_SET);
        lseek(fd[1], (off_t)0, SEEK_SET);
        return 0;
    } else {
        for (int i = 0; i < num; i++) {
            if (strcmp(buf, users[i].name) == 0) {
                sign[0] = '2';
                write(fd[0], (char *)sign, sizeof(char));
                memset(buf, 0x00, MAX_BUF_SIZE);
                memset(sign, 0x00, sizeof(char));
                lseek(fd[0], (off_t)0, SEEK_SET);
                lseek(fd[1], (off_t)0, SEEK_SET);
                saveNum = i;
                break;
            }
        }
        if (saveNum == -1) {
            printf("\n");
            sign[0] = '1';
            write(fd[0], (char *)sign, sizeof(char));
            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));
            lseek(fd[0], (off_t)0, SEEK_SET);
            lseek(fd[1], (off_t)0, SEEK_SET);
            return 0;
        }
    }
    // 패스워드 오류 : 0
    // 로그인 성공 : 1
    if (saveNum != -1) {
        printf("입력받은 PASSWORD는 : ");
        read(fd[1], (char *)buf, MAX_BUF_SIZE);
        printf("%s\n", buf);
        if (strcmp(buf, users[saveNum].pwd) == 0) {
            puts("로그인 성공");
            printf("\n");
            loginNum = saveNum;
            sign[0] = '1';
            write(fd[0], (char *)sign, sizeof(char));
            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));
            lseek(fd[0], (off_t)0, SEEK_SET);
            lseek(fd[1], (off_t)0, SEEK_SET);
            return 1;
        } else {
            puts("패스워드 오류");
            printf("\n");
            sign[0] = '0';
            write(fd[0], (char *)sign, sizeof(char));
            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));
            lseek(fd[0], (off_t)0, SEEK_SET);
            lseek(fd[1], (off_t)0, SEEK_SET);
            return 0;
        }
    }
}

/***********로그인 ING***********/
void signChoice() {
    int fd[2] = {
        0,
    };
    char sign;

    //서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_WRONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_RDONLY);

    while (1) {
        puts("대기중...");

        read(fd[1], &sign, sizeof(char));
        switch (sign) {
        case '0':
            if (signIn() == 1) {
                return;
            }
            break;
        case '1':
            signUp();
            break;
        default:
            break;
        }
    }
    sign = '\0';
    lseek(fd[1], (off_t)0, SEEK_SET);

    close(fd[0]);
    close(fd[1]);
}

void menu() { printf("main menu\n"); }
