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

void game();
void record();
void ranking();
void help();

int fd[2] = {
    0,
};

/***********메인함수***********/
int main() {

    // 서버 입력용 파이프라인(유저 출력)
    fd[0] = open("serverWR", O_RDONLY);
    //유저 입력용 파이프라인(서버 출력)
    fd[1] = open("clientWR", O_WRONLY);

    while (1) {
        system("clear");
        printf("숫자야구게임에 오신 걸 환영합니다\n");

        // 로그인 성공 시 종료됨.
        signChoice();
        printf("\n");

        while (1) {
            int choice;
            int whilebrk = 0;
            char sign[1] = {0};
            system("clear");
            puts(" ************************* 메인화면 "
                 "************************* ");
            puts("게임시작 1번，전적확인 2번, 랭킹확인 3번, 도움말 4번, "
                 "로그아웃 "
                 "5번");
            printf(">>> ");
            scanf("%d", &choice);

            switch (choice) {
            case 1: // game
                system("clear");
                sign[0] = '1';
                lseek(fd[1], (off_t)0, SEEK_SET);
                write(fd[1], (char *)sign, sizeof(char));
                game();
                break;
            case 2: // record
                system("clear");
                puts("record");
                printf("\n");
                sign[0] = '2';
                lseek(fd[1], (off_t)0, SEEK_SET);
                write(fd[1], (char *)sign, sizeof(char));
                record();
                sleep(2);
                break;
            case 3: // ranking
                system("clear");
                puts("ranking");
                printf("\n");
                sign[0] = '3';
                lseek(fd[1], (off_t)0, SEEK_SET);
                write(fd[1], (char *)sign, sizeof(char));
                ranking();
                sleep(2);
                break;
            case 4: // help
                system("clear");
                puts("help");
                printf("\n");
                sign[0] = '4';
                lseek(fd[1], (off_t)0, SEEK_SET);
                write(fd[1], (char *)sign, sizeof(char));
                help();
                sleep(2);
                break;
            case 5: // logout
                system("clear");
                puts("logout");
                printf("\n");
                sign[0] = '5';
                lseek(fd[1], (off_t)0, SEEK_SET);
                write(fd[1], (char *)sign, sizeof(char));
                whilebrk = 1;
                break;
            default:
                break;
            }
            if (whilebrk == 1)
                break;
        }
    }
    close(fd[0]);
    close(fd[1]);
    return 0;
}

/***********로그인 UI***********/
void signChoice() {
    int choice;
    char sign[1] = {0};

    while (1) {
        system("clear");
        puts(" ************************* 로그인 선택창 "
             "************************* ");
        puts("로그인 1번，회원가입 2번");
        printf(">>> ");
        scanf("%d", &choice);
        printf("\n");

        // 로그인 : 0
        // 회원가입 : 1
        switch (choice) {
        case 1:
            system("clear");
            sign[0] = '0';
            lseek(fd[1], (off_t)0, SEEK_SET);
            write(fd[1], (char *)sign, sizeof(char));
            if (signIn() == 1) {
                return;
            }
            break;
        case 2:
            system("clear");
            sign[0] = '1';
            lseek(fd[1], (off_t)0, SEEK_SET);
            write(fd[1], (char *)sign, sizeof(char));
            signUp();
            break;
        default:
            break;
        }
        memset(sign, 0x00, sizeof(char));
    }
}

