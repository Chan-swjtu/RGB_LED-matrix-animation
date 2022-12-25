#include "animation.h"

CCS::CCS() {
    x = y = 0;
}

CCS::CCS(int i) {
    x = locateX(i, N_LED);
    y = locateY(i, N_LED);
}

CCS::CCS(int x_, int y_) {
    x = x_;
    y = y_;
}

int CCS::locateI() {
    return (-y + N_LED / 2 - (y > 0 ? 0 : 1)) * N_LED + (x + N_LED / 2 - (x > 0 ? 1 : 0));
}

int CCS::toRealIndex() {
    int column = x + N_LED / 2 - (x > 0 ? 1 : 0);  // 列数
    if (column % 2) {
        return column * N_LED + (y + N_LED / 2 - (y > 0 ? 1 : 0));
    } else {
        return column * N_LED + (-y + N_LED / 2 - (y > 0 ? 0 : 1));
    }
}

// 动画类
uint8_t Animation::getCnt() {
    return cnt;
}

void Animation::setPatternNum(uint8_t num) {
    patternNum = num;
}

void Animation::nextPattern() {
    if (++cnt > patternNum - 1) cnt = 0;
}

// 挥手
Wave::Wave() {
    setPatternNum(5);
}

const char* Wave::getPattern() {
    switch (getCnt()) {
    case 0: return wavePattern_1;
    case 1: return wavePattern_2;
    case 2: return wavePattern_3;
    case 3: return wavePattern_2;
    case 4: return wavePattern_1;
    default: return wavePattern_1;
    }
}

// 顺时针转圈
Clockwise::Clockwise() {
    setPatternNum(8);
}
const char* Clockwise::getPattern() {
    switch (getCnt()) {
    case 0: return clockwise_1;
    case 1: return clockwise_2;
    case 2: return clockwise_3;
    case 3: return clockwise_4;
    case 4: return clockwise_5;
    case 5: return clockwise_6;
    case 6: return clockwise_7;
    case 7: return clockwise_8;
    default: return clockwise_1;
    }
}

// 圆圈变大
Expand::Expand() {
    setPatternNum(5);
}

const char* Expand::getPattern() {
    switch (getCnt()) {
    case 0: return wavePattern_1;
    case 1: return wavePattern_2;
    case 2: return wavePattern_3;
    case 3: return wavePattern_2;
    case 4: return wavePattern_1;
    default: return wavePattern_1;
    }
}

int locateX(int i, int n) {
    return (i % n) - ((i % n) >= (n / 2) ? (n / 2 - 1) : (n / 2));
}

int locateY(int i, int n) {
    return ((i / n) >= (n / 2) ? (n / 2 - 1) : (n / 2)) - (i / n);
}

void moveToRight(char array[]) {
    CCS arr[N_LED * N_LED];
    int cnt = 0;
    for (int i = 0; i < N_LED * N_LED; i++) {
        if (array[i]) arr[cnt++] = CCS(i);
        array[i] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        arr[i].x++;  // 向右平移
        if (arr[i].x == 0) arr[i].x++;
        if (arr[i].x > N_LED / 2) arr[i].x = -N_LED / 2;
        array[arr[i].locateI()] = 1;
    }
}

void moveToLeft(char array[]) {
    CCS arr[N_LED * N_LED];
    int cnt = 0;
    for (int i = 0; i < N_LED * N_LED; i++) {
        if (array[i]) arr[cnt++] = CCS(i);
        array[i] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        arr[i].x--;  // 向左平移
        if (arr[i].x == 0) arr[i].x--;
        if (arr[i].x < -N_LED / 2) arr[i].x = N_LED / 2;
        array[arr[i].locateI()] = 1;
    }
}

void moveToUp(char array[]) {
    CCS arr[N_LED * N_LED];
    int cnt = 0;
    for (int i = 0; i < N_LED * N_LED; i++) {
        if (array[i]) arr[cnt++] = CCS(i);
        array[i] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        arr[i].y++;  // 向上平移
        if (arr[i].y == 0) arr[i].y++;
        if (arr[i].y > N_LED / 2) arr[i].y = -N_LED / 2;
        array[arr[i].locateI()] = 1;
    }
}

void moveToDown(char array[]) {
    int cnt = 0;
    CCS arr[N_LED * N_LED];
    for (int i = 0; i < N_LED * N_LED; i++) {
        if (array[i]) arr[cnt++] = CCS(i);
        array[i] = 0;
    }
    for (int i = 0; i < cnt; i++) {
        arr[i].y--;  // 向下平移
        if (arr[i].y == 0) arr[i].y--;
        if (arr[i].y < -N_LED / 2) arr[i].y = N_LED / 2;
        array[arr[i].locateI()] = 1;
    }
}

// 图案数组转实际数组
void toRealArray(const char patternArr[], int realArr[]) {
    for (int i = 0; i < N_LED * N_LED; i++) {
        int row = i / N_LED, column = i % N_LED;
        int rest = (column % 2) ? (N_LED - row - 1) : row;
        int locate = N_LED * column + rest;
        realArr[locate] = pgm_read_byte_near(patternArr + i);
    }
}
