#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF_SIZE 32 + 1
#define PERMS 0666

void game();

int main(int argc, char const *argv[]) {

    game();

    printf("게임종료！\n");

    return 0;
}

void game() {
    char check;
    int num = 0;
    int fd[3] = {
        0,
    };
    char buf[MAX_BUF_SIZE] = {
        0x00,
    };
    int a = 0;
    int units = 0, tens = 0, hunds = 0;
    int temp;

    fd[1] = open("clientWR", O_WRONLY);
    fd[0] = open("serverWR", O_RDONLY);

    printf("게임 시작\n");

    dup2(STDOUT_FILENO, fd[2]);
    dup2(fd[1], STDOUT_FILENO);

    while (1) {
        dup2(fd[2], STDOUT_FILENO);
        printf("\n공백없이 세자리수 입력 ex)123\n");
        dup2(fd[1], STDOUT_FILENO);
        if (scanf("%d", &a) == 0) {
            dup2(fd[2], STDOUT_FILENO);
            printf("숫자를 입력해주세요\n");
            while (getchar() != '\n')
                ;
            dup2(fd[1], STDOUT_FILENO);
        } else {
            if (a > 100 && a < 1000) {
                temp = a;
                units = temp % 10;
                temp /= 10;
                tens = temp % 10;
                temp /= 10;
                hunds = temp;
                if (units == 0 || tens == 0 || hunds == 0) {
                    dup2(fd[2], STDOUT_FILENO);
                    printf("각 자리 숫자는 1~9의 수입니다.\n");
                    while (getchar() != '\n')
                        ;
                    dup2(fd[1], STDOUT_FILENO);
                } else {
                    if (units != hunds && units != tens && tens != hunds) {
                        printf("%d", a);
                        fflush(stdout);
                        read(fd[0], &check, sizeof(char));
                        if (read(fd[0], (char *)buf, MAX_BUF_SIZE) > 0) {
                            dup2(fd[2], STDOUT_FILENO);
                            printf("%s", buf);
                            memset(buf, 0x00, MAX_BUF_SIZE);
                        }
                        if (check == 'a')
                            return;
                    } else {
                        dup2(fd[2], STDOUT_FILENO);
                        printf("각 자리 숫자는 중복되면 안됩니다.\n");
                        while (getchar() != '\n')
                            ;
                        dup2(fd[1], STDOUT_FILENO);
                    }
                }

            } else {
                dup2(fd[2], STDOUT_FILENO);
                printf("세자리 수를 입력해주세요\n");
                while (getchar() != '\n')
                    ;
                dup2(fd[1], STDOUT_FILENO);
            }
        }
    }
    close(fd[0]);
    close(fd[1]);
}
