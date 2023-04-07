/*
 * game.h
 *
 *  Created on: Feb 15, 2023
 *      Author: Robert for Soldered.com
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "stm32c0xx_hal.h"
#include "stdint.h"
#include "defines.h"

// Function prototypes

void setPinsAsInputs();
void setPinsAsOutputs();
void showStartAnimation(GPIO_TypeDef *portList[], uint16_t *pinList);
uint8_t calculateNewRandom(ADC_HandleTypeDef *hadc);
uint16_t calculateNewRandomInRange(ADC_HandleTypeDef *hadc, uint16_t range);
uint8_t getKeys(uint16_t conutdown, uint16_t reactionTime, uint8_t secretKey,
		GPIO_TypeDef *portList[], uint16_t *pinList);
uint8_t getButtons(GPIO_TypeDef *portList[], uint16_t *pinList);
void showResult(uint8_t _r, GPIO_TypeDef *portList[], uint16_t *pinList);
void showSequence(ADC_HandleTypeDef *hadc, uint8_t * secretKey,
		GPIO_TypeDef *portList[], uint16_t *pinList);
void showFailAnimation(GPIO_TypeDef *portList[], uint16_t *pinList);

#endif /* INC_GAME_H_ */
