#ifndef __MSG_H__
#define __MSG_H__

#define SIZE 1024
#define QUEST_SIZE sizeof(QuestSign) - sizeof(long)
#define RESPONSE_SIZE sizeof(ResponseSign) - sizeof(long)
#define QUEST_GAME_SIZE sizeof(QuestGame) - sizeof(long)
#define RESPONSE_GAME_SIZE sizeof(ResponseGame) - sizeof(long)

typedef enum {
    SIGN_CHOICE = 0,
    SIGN_IN = 1,
    SIGN_UP = 2,
    PLAY_GAME = 3,
    RECORD = 4,
    RAKING = 5,
    HELP = 6,
    SIGN_OUT = 7
} Service;

typedef struct __Quest_Sign {
    long pid;
    Service service;
    char UsrId[SIZE];
    char UsrPassword[SIZE];
} QuestSign;

typedef struct __Response_Sign {
    long pid;
    char responsceData[SIZE];
    char UsrId[SIZE];
    int win;
    int lose;
} ResponseSign;

typedef struct __Quest_Game {
    long pid;
    int target;
} QuestGame;

typedef struct __Response_Game {
    long pid;
    int strike;
    int ball;
    int answer;
} ResponseGame;

#endif //__MSG_H__
