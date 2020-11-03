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

void signUp();
int signIn();
void signChoice();
void game();

/***********정의할 함수***********/
void record();
void ranking();
void help();

/***********전역변수***********/
size_t num = 0; // 생성된 User 구조체 갯수
User users[MAX_USER_CAPACITY + 1] = {'\0', '\0', 0, 0}; // User 구조체
int loginNum; // 로그인에 이용된 구조체 배열 key
int fd[2] = {
    0,
};

/***********메인 함수***********/
int main() {
    mkfifo("serverWR", PERMS); //서버 입력용 파이프라인(유저 출력)
    mkfifo("clientWR", PERMS); //유저 입력용 파이프라인(서버 출력)

    while (1) { // login

        fd[0] = open("serverWR", O_WRONLY);
        fd[1] = open("clientWR", O_RDONLY);

        signChoice(); // 로그인 성공 시 종료됨.

        while (1) { // main
            int whilebrk = 0;
            char sign;

            fd[0] = open("serverWR", O_WRONLY);
            fd[1] = open("clientWR", O_RDONLY);

            puts("메인화면 waiting...");

            lseek(fd[1], (off_t)0, SEEK_SET);
            read(fd[1], &sign, sizeof(char));

            // 유저가 보내는 sign을 받아서 해당 함수 실행
            switch (sign) {
            case '1':
                puts(">>> game");
                game();
                break;
            case '2':
                puts(">>> record");
                printf("\n");
                record();
                break;
            case '3':
                puts(">>> ranking");
                printf("\n");
                ranking();
                break;
            case '4':
                puts(">>> help");
                printf("\n");
                help();
                break;
            case '5':
                puts(">>> logout");
                printf("\n");
                whilebrk = 1;
                break;
            default:
                break;
            }
            if (whilebrk == 1) {
                break;
            }
        }
    }

    close(fd[0]);
    close(fd[1]);
    return 0;
}

/***********회원가입***********/
void signUp() {
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign[1] = {0};
    int overlap = 0;

    puts(">> 회원가입");

    printf("입력받은 ID는 : ");
    lseek(fd[1], (off_t)0, SEEK_SET);
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
        lseek(fd[0], (off_t)0, SEEK_SET);
        write(fd[0], (char *)sign, sizeof(char));

        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));

        puts("회원가입 실패");
        printf("\n");
    } else { // num이 0인 경우도 overlap이 0이기 때문에 포함
        // 유저에게 non-overlap sign 전달
        sign[0] = '1';
        lseek(fd[0], (off_t)0, SEEK_SET);
        write(fd[0], (char *)sign, sizeof(char));

        strcpy(users[num].name, buf);

        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));

        // Password
        printf("입력받은 PASSWORD는 : ");
        lseek(fd[1], (off_t)0, SEEK_SET);
        read(fd[1], (char *)buf, MAX_BUF_SIZE);
        printf("%s\n", buf);
        strcpy(users[num].pwd, buf);

        memset(buf, 0x00, MAX_BUF_SIZE);

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
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign[1] = {0};
    int saveNum = -1;

    puts(">> 로그인");
    printf("입력받은 ID는 : ");
    lseek(fd[1], (off_t)0, SEEK_SET);
    read(fd[1], (char *)buf, MAX_BUF_SIZE);
    printf("%s\n", buf);
    // 아이디가 아예　없는 경우 : 0
    // 해당하는 아이디가 없는 경우 : 1
    // 해당하는 아이디가 있는 경우 : 2
    if (num == 0) {
        puts("None");
        printf("\n");
        sign[0] = '0';
        lseek(fd[0], (off_t)0, SEEK_SET);
        write(fd[0], (char *)sign, sizeof(char));

        memset(buf, 0x00, MAX_BUF_SIZE);
        memset(sign, 0x00, sizeof(char));

        return 0;
    } else {
        for (int i = 0; i < num; i++) {
            if (strcmp(buf, users[i].name) == 0) {
                sign[0] = '2';
                lseek(fd[0], (off_t)0, SEEK_SET);
                write(fd[0], (char *)sign, sizeof(char));

                memset(buf, 0x00, MAX_BUF_SIZE);
                memset(sign, 0x00, sizeof(char));

                saveNum = i;
                break;
            }
        }
        if (saveNum == -1) {
            printf("\n");
            sign[0] = '1';
            lseek(fd[0], (off_t)0, SEEK_SET);
            write(fd[0], (char *)sign, sizeof(char));

            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));

            return 0;
        }
    }
    // 패스워드 오류 : 0
    // 로그인 성공 : 1
    if (saveNum != -1) {
        printf("입력받은 PASSWORD는 : ");
        lseek(fd[1], (off_t)0, SEEK_SET);
        read(fd[1], (char *)buf, MAX_BUF_SIZE);
        printf("%s\n", buf);
        if (strcmp(buf, users[saveNum].pwd) == 0) {
            puts("로그인 성공");
            printf("\n");
            loginNum = saveNum;
            sign[0] = '1';
            lseek(fd[0], (off_t)0, SEEK_SET);
            write(fd[0], (char *)sign, sizeof(char));

            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));

            return 1;
        } else {
            puts("패스워드 오류");
            printf("\n");
            sign[0] = '0';
            lseek(fd[0], (off_t)0, SEEK_SET);
            write(fd[0], (char *)sign, sizeof(char));

            memset(buf, 0x00, MAX_BUF_SIZE);
            memset(sign, 0x00, sizeof(char));

            return 0;
        }
    }
}

