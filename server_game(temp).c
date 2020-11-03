#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define PERMS 0666
#define MAX_BUF_SIZE 32 + 1
void game();
int main(int argc, char const *argv[]) {

    game();
    return 0;
}
void game() {
    int num = 0;
    int fd[3] = {
        0,
    };
    int rcv = 0;
    char buf[MAX_BUF_SIZE] = {
        0x00,
    };
    int serverNum[3] = {
        0,
    };
    int userNum[3] = {
        0,
    };
    int temp;
    int strike = 0;
    int ball = 0;

    mkfifo("serverWR", PERMS); //서버 입력용 파이프라인(유저 출력)
    mkfifo("clientWR", PERMS); //유저 입력용 파이프라인(서버 출력)

    dup2(STDOUT_FILENO, fd[2]);

    srand(time(NULL));
    serverNum[2] = (rand() % 9) + 1; //숫자야구용 랜덤 세자리수
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
    num = 100 * serverNum[0] + 10 * serverNum[1] + serverNum[2];
    printf("%d\n", num);
    fd[0] = open("clientWR", O_RDONLY); // 4번 fd 서버 출력용 파이프라인
    fd[1] = open("serverWR", O_WRONLY); // 5번 fd 서버 입력용 파이프라인
    while (1) {
        strike = 0;
        ball = 0;
        //　buf에 출력 파이프 read
        if (read(fd[0], (char *)buf, MAX_BUF_SIZE) > 0) {
            rcv = atoi(buf); // 읽은 buf int로 rcv 에 저장 (세자리 수)
            memset(buf, 0x00, MAX_BUF_SIZE); // buf 클리어
            printf("%d\n", rcv);
            temp = rcv;
            userNum[2] = temp % 10;
            temp /= 10;
            userNum[1] = temp % 10;
            temp /= 10;
            userNum[0] = temp;
            printf("%d %d %d\n", userNum[0], userNum[1], userNum[2]);
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
            if (strike == 3) {
                dup2(fd[1], STDOUT_FILENO);
                printf("a정답입니다.\n");
                fflush(stdout);
                dup2(fd[2], STDOUT_FILENO);
            } else {
                dup2(fd[1], STDOUT_FILENO);
                printf("b%d스트라이크 %d볼입니다.\n", strike, ball);
                fflush(stdout);
                dup2(fd[2], STDOUT_FILENO);
            }
        }
    }
}
