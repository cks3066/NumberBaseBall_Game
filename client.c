#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Msg.h"

#define PERMS 0666

#define MAX_BUF_SIZE 32
#define MAX_NAME_SIZE 10
#define MAX_PWD_SIZE 10

QuestSign questsign;
ResponseSign responsesign;
QuestGame questgame;
ResponseGame responsegame;
key_t mykey;
long mspid;

void Init(); //메시지 큐 할당

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
    Init();

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
            puts(" ***************************** 메인화면 "
                 "***************************** ");
            puts("  게임시작 1번，전적확인 2번, 랭킹확인 3번, 도움말 4번, "
                 "로그아웃 "
                 "5번");
            printf("  >>> ");
            scanf("%d", &choice);

            switch (choice) {
            case 1: // game
                system("clear");
                questsign.service = PLAY_GAME;
                msgsnd(mspid, &questsign, QUEST_SIZE, 0);
                game();
                break;
            case 2: // record
                system("clear");
                questsign.service = RECORD;
                msgsnd(mspid, &questsign, QUEST_SIZE, 0);
                record();
                sleep(1);
                break;
            case 3: // ranking
                system("clear");
                questsign.service = RAKING;
                msgsnd(mspid, &questsign, QUEST_SIZE, 0);
                ranking();
                sleep(2);
                break;
            case 4: // help
                system("clear");
                questsign.service = HELP;
                msgsnd(mspid, &questsign, QUEST_SIZE, 0);
                help();
                sleep(2);
                break;
            case 5: // logout
                system("clear");
                puts("logout");
                printf("\n");
                questsign.service = SIGN_OUT;
                msgsnd(mspid, &questsign, QUEST_SIZE, 0);
                sleep(1);
                whilebrk = 1;
                break;
            default:
                break;
            }
            if (whilebrk == 1)
                break;
        }
    }
    return 0;
}

/**********메시지 큐 ***********/
void Init() {
    mykey = ftok("./Msg.h", 1);
    mspid = msgget(mykey, IPC_CREAT);
    memset(&questsign, 0x00, sizeof(questsign));
    memset(&responsesign, 0x00, sizeof(responsesign));
    memset(&questgame, 0x00, sizeof(questgame));
    memset(&responsegame, 0x00, sizeof(responsegame));
    questgame.pid = getpid();
    questsign.pid = getpid();
}

/***********로그인 UI***********/
void signChoice() {
    int choice;
    char sign[1] = {0};

    while (1) {
        system("clear");
        puts(" ************************* 로그인 선택창 "
             "************************* ");
        puts("  로그인 1번，    회원가입 2번");
        printf("  >>> ");
        scanf("%d", &choice);
        printf("\n");

        // 로그인 : 0
        // 회원가입 : 1
        switch (choice) {
        case 1:
            system("clear");
            if (signIn() == 1) {
                return;
            }
            break;
        case 2:
            system("clear");
            signUp();
            break;
        default:
            break;
        }
    }
}