/***********로그인 ING***********/
void signChoice() {
    char sign;

    while (1) {
        puts("login 대기중...");

        lseek(fd[1], (off_t)0, SEEK_SET);
        read(fd[1], &sign, sizeof(char));
        switch (sign) {
        case '0':
            if (signIn() == 1) {

                close(fd[0]);
                close(fd[1]);
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
}

void game() {
    int gfd[3] = {
        0,
    };
    int rcvNum = 0;
    char buf[MAX_BUF_SIZE] = {
        0x00,
    };
    char sign[1] = {0};

    int serverNum[3] = {
        0,
    };
    int userNum[3] = {
        0,
    };
    int num = 1;
    int target = 0;
    int temp;

    gfd[0] = open("serverWR", O_WRONLY);
    gfd[1] = open("clientWR", O_RDONLY);

    srand(time(NULL));
    serverNum[2] = (rand() % 9) + 1; //숫자야구용 랜덤 세자리수 생성
    while (1) {
        serverNum[1] = (rand() % 9) + 1;
        if (serverNum[1] != serverNum[2])
            break;
    }
    while (1) {
        serverNum[0] = (rand() % 9) + 1;
        if (serverNum[0] != serverNum[2] && serverNum[0] != serverNum[1])
            break;
    }
    target = 100 * serverNum[0] + 10 * serverNum[1] + serverNum[2];
    printf("타겟 숫자 : %d\n", target);
    printf("\n");

    while (1) {
        int strike = 0;
        int ball = 0;

        printf("%d번째 시도\n", num++);

        // 유저 입력 read
        lseek(gfd[1], (off_t)0, SEEK_SET);
        read(gfd[1], (char *)buf, MAX_BUF_SIZE);
        rcvNum = atoi(buf); // 읽은 buf int로 rcvNum 에 저장 (세자리 수)
        memset(buf, 0x00, MAX_BUF_SIZE); // buf 클리어

        printf("입력받은 숫자 : %d\n", rcvNum);

        temp = rcvNum;
        userNum[2] = temp % 10;
        temp /= 10;
        userNum[1] = temp % 10;
        temp /= 10;
        userNum[0] = temp;

        for (int i = 0; i < 3; i++) {
            if (userNum[i] == serverNum[i]) {
                strike++;
            }
            for (int j = 0; j < 3; j++) {
                if (i != j && userNum[i] == serverNum[j]) {
                    ball++;
                }
            }
        }
        // 결과 전달 sign   게임진행:1  정답:2  패배:3
        if (num > 9 && strike != 3) { // 서버 승리
            puts("서버 승리");
            printf("\n");
            sign[0] = '3';
            lseek(gfd[0], (off_t)0, SEEK_SET);
            write(gfd[0], (char *)sign, sizeof(char));
            memset(sign, 0x00, sizeof(char));
            return;
        } else { // 유저 승리
            if (strike == 3) {
                puts("유저 승리");
                printf("\n");
                sign[0] = '2';
                lseek(gfd[0], (off_t)0, SEEK_SET);
                write(gfd[0], (char *)sign, sizeof(char));
                memset(sign, 0x00, sizeof(char));
                return;
            } else { // 진행
                puts("진행");
                printf("\n");
                sign[0] = '1';
                lseek(gfd[0], (off_t)0, SEEK_SET);
                write(gfd[0], (char *)sign, sizeof(char));
                memset(sign, 0x00, sizeof(char));

                // 스트라이크 전달
                sprintf(buf, "%d", strike);
                lseek(gfd[0], (off_t)0, SEEK_SET);
                write(gfd[0], &buf, strlen(buf));
                memset(buf, 0x00, MAX_BUF_SIZE);

                // 볼 전달
                sprintf(buf, "%d", ball);
                lseek(gfd[0], (off_t)0, SEEK_SET);
                write(gfd[0], &buf, strlen(buf));
                memset(buf, 0x00, MAX_BUF_SIZE);
            }
        }
    }
    close(gfd[0]);
    close(gfd[1]);
}

void record() {}
void ranking() {}
void help() {}
