/*
 * game.c
 *
 *  Created on: Feb 15, 2023
 *      Author: Robert for Soldered.com
 */

#include "game.h"

/**
 * @brief			Function which sets the four GPIO pins as inputs
 *
 * @returns			None
 */
void setPinsAsInputs() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/*Configure GPIO pin : LED_1_BLUE_Pin */
	GPIO_InitStruct.Pin = LED_1_BLUE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_1_BLUE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_2_YELLOW_Pin LED_3_GREEN_Pin LED_4_RED_Pin */
	GPIO_InitStruct.Pin = LED_2_YELLOW_Pin | LED_3_GREEN_Pin | LED_4_RED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
 * @brief			Function which sets the four GPIO pins as outputs
 *
 * @returns			None
 */
void setPinsAsOutputs() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/*Configure GPIO pin : LED_1_BLUE_Pin */
	GPIO_InitStruct.Pin = LED_1_BLUE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_1_BLUE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_2_YELLOW_Pin LED_3_GREEN_Pin LED_4_RED_Pin */
	GPIO_InitStruct.Pin = LED_2_YELLOW_Pin | LED_3_GREEN_Pin | LED_4_RED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
 * @brief			Function which shows the animation which plays at game start
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 *
 * @returns			None
 */
void showStartAnimation(GPIO_TypeDef *portList[], uint16_t *pinList) {
	int i;
	int j;

	setPinsAsOutputs();

	for (j = 0; j < 2; j++) {
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_RESET);
			HAL_Delay(100);
		}
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_SET);
			HAL_Delay(100);
		}
	}
	HAL_Delay(1000);
}

/**
 * @brief			Function which shows current game sequence
 *
 * @params			ADC_HandleTypeDef *hadc: pointer to ADC object
 *
 * @params			uint8_t *secretKey: Selector of the next button which needs to be pressed
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 *
 * @returns			None
 */
void showSequence(ADC_HandleTypeDef *hadc, uint8_t *secretKey,
		GPIO_TypeDef *portList[], uint16_t *pinList) {

	uint8_t newRandom = calculateNewRandom(hadc);
	*secretKey = newRandom;
	setPinsAsOutputs();
	HAL_GPIO_WritePin(portList[newRandom], pinList[newRandom], GPIO_PIN_RESET);

	HAL_Delay(50);

	// Reset pins
	for (int i = 0; i < 4; i++) {
		HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_SET);
	}
}

/**
 * @brief			Calculate a random number in range 0-3
 *
 * @params			ADC_HandleTypeDef *hadc: Pointer to ADC object to read noise from ADC to generate a random number
 *
 * @returns			A uint8_t of values 0, 1, 2 or 3, generated randomly
 */
uint8_t calculateNewRandom(ADC_HandleTypeDef *hadc) {

	uint16_t seed;
	uint8_t temp;
	int i;
	//Get the data
	for (i = 0; i < 25; i++) {
		seed ^= HAL_ADC_GetValue(hadc);
	}
	//Shuffle the seed
	seed = 2053 * seed + 13849;
	//XOR two bytes
	temp = seed ^ (seed >> 8);
	//XOR two nibbles
	temp ^= (temp >> 4);
	//XOR two pairs of bits and return remainder after division by 4
	return (temp ^ (temp >> 2)) & 0b00000011;
}

/**
 * @brief			Calculate a random number in range 0-3
 *
 * @params			ADC_HandleTypeDef *hadc: Pointer to ADC object to read noise from ADC to generate a random number
 *
 * @params			uint16_t range: the maximum value to generate
 *
 * @note			This function works on the principle of dividing the 0 to max range into 0xFFFF parts, and then randomly selects one of those.
 *
 * @returns			A uint16_t generated randomly
 */
uint16_t calculateNewRandomInRange(ADC_HandleTypeDef *hadc, uint16_t range) {
	uint16_t seed;
	uint16_t temp;
	int i;
	//Get the data
	for (i = 0; i < 25; i++) {
		seed ^= HAL_ADC_GetValue(hadc);
	}
	//Shuffle the seed
	seed = 2053 * seed + 13849;
	//XOR two bytes
	temp = seed ^ (seed >> 16);
	//XOR two nibbles
	temp ^= (temp >> 8);
	//XOR four pairs of bits
	temp = (temp ^ (temp >> 4));

	// Return random number in range
	return (uint16_t) (range * ((float) temp / (float) 0xFFFF));
}

/**
 * @brief			Read the inputted keys from the buttons
 *
 * @params			uint16_t conutdown: How long has the game been measuring time in this round
 *
 * @params			uint16_t reactionTime: Total reaction time of the player in the game
 *
 * @params			uint8_t *secretKey: Selector of the next button which needs to be pressed
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 *
 * @returns			uint8_t The result of the current round (success or fail)
 */
uint8_t getKeys(uint16_t conutdown, uint16_t reactionTime, uint8_t secretKey,
		GPIO_TypeDef *portList[], uint16_t *pinList) {
	uint8_t _b;
	int i;
	conutdown = reactionTime;
	setPinsAsInputs();
	do {
		_b = getButtons(portList, pinList);
		conutdown--;
		HAL_Delay(1);
	} while (_b == 0 && conutdown > 0);

	if (conutdown == 0)
		return 0;

	for (i = 0; i < 4; i++) {
		if (_b & (1 << i))
			break;
	}
	if (secretKey != i)
		return 0;
	while (getButtons(portList, pinList))
		;
	return 1;
}

/**
 * @brief			Read the button states
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
uint8_t getButtons(GPIO_TypeDef *portList[], uint16_t *pinList) {
	uint8_t _buttons = 0;
	setPinsAsInputs();

	for (int i = 0; i < 4; i++) {
		int pinState = HAL_GPIO_ReadPin(portList[i], pinList[i]);
		_buttons |= !pinState << i;
	}
	_buttons &= 0x0f;

	return (_buttons);
}

/**
 * @brief			Show the game over animation
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
void showFailAnimation(GPIO_TypeDef *portList[], uint16_t *pinList) {
	int i;
	int j;
	setPinsAsOutputs();
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_RESET);
		}
		HAL_Delay(250);
		for (i = 0; i < 4; i++) {
			HAL_GPIO_WritePin(portList[i], pinList[i], GPIO_PIN_SET);
		}
		HAL_Delay(250);
	}
}

/**
 * @brief			Show the game result
 *
 * @params			uint8_t _r: Current result of the game
 *
 * @params			GPIO_TypeDef * portList[]: Array of ports which are used for GPIO
 *
 * @params			uint16_t * pinList: Array of pins which are used for GPIO
 */
void showResult(uint8_t _r, GPIO_TypeDef * portList[], uint16_t  * pinList) {
	uint8_t _blinks;
	uint8_t i;
	setPinsAsOutputs();
	_blinks = _r / 100;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[2], pinList[2], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[2], pinList[2], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_OFF);
	}

	_blinks = _r / 10 % 10;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[1], pinList[1], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[1], pinList[1], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_OFF);
	}

	_blinks = _r % 10;
	for (i = 0; i < _blinks; i++) {
		HAL_GPIO_WritePin(portList[0], pinList[0], GPIO_PIN_SET);
		HAL_Delay(SCORE_BLINK_ON);
		HAL_GPIO_WritePin(portList[0], pinList[0], GPIO_PIN_RESET);
		HAL_Delay(SCORE_BLINK_OFF);
	}
}