/***********로그인***********/
int signIn() {
    char buf[MAX_BUF_SIZE + 1] = {
        0,
    };
    char sign;

    questsign.service = SIGN_IN;
    puts(" ************************* 로그인 ************************* ");
    printf("                ID :        ");
    scanf("%s", questsign.UsrId);
    if (strlen(questsign.UsrId) > MAX_NAME_SIZE) {
        printf("    #아이디가 너무 깁니다. 글자 수 제한은 10입니다#\n");
        sleep(2);
        return 0;
    }
    printf("                PASSWORD :  ");
    scanf("%s", questsign.UsrPassword);
    if (strlen(questsign.UsrPassword) > MAX_PWD_SIZE) {
        printf("    #비밀번호가 너무 깁니다. 글자 수 제한은 10입니다#\n");
        sleep(2);
        return 0;
    }
    msgsnd(mspid, &questsign, QUEST_SIZE, 0);
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);
    switch (responsesign.responsceData[0]) {
    case 0:
        puts("              #아이디를 생성해주세요#");
        printf("\n");
        sleep(2);
        return 0;
    case 1:
        puts("            #일치하는 아이디가 없습니다#");
        printf("\n");
        sleep(2);
        return 0;
    case 2:
        break;
    default:
        break;
    }
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);
    // Password에 대한 sign

    switch (responsesign.responsceData[0]) {
    case 0:
        puts("              #비밀번호가　틀렸습니다#");
        printf("\n");
        sleep(2);
        return 0;
    case 1:
        puts(" ********************** 로그인 완료 "
             "********************** ");
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

    questsign.service = SIGN_UP;
    puts("************************* 회원 가입 ************************* ");
    printf("                ID :        ");
    scanf("%s", questsign.UsrId);
    if (strlen(questsign.UsrId) > MAX_NAME_SIZE) {
        printf("  #아이디가 너무 깁니다. 글자 수 제한은 10입니다#\n");
        sleep(1);
        return;
    }
    printf("                PASSWORD :  ");
    scanf("%s", questsign.UsrPassword);
    if (strlen(questsign.UsrPassword) > MAX_PWD_SIZE) {
        printf(" #비밀번호가 너무 깁니다. 글자 수 제한은 10입니다#\n");
        sleep(1);
        return;
    }
    msgsnd(mspid, &questsign, QUEST_SIZE, 0);
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);

    // server로부터 입력된 buf에 따른 sign을 받아옴 0 중복, 1 미중복

    if (responsesign.responsceData[0] == 0) { // 중복이면
        // sign = '\0';
        puts("              #이미 존재하는 ID입니다#");
        sleep(1);
        return;
    } else if (responsesign.responsceData[0] == 1) {
        puts("*********************** 회원가입 완료 "
             "*********************** ");
        printf("\n");
        sleep(1);
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

    printf("게임 시작\n");

    while (1) {
        printf("-%d번째 시도-\n", num);
        puts("  공백없이 세자리 숫자 입력 ex)123");
        printf("    >>> ");
        if (scanf("%d", &questgame.target) == 0) {
            printf("    프로그램을 종료합니다.[문자입력]\n");
            exit(0);
        } else {
            if (questgame.target > 100 && questgame.target < 1000) {
                temp = questgame.target;
                units = temp % 10;
                temp /= 10;
                tens = temp % 10;
                temp /= 10;
                hunds = temp;
                if (units == 0 || tens == 0 || hunds == 0) {
                    printf("    *경고! 각 자리 숫자는 1~9의 수입니다.\n");
                    printf("\n");
                    sleep(2);
                } else {
                    if (units != hunds && units != tens && tens != hunds) {
                        // 입력값 전달
                        msgsnd(mspid, &questgame, QUEST_GAME_SIZE, 0);
                        msgrcv(mspid, &responsegame, RESPONSE_GAME_SIZE,
                               questgame.pid, 0);

                        switch (responsegame.answer) {
                        case 1: // 진행

                            printf("    %d스트라이크 %d볼입니다.\n",
                                   responsegame.strike, responsegame.ball);
                            printf("\n");

                            num++;
                            break;

                        case 2: // 승리
                            puts("  승리했습니다!");
                            printf("\n");
                            sleep(2);
                            return;
                        case 3: // 패배
                            puts("  패배했습니다..");
                            printf("\n");
                            sleep(2);
                            return;
                        default:
                            break;
                        }
                    } else {
                        printf(
                            "    *경고! 각 자리 숫자는 중복되면 안됩니다.\n");
                        printf("\n");
                        sleep(2);
                    }
                }
            } else {
                printf("    *경고! 세자리 수를 입력해주세요\n");
                printf("\n");
                sleep(2);
            }
        }
    }
}
void record() {
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);
    printf(" ***************** 내 전적 *****************\n\n");
    printf("            사용자 아이디 : %s\n", responsesign.UsrId);
    printf("            승  리  :   %d\n", responsesign.win);
    printf("            패  배  :   %d\n\n", responsesign.lose);
    printf(" *******************************************\n");
    char skip;
    printf("아무 키나 입력하세요.");
    scanf("%s", &skip);
}
void ranking() {
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);
    printf(" ***************** 내 랭크 *****************\n\n");
    printf("            사용자 아이디 : %s\n", responsesign.UsrId);
    printf("            순  위  :   %d\n\n", responsesign.win);
    printf(" *******************************************\n");
    char skip;
    printf("아무 키나 입력하세요.");
    scanf("%s", &skip);
}

void help() {
    msgrcv(mspid, &responsesign, RESPONSE_SIZE, questsign.pid, 0);
    printf("%s\n", responsesign.responsceData);
    char skip;
    printf("아무 키나 입력하세요.");
    scanf("%s", &skip);
}
