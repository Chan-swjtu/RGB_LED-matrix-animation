#ifndef ANIMATION_H
#define ANIMATION_H

#include "patterns.h"

#define N_LED 18  // 每行或每列有多少LED灯珠

// 一个笛卡尔坐标系的点
class CCS {
public:
    int x, y;
    CCS();
    CCS(int i);
    CCS(int x_, int y_);
    int locateI();
    int toRealIndex();
};

class Animation {
    uint8_t cnt = 0;     // 动画进行到第几帧
    uint8_t patternNum;  // 共有几帧

public:
    virtual const char* getPattern() = 0;
    uint8_t getCnt();
    void setPatternNum(uint8_t num);
    virtual void nextPattern();
};

// 挥手
class Wave : public Animation {
public:
    Wave();
    const char* getPattern();
};

// 顺时针转圈
class Clockwise : public Animation {
public:
    Clockwise();
    const char* getPattern();
};

// 圆圈变大
class Expand : public Animation {
public:
    Expand();
    const char* getPattern();
};

int locateX(int i, int n);
int locateY(int i, int n);
void toRealArray(const char patternArr[], int raelArr[]);
void moveToRight(char array[]);
void moveToLeft(char array[]);
void moveToUp(char array[]);
void moveToDown(char array[]);

#endif