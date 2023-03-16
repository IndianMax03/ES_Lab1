#include "hal.h"

int LED[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};
int SW[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
int FRAME_COUNT = 12;
int DIOD_MAX_INDEX = 7;
int DELAY = 500;
int TRIO_RIGHT_LIMITATION = 6;
int TRIO_LEFT_LIMITATION = 10;
int DUO_LEFT_LIMITATION = 7;

void setGreen();

void setYellow();

void setRed();

void mapSwToLeds();

void clearSwMapping();

bool isSwCorrect();

bool isNBTNClicked();

void startAnimation();

void stopAnimation();

void flashDiods(int frame);

void setTrios(int frame, GPIO_PinState state);

void setDuos(int frame, GPIO_PinState state);


int umain() {

    while (1) {
        if (isSwCorrect()) {
            clearSwMapping();
            setGreen();
            startAnimation();
        } else {
            setYellow();
            mapSwToLeds();
        }
    }

    return 0;
}

void setGreen() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void setYellow() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void setRed() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void mapSwToLeds() {
    for (int i = 0; i < 4; i++) {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, SW[i]);
        HAL_GPIO_WritePin(GPIOD, LED[i], state);
    }
}

void clearSwMapping() {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOD, LED[i], GPIO_PIN_RESET);
    }
}

bool isSwCorrect() {
    bool is3Disabled = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET;       //  2^3
    bool is2Abled = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET;            //  2^2
    bool is1Disabled = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_RESET;      //  2^1
    bool is0Disabled = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_RESET;      //  2^0
    return is3Disabled && is2Abled && is1Disabled && is0Disabled;                   //  == 4
}

bool isNBTNClicked() {
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET;
}

void startAnimation() {
    for (int frame = 0; frame < FRAME_COUNT; frame++) {

        if (!isSwCorrect()) {
            return;
        }

        flashDiods(frame);

        if (isNBTNClicked()) {
            stopAnimation();
        }
    }
}

void stopAnimation() {
    setRed();
    HAL_Delay(DELAY/2);
    GPIO_PinState state = GPIO_PIN_SET;
    while (state == GPIO_PIN_SET) {
        state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);   //  read nBTN
    }
    setGreen();                                         //  cause can be called only from startAnimation
}

void flashDiods(int frame) {
    setTrios(frame, GPIO_PIN_SET);
    setDuos(frame, GPIO_PIN_SET);

    HAL_Delay(DELAY);

    setTrios(frame, GPIO_PIN_RESET);
    setDuos(frame, GPIO_PIN_RESET);
}

void setTrios(int frame, GPIO_PinState state) {
    if (frame < TRIO_RIGHT_LIMITATION) {
        HAL_GPIO_WritePin(GPIOD, LED[frame], state);
        HAL_GPIO_WritePin(GPIOD, LED[frame + 1], state);
        HAL_GPIO_WritePin(GPIOD, LED[frame + 2], state);
        return;
    }
    if (frame < TRIO_LEFT_LIMITATION) {
        frame = FRAME_COUNT - frame - 2;
        HAL_GPIO_WritePin(GPIOD, LED[frame], state);
        HAL_GPIO_WritePin(GPIOD, LED[frame + 1], state);
        HAL_GPIO_WritePin(GPIOD, LED[frame + 2], state);
    }
}

void setDuos(int frame, GPIO_PinState state) {
    frame = frame < DUO_LEFT_LIMITATION ? frame : FRAME_COUNT - frame;
    HAL_GPIO_WritePin(GPIOD, LED[DIOD_MAX_INDEX - frame], state);
    HAL_GPIO_WritePin(GPIOD, LED[DIOD_MAX_INDEX - frame - 1], state);
}
