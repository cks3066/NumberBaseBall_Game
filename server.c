#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "Msg.h"

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
    int rank;
} User;

void Init();
void getUsrData();
void writeUsrData();

void signUp();
int signIn();
void signChoice();
void game();

/***********정의할 함수***********/
void record();
void set_rank();
void ranking();
void help();

void signalHandler(int signum);
void EndFunc();

/***********전역변수***********/
size_t num = 0; // 생성된 User 구조체 갯수
User users[MAX_USER_CAPACITY + 1] = {'\0', '\0', 0, 0}; // User 구조체
int loginNum; // 로그인에 이용된 구조체 배열 key
int fd[2] = {
    0,
};
QuestSign questsign;
ResponseSign responsesigh;
QuestGame questgame;
ResponseGame responsegame;
key_t mykey;
long mspid;

/***********메인 함수***********/
int main() {
    atexit(EndFunc);
    signal(SIGINT, signalHandler);
    Init();
    getUsrData();

    while (1) { // login

        signChoice(); // 로그인 성공 시 종료됨.

        while (1) { // main
            int whilebrk = 0;
            char sign;

            puts("메인화면 waiting...");

            msgrcv(mspid, &questsign, QUEST_SIZE, 0, 0);

            // 유저가 보내는 sign을 받아서 해당 함수 실행
            switch (questsign.service) {
            case PLAY_GAME:
                puts(">>> game");
                game();
                break;
            case RECORD:
                puts(">>> record");
                printf("\n");
                record();
                break;
            case RAKING:
                puts(">>> ranking");
                printf("\n");
                ranking();
                break;
            case HELP:
                puts(">>> help");
                printf("\n");
                help();
                break;
            case SIGN_OUT:
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

    return 0;
}

void Init() {
    mykey = ftok("./Msg.h", 1);
    mspid = msgget(mykey, IPC_CREAT | 0600);
    memset(&questsign, 0x00, sizeof(questsign));
    memset(&responsesigh, 0x00, sizeof(responsesigh));
    memset(&questgame, 0x00, sizeof(questgame));
    memset(&responsegame, 0x00, sizeof(responsegame));
}

/***********회원가입***********/
void signUp() {
    int overlap = 0;

    puts(">> 회원가입");

    printf("입력받은 ID는 : ");
    printf("%s\n", questsign.UsrId);

    ////////////////////////////////////////////
    //기존　users.name 과 중복체크
    ///////////////////////////////////////////
    if (num != 0) { // 등록된 유저 정보가 있다면
        for (int i = 0; i < num; i++) {
            if (strcmp(users[i].name, questsign.UsrId) == 0) //중복 존재시
                overlap = 1;
        }
    }

    // 중복 존재 시 0   responsesigh.responsceData[0]
    // 중복 없을 시 1   responsesigh.responsceData[0]
    if (overlap == 1) {
        // 유저에게 overlap sign 전달
        responsesigh.responsceData[0] = 0;
        msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);

        puts("회원가입 실패");
        printf("\n");
    } else { // num이 0인 경우도 overlap이 0이기 때문에 포함
        // 유저에게 non-overlap responsesigh.responsceData[0] 전달
        responsesigh.responsceData[0] = 1;
        msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);

        strcpy(users[num].name, questsign.UsrId);

        // Password
        printf("입력받은 PASSWORD는 : ");
        printf("%s\n", questsign.UsrPassword);
        strcpy(users[num].pwd, questsign.UsrPassword);

        users[num].win = 0;
        users[num].lose = 0;
        puts("회원가입 성공");
        printf("\n");

        num++;
        users[num - 1].rank = num;
        writeUsrData();
    }
}

/***********로그인***********/
// 로그인 실패 : 0 반환
// 로그인 성공 : 1 반환
int signIn() {
    // num 만큼　구조체를　돌면서　name 체크 - saveNum에 저장
    // name　발견시　해당　num의　pwd 체크
    int saveNum = -1;

    puts(">> 로그인");
    printf("입력받은 ID는 : ");
    printf("%s\n", questsign.UsrId);
    // 아이디가 아예　없는 경우 : 0
    // 해당하는 아이디가 없는 경우 : 1
    // 해당하는 아이디가 있는 경우 : 2
    if (num == 0) {
        puts("None");
        printf("\n");
        responsesigh.responsceData[0] = 0;
        msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);
        return 0;
    } else {
        for (int i = 0; i < num; i++) {
            if (strcmp(questsign.UsrId, users[i].name) == 0) {
                responsesigh.responsceData[0] = 2;
                msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);

                saveNum = i;
                sleep(1);
                break;
            }
        }
        if (saveNum == -1) {
            printf("\n");
            responsesigh.responsceData[0] = 1;
            msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);

            return 0;
        }
    }
    // 패스워드 오류 : 0
    // 로그인 성공 : 1
    if (saveNum != -1) {
        printf("입력받은 PASSWORD는 : ");
        printf("%s\n", questsign.UsrPassword);
        if (strcmp(questsign.UsrPassword, users[saveNum].pwd) == 0) {
            puts("로그인 성공");
            printf("\n");
            loginNum = saveNum;
            responsesigh.responsceData[0] = 1;
            msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);

            return 1;
        } else {
            puts("패스워드 오류");
            printf("\n");
            responsesigh.responsceData[0] = 0;
            msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);
            return 0;
        }
    }
}

