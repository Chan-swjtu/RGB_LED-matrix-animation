#include <Arduino.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include "animation.h"
#include "patterns.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN_1 4
#define DATA_PIN_2 5
#define RX_PIN 8
#define TX_PIN 9
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 18 * 18
#define BRIGHTNESS 100
#define FRAMES_PER_SECOND 120

char tempArr[NUM_LEDS];
CRGB leds_1[NUM_LEDS / 2];
CRGB leds_2[NUM_LEDS / 2];
SoftwareSerial softSerial(RX_PIN, TX_PIN);
uint8_t gCurrentPatternNum = 0;   // 图案下标
uint8_t gPreviousPatternNum = 0;  // 图案下标（上一状态）
uint8_t gHue = 0;
int recvData = 0;
int recvNum = 0;

void setup() {
    Serial.begin(4800);
    softSerial.begin(9600);

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN_1, COLOR_ORDER>(leds_1, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN_2, COLOR_ORDER>(leds_2, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);  // 设置灯光亮度
}

void setPatternNum(int recvData) {
    gPreviousPatternNum = gCurrentPatternNum;
    switch (recvData) {
    case 0x98: gCurrentPatternNum = 6; break;  // 开灯
    case 243: gCurrentPatternNum = 0; break;   // 向左
    case 241: gCurrentPatternNum = 11; break;  // 向上
    case 153: gCurrentPatternNum = 3; break;   // 笑/厉害厉害
    case 161: gCurrentPatternNum = 4; break;   // 哭/丑八怪
    case 225: gCurrentPatternNum = 5; break;   // 爱心
    case 240: gCurrentPatternNum = 8; break;   // 灯光效果
    case 163: gCurrentPatternNum = 3; break;   // 熄灭
    default: gCurrentPatternNum = 0; break;
    }
}

// 为leds设置团案
void setLED(const char patternArr[], CRGB::HTMLColorCode color, int gradient = 0) {
    for (int i = 0; i < N_LED * N_LED; i++) {
        uint8_t row = i / N_LED, column = i % N_LED;
        uint8_t rest = (column % 2) ? (N_LED - row - 1) : row;
        uint16_t locate = N_LED * column + rest;
        if (locate < NUM_LEDS / 2) {  // 前半段
            if (pgm_read_byte_near(patternArr + i)) {
                leds_1[locate] = color;
                if (gradient) leds_1[locate] += CHSV(gHue, 255, 192);
            } else {
                leds_1[locate] = CRGB::Black;
            }
        } else {  // 后半段
            column = locate / N_LED;
            uint16_t first = column * N_LED;
            uint16_t last = (column + 1) * N_LED - 1;
            locate = last - (locate - first) - (NUM_LEDS / 2) - 1;
            if (pgm_read_byte_near(patternArr + i)) {
                leds_2[locate] = color;
                if (gradient) leds_2[locate] += CHSV(gHue, 255, 192);
            } else {
                leds_2[locate] = CRGB::Black;
            }
        }
    }
}

bool patternChanged() {
    return gCurrentPatternNum != gPreviousPatternNum;
}

void setPatternArr(const char* patternArr) {
    for (int i = 0; i < NUM_LEDS; i++) {
        tempArr[i] = patternArr[i];
    }
}

void checkArrow(const char* patternArr) {
    if (patternChanged()) setPatternArr(patternArr);
}

// 测试图案
void testPaterns() {
    setLED(testPattern, CRGB::Purple, 1);
}

// 灭灯状态
void off() {
    for (int i = 0; i < NUM_LEDS / 2; i++) {
        leds_1[i] = CRGB::Black;
        leds_2[i] = CRGB::Black;
    }
    delay(1000);
}

// 向上、下、左、右移动的箭头
void moveUpArrow() {
    delay(100);
    setLED(tempArr, CRGB::Green, 1);
    moveToUp(tempArr);
}

void moveDownArrow() {
    delay(100);
    setLED(tempArr, CRGB::Yellow, 1);
    moveToDown(tempArr);
}

void moveLeftArrow() {
    delay(100);
    setLED(tempArr, CRGB::Red, 1);
    moveToLeft(tempArr);
}

void moveRightArrow() {
    delay(100);
    setLED(tempArr, CRGB::Blue, 1);
    moveToRight(tempArr);
}

// 挥手
Wave wave;
void waving() {
    delay(150);
    setLED(wave.getPattern(), CRGB::Yellow);
    wave.nextPattern();
}

// 顺时针绕圈圈
Clockwise clockwise;
void clockwising() {
    delay(50);
    setLED(clockwise.getPattern(), CRGB::Green, 1);
    clockwise.nextPattern();
}


// 圆圈放大
Expand expand;
void expanding() {
    delay(100);
    setLED(expand.getPattern(), CRGB::Green, 1);
    expand.nextPattern();
}

// 爱心图案
void showHeart() {
    setLED(heartPattern, CRGB::Red);
}

// 正常脸部图案
void normalFace() {
    setLED(facePattern, CRGB::Blue);
}

// 笑脸图案
void smilingFace() {
    setLED(laughPattern, CRGB::Green);
}

// 哭脸图案
void cryFace() {
    setLED(cryPattern, CRGB::Red);
}

// 亲亲图案
void showKiss() {
    setLED(kissPattern, CRGB::Blue);
}

// 彩虹
void rainbow() {
    // FastLED's built-in rainbow generator
    fill_rainbow(leds_1, NUM_LEDS, gHue, 7);
}

void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
        leds_1[random16(NUM_LEDS)] += CRGB::White;
    }
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void confetti() {
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds_1, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds_1[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon() {
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds_1, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS - 1);
    leds_1[pos] += CHSV(gHue, 255, 192);
}

void bpm() {
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++) {  // 9948
        leds_1[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds_1, NUM_LEDS, 20);
    uint8_t dothue = 0;
    for (int i = 0; i < 8; i++) {
        leds_1[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

// 待机状态
void standby() {
    rainbow();
}

// 显示图案
void showPattern() {
    switch (gCurrentPatternNum) {
    case 0: clockwising(); break;
    case 1: waving(); break;
    case 2: showHeart(); break;
    case 3: off(); break;
    case 4: smilingFace(); break;
    case 5: normalFace(); break;
    case 6: expanding(); break;
    case 7: showKiss(); break;
    case 8:
        checkArrow(upArrowPattern);
        moveUpArrow();
        break;
    case 9:
        checkArrow(downArrowPattern);
        moveDownArrow();
        break;
    case 10:
        checkArrow(leftArrowPattern);
        moveLeftArrow();
        break;
    case 11:
        checkArrow(rightArrowPattern);
        moveRightArrow();
        break;
    default: gCurrentPatternNum = 0;
    }

    FastLED.show();
}


void loop() {
    recvNum = softSerial.available();
    if (recvNum) {
        recvData = softSerial.read();
        Serial.print("new data: ");
        Serial.println(recvData);
        setPatternNum(recvData);
    }

    showPattern();

    EVERY_N_MILLISECONDS(20) {
        gHue++;
    }

    EVERY_N_SECONDS(2) {
        gPreviousPatternNum = gCurrentPatternNum;
        gCurrentPatternNum++;
    }
}