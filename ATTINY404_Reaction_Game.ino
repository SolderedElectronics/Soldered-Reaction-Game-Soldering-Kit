/**
 **************************************************
 *
 * @file        ATTINY404_Reaction_Game.ino
 * @brief       Arduino code for Reaction Game
 *              Soldering Kit from Soldered.
 *
 * @note        In order to use this code, install megaTinyCore from https://github.com/SpenceKonde/megaTinyCore
 *              Core version: 2.4.2
 *              Chip: ATtiny404
 *              Clock: 1MHz Internal
 *              millis()/micros Timer: Enabled (default timer)
 *
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#define LED1_RED    PIN_PA4
#define LED2_GREEN  PIN_PA5
#define LED3_BLUE   PIN_PA6
#define LED4_YELLOW PIN_PA7

#define REACTION_TIME_STEP1 50
#define REACTION_TIME_STEP2 10
#define REACTION_TIME_STEP3 5
#define SCORE_BLINK_ON      200
#define SCORE_BLINK_OFF     200

const int pinList[] = {LED1_RED, LED2_GREEN, LED3_BLUE, LED4_YELLOW};
uint8_t score = 0;
uint16_t reactionTime = 1000;
uint16_t conutdown = 0;
uint8_t secretKey;

void setup()
{
    pinsAsOutputs();
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(pinList[i], HIGH);
    }

    showStartAnimation();
}

void loop()
{
    showSequence();
    if (getKeys())
    {
        if (reactionTime > 500)
        {
            reactionTime -= REACTION_TIME_STEP1;
        }
        else if ((reactionTime <= 500) && (reactionTime > 150))
        {
            reactionTime -= REACTION_TIME_STEP2;
        }
        else if ((reactionTime <= 150) && (reactionTime > 5))
        {
            reactionTime -= REACTION_TIME_STEP3;
        }
        score++;
        delay(1500 + random(-750, 750));
    }
    else
    {
        reactionTime = 1000;
        delay(500);
        showFailAnimation();
        delay(500);
        showResult(score);
        delay(500);
        score = 0;
        showStartAnimation();
    }
}

void pinsAsOutputs()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(pinList[i], OUTPUT);
    }
}

void pinsAsInputs()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(pinList[i], INPUT_PULLUP);
    }
}

void showStartAnimation()
{
    int i;
    int j;

    pinsAsOutputs();

    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], LOW);
            delay(100);
        }
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], HIGH);
            delay(100);
        }
    }

    delay(1000);
}

uint8_t getKeys(void)
{
    uint8_t _b;
    int i;
    conutdown = reactionTime;
    pinsAsInputs();
    do
    {
        _b = getButtons();
        conutdown--;
        delay(1);
    } while (_b == 0 && conutdown > 0);

    if (conutdown == 0)
        return 0;

    for (i = 0; i < 4; i++)
    {
        if (_b & (1 << i))
            break;
    }
    if (secretKey != i)
        return 0;
    while (getButtons())
        ;
    return 1;
}

uint8_t calculateNewRandom()
{
    // Randomize it!
    randomSeed(analogRead(6) ^ micros());
    return ((uint8_t)random(0, 255) >> 3) & 0x03;
}

void showSequence(void)
{
    int i;
    secretKey = calculateNewRandom();
    pinsAsOutputs();
    digitalWrite(pinList[secretKey], LOW);
    delay(100);
}


uint8_t getButtons()
{
    uint8_t _buttons = 0;
    pinsAsInputs();

    for (int i = 0; i < 4; i++)
    {
        _buttons |= (~digitalRead(pinList[i]) & 1) << i;
    }
    _buttons &= 0x0f;

    return (_buttons);
}

void showFailAnimation(void)
{
    int i;
    int j;
    pinsAsOutputs();
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], LOW);
        }
        delay(250);
        for (i = 0; i < 4; i++)
        {
            digitalWrite(pinList[i], HIGH);
        }
        delay(250);
    }
}

void showResult(uint8_t _r)
{
    uint8_t _blinks;
    uint8_t i;
    pinsAsOutputs();
    _blinks = _r / 100;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[2], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[2], HIGH);
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r / 10 % 10;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[1], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[1], HIGH);
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r % 10;
    for (i = 0; i < _blinks; i++)
    {
        digitalWrite(pinList[0], LOW);
        delay(SCORE_BLINK_ON);
        digitalWrite(pinList[0], HIGH);
        delay(SCORE_BLINK_OFF);
    }
}