/***********로그인 ING***********/
void signChoice() {

    while (1) {
        puts("login 대기중...");

        msgrcv(mspid, &questsign, QUEST_SIZE, 0, 0);
        responsesigh.pid = questsign.pid;
        switch (questsign.service) {
        case SIGN_IN:
            if (signIn() == 1) {
                return;
            }
            break;
        case SIGN_UP:
            signUp();
            break;
        default:
            break;
        }
    }
}

void game() {
    int rcvNum = 0;

    int serverNum[3] = {
        0,
    };
    int userNum[3] = {
        0,
    };
    int num = 1;
    int target = 0;
    int temp;

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

        msgrcv(mspid, &questgame, QUEST_GAME_SIZE, 0, 0);
        responsegame.pid = questgame.pid;
        rcvNum = questgame.target; // 메시지 큐로 받은 questgame.target 으로
                                   // rcvNum 에 저장 (세자리 수)

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
        // 결과 전달 responscegame.answer   게임진행:1  정답:2  패배:3
        if (num > 9 && strike != 3) { // 서버 승리
            puts("서버 승리");
            printf("\n");
            responsegame.answer = 3;
            msgsnd(mspid, &responsegame, RESPONSE_GAME_SIZE, 0);
            users[loginNum].lose++;
            writeUsrData();
            return;
        } else { // 유저 승리
            if (strike == 3) {
                puts("유저 승리");
                printf("\n");
                responsegame.answer = 2;
                msgsnd(mspid, &responsegame, RESPONSE_GAME_SIZE, 0);
                users[loginNum].win++;
                set_rank();
                writeUsrData();
                return;
            } else { // 진행
                puts("진행");
                printf("\n");
                // // 볼 스트라이크 전달
                responsegame.ball = ball;
                responsegame.strike = strike;
                responsegame.answer = 1;
                msgsnd(mspid, &responsegame, RESPONSE_GAME_SIZE, 0);
            }
        }
    }
}

void getUsrData() {
    FILE *pFile = NULL;
    pFile = fopen("./UserData.txt", "r");
    if (pFile == NULL)
        return;
    if (pFile != NULL) {
        char strTemp[MAX_BUF_SIZE];
        char *pStr;
        while (!feof(pFile)) {
            pStr = fgets(strTemp, sizeof(strTemp), pFile);
            if (pStr == NULL)
                break;
            for (int i = 0; i < 5; i++) {
                switch (i) {
                case 0:
                    strcpy(users[num].name, strtok(pStr, " "));
                    break;
                case 1:
                    strcpy(users[num].pwd, strtok(NULL, " "));
                    break;
                case 2:
                    users[num].win = atoi(strtok(NULL, " "));
                    break;
                case 3:
                    users[num].lose = atoi(strtok(NULL, " "));
                    break;
                case 4:
                    users[num].rank = atoi(strtok(NULL, " "));
                }
            }
            num++;
        }
        fclose(pFile);
    } else {
        printf("no data let's sign up");
    }
}

void writeUsrData() {
    char buf[MAX_BUF_SIZE];
    char userdata[MAX_BUF_SIZE];
    fd[0] = open("./UserData.txt", O_WRONLY);
    for (int i = 0; i < num; i++) {
        strcpy(userdata, users[i].name);
        strcat(userdata, " ");
        strcat(userdata, users[i].pwd);
        strcat(userdata, " ");
        sprintf(buf, "%d ", users[i].win);
        strcat(userdata, buf);
        sprintf(buf, "%d ", users[i].lose);
        strcat(userdata, buf);
        sprintf(buf, "%d\n", users[i].rank);
        strcat(userdata, buf);

        write(fd[0], (char *)userdata, strlen(userdata));
    }
    close(fd[0]);
}

void record() {
    printf("유저의 전적을 보여줍니다.\n");
    strcpy(responsesigh.UsrId, users[loginNum].name);
    responsesigh.win = users[loginNum].win;
    responsesigh.lose = users[loginNum].lose;
    msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);
    sleep(2);
}
void ranking() {
    printf("랭킹을 보여줍니다.\n");
    set_rank();
    strcpy(responsesigh.UsrId, users[loginNum].name);
    responsesigh.win = users[loginNum].rank;
    msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);
    sleep(1);
}

void set_rank() {
    strcpy(questsign.UsrId, users[loginNum].name);
    User temp;
    for (int i = 0; i < num - 1; i++) {
        for (int j = 0; j < num - 1; j++) {
            if (users[j].win < users[j + 1].win) {
                temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < num; i++) {
        users[i].rank = i + 1;
    }

    for (int i = 0; i < num; i++) {
        if (strcmp(users[i].name, questsign.UsrId) == 0) //현재 로그인한 아이디
            loginNum = i;
    }
}

void help() {
    printf("숫자야구 도움말을 보여줍니다.\n");
    fd[0] = open("./help.txt", O_RDONLY);
    read(fd[0], &responsesigh.responsceData, SIZE);
    msgsnd(mspid, &responsesigh, RESPONSE_SIZE, 0);
    sleep(1);
}

void signalHandler(int signum) {
    if (signum == SIGINT) {
        msgctl(mspid, IPC_RMID, NULL);
        exit(0);
    }
}

void EndFunc() {
    msgctl(mspid, IPC_RMID, NULL);
    exit(0);
}