/***********로그인***********/
int signIn() {
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign;

    puts(" ************************* 로그인 ************************* ");
    printf("ID : ");
    if (scanf("%s", buf) > 0) {
        lseek(fd[1], (off_t)0, SEEK_SET);
        write(fd[1], &buf, strlen(buf));
        memset(buf, 0x00, MAX_BUF_SIZE);
        lseek(fd[0], (off_t)0, SEEK_SET);
        read(fd[0], &sign, sizeof(char));
    } else {
        memset(buf, 0x00, MAX_BUF_SIZE);
        printf("아이디가 너무 깁니다.\n");
        sleep(2);
        return 0;
    }

    switch (sign) {
    case '0':
        sign = '\0';
        memset(buf, 0x00, MAX_BUF_SIZE);
        puts("아이디를 생성해주세요");
        printf("\n");
        sleep(2);
        return 0;
    case '1':
        sign = '\0';
        memset(buf, 0x00, MAX_BUF_SIZE);
        puts("일치하는 아이디가 없습니다.");
        printf("\n");
        sleep(2);
        return 0;
    case '2':
        sign = '\0';
        printf("PASSWORD : ");
        if (scanf("%s", buf) > 0) {
            lseek(fd[1], (off_t)0, SEEK_SET);
            write(fd[1], &buf, strlen(buf));
            memset(buf, 0x00, MAX_BUF_SIZE);
            lseek(fd[0], (off_t)0, SEEK_SET);
            read(fd[0], &sign, sizeof(char));
        } else {
            memset(buf, 0x00, MAX_BUF_SIZE);
            printf("비밀번호가 너무 깁니다.\n");
            sleep(2);
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
        memset(buf, 0x00, MAX_BUF_SIZE);
        puts("비밀번호가　틀렸습니다.");
        printf("\n");
        sleep(2);
        return 0;
    case '1':
        sign = '\0';
        memset(buf, 0x00, MAX_BUF_SIZE);
        puts(" ************************* 로그인 완료 "
             "************************* ");
        printf("\n");
        sleep(2);
        return 1;
    default:
        break;
    }
}

/***********회원가입***********/
void signUp() {
    // 중복체크(미완성)
    // 아이디 생성시 기존구조체와 동일아이디 중복체크 해야됨
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign;

    puts(" ************************* 회원 가입 ************************* ");
    printf("ID : ");
    if (scanf("%s", buf) > 0) {
        lseek(fd[1], (off_t)0, SEEK_SET);
        write(fd[1], &buf, strlen(buf));
        memset(buf, 0x00, MAX_BUF_SIZE);
    } else {
        printf("아이디가 너무 깁니다.\n");
        sleep(2);
    }

    // server로부터 입력된 buf에 따른 sign을 받아옴 0 중복, 1 미중복
    lseek(fd[0], (off_t)0, SEEK_SET);
    read(fd[0], &sign, sizeof(char));

    if (sign == '0') { // 중복이면
        sign = '\0';
        puts("이미 존재하는 ID입니다.");
        sleep(2);
        return;
    } else if (sign == '1') {
        sign = '\0';
        printf("PASSWORD : ");
        if (scanf("%s", buf) > 0) {
            lseek(fd[1], (off_t)0, SEEK_SET);
            write(fd[1], &buf, strlen(buf));
            memset(buf, 0x00, MAX_BUF_SIZE);
        } else {
            printf("비밀번호가 너무 깁니다.\n");
            sleep(2);
        }
        puts(" ************************* 회원가입 완료 "
             "************************* ");
        printf("\n");
        sleep(2);
    }
}

void game() {
    int gfd[3] = {
        0,
    };
    char buf[MAX_BUF_SIZE] = {
        0x00,
    };
    char sign;
    int num = 1;
    int inputNum = 0;
    int units = 0, tens = 0, hunds = 0;
    int temp;
    char strike, ball;

    gfd[0] = open("serverWR", O_RDONLY);
    gfd[1] = open("clientWR", O_WRONLY);

    printf("게임 시작\n");

    while (1) {
        printf("-%d번째 시도-\n", num);
        puts("공백없이 세자리 숫자 입력 ex)123");
        printf(">>> ");
        if (scanf("%d", &inputNum) == 0) {
            printf("프로그램을 종료합니다.[문자입력]\n");
            exit(0);
        } else {
            if (inputNum > 100 && inputNum < 1000) {
                temp = inputNum;
                units = temp % 10;
                temp /= 10;
                tens = temp % 10;
                temp /= 10;
                hunds = temp;
                if (units == 0 || tens == 0 || hunds == 0) {
                    printf("*경고! 각 자리 숫자는 1~9의 수입니다.\n");
                    printf("\n");
                    sleep(2);
                } else {
                    if (units != hunds && units != tens && tens != hunds) {
                        // 입력값 전달
                        sprintf(buf, "%d", inputNum);
                        lseek(gfd[1], (off_t)0, SEEK_SET);
                        write(gfd[1], &buf, strlen(buf));
                        memset(buf, 0x00, MAX_BUF_SIZE);

                        // get sign
                        lseek(gfd[0], (off_t)0, SEEK_SET);
                        read(gfd[0], &sign, sizeof(char));

                        switch (sign) {
                        case '1': // 진행
                            sign = '\0';
                            lseek(gfd[0], (off_t)0, SEEK_SET);
                            read(gfd[0], &strike, sizeof(char));
                            lseek(gfd[0], (off_t)0, SEEK_SET);
                            read(gfd[0], &ball, sizeof(char));

                            printf("%c스트라이크 %c볼입니다.\n", strike, ball);
                            printf("\n");

                            num++;
                            break;

                        case '2': // 승리
                            sign = '\0';
                            puts("승리했습니다!");
                            printf("\n");
                            return;
                        case '3': // 패배
                            sign = '\0';
                            puts("패배했습니다..");
                            printf("\n");
                            sleep(2);
                            return;
                        default:
                            break;
                        }
                    } else {
                        printf("*경고! 각 자리 숫자는 중복되면 안됩니다.\n");
                        printf("\n");
                        sleep(2);
                    }
                }
            } else {
                printf("*경고! 세자리 수를 입력해주세요\n");
                printf("\n");
                sleep(2);
            }
        }
    }
    close(gfd[0]);
    close(gfd[1]);
}
void record() {}
void ranking() {}
void help() {}

