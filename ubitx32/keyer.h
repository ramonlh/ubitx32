
#pragma once


#pragma once

//#include <avr/pgmspace.h>
#include <pgmspace.h>   //EA4GZI

//27 + 10 + 18 + 1(SPACE) = //56 
const PROGMEM uint8_t cwAZTable[27] = {0b00100100 , 0b01001000 , 0b01001010 , 0b00111000 , 0b00010000, 0b01000010, 0b00111100, 0b01000000 , //A ~ H
0b00100000, 0b01000111 ,0b00111010, 0b01000100, 0b00101100, 0b00101000 , 0b00111110, 0b01000110, 0b01001101, 0b00110100, //I ~ R
0b00110000, 0b00011000, 0b00110010, 0b01000001, 0b00110110, 0b01001001, 0b01001011, 0b01001100};  //S ~ Z
PGM_P pCwAZTable = reinterpret_cast<PGM_P>(cwAZTable);

const PROGMEM uint8_t cw09Table[27] = {0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000001, 0b00000000, 0b00010000, 0b00011000, 0b00011100, 0b00011110};
PGM_P pcw09Table = reinterpret_cast<PGM_P>(cw09Table);

//# : AR, ~:BT, [:AS, ]:SK, ^:KN
const PROGMEM uint8_t cwSymbolIndex[] =  {'.',         ',',        '?',         '"',       '!',         '/',      '(',       ')',        '&',        ':',        ';',         '=',        '+',        '-',        '_',        '\'',       '@',          '#',         '~',        '[',        ']',        '^' };
PGM_P pCwSymbolIndex = reinterpret_cast<PGM_P>(cwSymbolIndex);

const PROGMEM uint8_t cwSymbolTable[]  = {0b11010101, 0b11110011, 0b11001100, 0b11011110, 0b11101011, 0b10100100, 0b10101100, 0b11101101, 0b10010000, 0b11111000, 0b11101010, 0b10100010, 0b10010100, 0b11100001, 0b11001101, 0b11010010,  0b11011010,  0b10010100, 0b10100010, 0b10010000, 0b11000101, 0b10101100};
PGM_P pCwSymbolTable = reinterpret_cast<PGM_P>(cwSymbolTable);
////const PROGMEM uint8_t cwSymbolLength[] = {6,          6,          6,         6,           6,          5,          5,          6,          5,          6,          6,          5,          5,          6,          6,          6,         6,         5,          5,          5,           6,          5};

// ":(Start"),   ':(End "), >: My callsign, <:QSO Callsign (Second Callsign), #:AR, ~:BT, [:AS, ]:SK

byte knobPosition = 0;
//byte cwTextData[30];                        //Maximum 30  Remarked by KD8CE -> Direct Read EEPROM
byte autoCWSendCharEndIndex = 0;
byte autoCWSendCharIndex = 0;
unsigned long autoCWbeforeTime = 0;         //for interval time between chars
byte pttBeforeStatus = 1;                   //PTT : default high
byte isKeyStatusAfterCWStart = 0;           //0 : Init, 1 : Keyup after auto CW Start, 2 : Keydown after
byte selectedCWTextIndex = 0;
unsigned long autoCWKeydownCheckTime = 0;   //for interval time between chars
byte changeReserveStatus = 0;
byte isAutoCWHold = 0;                      //auto CW Pause => Manual Keying => auto

void autoSendPTTCheck()
{
    if (isCWAutoMode == 2) {                           //Sending Mode
        //check PTT Button
        //short Press => reservation or cancel
        //long Press => Hold
        if (digitalRead(PTT) == LOW)
        {
          //if (isKeyStatusAfterCWStart == 0)          //Yet Press PTT from start TX
          //{
          //}
          
          if (isKeyStatusAfterCWStart == 1)            //while auto cw send, ptt up and ptt down again
          {
            //Start Time
            autoCWKeydownCheckTime = millis() + 200;   //Long push time
            isKeyStatusAfterCWStart = 2;               //Change status => ptt down agian
          }
          else if (isKeyStatusAfterCWStart == 2 && autoCWKeydownCheckTime < millis())
          {
            //Hold Mode
            isAutoCWHold = 1;
            isKeyStatusAfterCWStart = 3;
          }
          else if (isKeyStatusAfterCWStart == 3)
          {
            autoCWKeydownCheckTime = millis() + 200;
          }
        }
        else
        {
          //PTT UP
          if (isKeyStatusAfterCWStart == 2)            //0 (down before cw start) -> 1 (up while cw sending) -> 2 (down while cw sending)
          {
            if (autoCWKeydownCheckTime > millis())     //Short : Reservation or cancel Next Text
            {
              if (autoCWSendReservCount == 0 || 
                  (autoCWSendReservCount < AUTO_CW_RESERVE_MAX &&
                autoCWSendReserv[autoCWSendReservCount - 1] != selectedCWTextIndex))
              {
                //Reserve
                autoCWSendReserv[autoCWSendReservCount++] = selectedCWTextIndex;
                changeReserveStatus = 1;
              }
              else if (autoCWSendReservCount > 0 && autoCWSendReserv[autoCWSendReservCount - 1] == selectedCWTextIndex)
              {
                autoCWSendReservCount--;
                changeReserveStatus = 1;
              }
            } // end of Short Key up
          }
          else if (isKeyStatusAfterCWStart == 3)    //play from Hold (pause Auto CW Send)
          {
            isAutoCWHold = 0;
          }

          isKeyStatusAfterCWStart = 1;                        //Change status => ptt up (while cw send mode)
        }     //end of PTT UP  
    }
}

//Send 1 char
void sendCWChar(char cwKeyChar)
{
  byte sendBuff[7];
  byte i, j, charLength;
  byte tmpChar;

  //For Macrofunction
  //replace > and  < to My callsign, qso callsign, use recursive function call
  if (cwKeyChar == '>' || cwKeyChar == '<')
    {
    uint16_t callsignStartIndex = 0;
    uint16_t callsignEndIndex = 0;
    
    if (cwKeyChar == '>') //replace my callsign
      {
      if (conf.userCallsignLength > 0)
        {
        callsignStartIndex = 0;
        callsignEndIndex = conf.userCallsignLength;
        }
      }
    else if (cwKeyChar == '<')  //replace qso callsign
      {
      //ReadLength
      callsignEndIndex = EEPROM.read(CW_STATION_LEN);
      if (callsignEndIndex > 0)
        {
        callsignStartIndex = CW_STATION_LEN - callsignEndIndex - USER_CALLSIGN_DAT;
        callsignEndIndex = callsignStartIndex + callsignEndIndex;
        }
      }

    if (callsignStartIndex == 0 && callsignEndIndex == 0)
      return;

    for (uint16_t i = callsignStartIndex; i <= callsignEndIndex; i++)
      {
      sendCWChar(EEPROM.read(USER_CALLSIGN_DAT + i));
      autoSendPTTCheck(); //for reserve and cancel next CW Text
      if (changeReserveStatus == 1)
        {
        changeReserveStatus = 0;
        updateDisplay(1);
        }
      if (i < callsignEndIndex) delay_background(conf.cwSpeed * 3, 4,0); //
      }
    
    return;
  }
  else if (cwKeyChar >= 'A' && cwKeyChar <= 'Z')  //Encode Char by KD8CEC
  {
    tmpChar = pgm_read_byte(pCwAZTable + (cwKeyChar - 'A'));
    charLength = (tmpChar >> 4) & 0x0F;
    for (i = 0; i < charLength; i++)
      sendBuff[i] = (tmpChar << i) & 0x08;
  }
  else if (cwKeyChar >= '0' && cwKeyChar <= '9')
  {
    charLength = 5;
    for (i = 0; i < charLength; i++)
      sendBuff[i] = (pgm_read_byte(pcw09Table + (cwKeyChar - '0')) << i) & 0x10;
  }
  else if (cwKeyChar == ' ')
  {
    charLength = 0;
    delay_background(conf.cwSpeed * 4, 4,0); //7 -> basic interval is 3
  }
  else if (cwKeyChar == '$')  //7 digit
  {
    charLength = 7;
    for (i = 0; i < 7; i++)
      sendBuff[i] = (0b00010010 << i) & 0x80; //...1..1
  }
  else
  {
    //symbol
    for (i = 0; i < 22; i++)
    {
      if (pgm_read_byte(pCwSymbolIndex + i) == cwKeyChar)
      {
        tmpChar = pgm_read_byte(pCwSymbolTable + i);
        charLength = ((tmpChar >> 6) & 0x03) + 3;
        
        for (j = 0; j < charLength; j++)
          sendBuff[j] = (tmpChar << (j + 2)) & 0x80;

        break;
      }
      else
      {
        charLength = 0;
      }
    }
  }

  for (i = 0; i < charLength; i++)
  {
    cwKeydown();
    delay_background(sendBuff[i] == 0?conf.cwSpeed:conf.cwSpeed * 3, 4,0);
    cwKeyUp();
    if (i != charLength -1)
      delay_background(conf.cwSpeed, 4,0);
  }
}


/**
 CW Keyer
 CW Key logic change with ron's code (ubitx_keyer.cpp)
 Ron's logic has been modified to work with the original uBITX by KD8CEC

 Original Comment ----------------------------------------------------------------------------
 * The CW keyer handles either a straight key or an iambic / paddle key.
 * They all use just one analog input line. This is how it works.
 * The analog line has the internal pull-up resistor enabled. 
 * When a straight key is connected, it shorts the pull-up resistor, analog input is 0 volts
 * When a paddle is connected, the dot and the dash are connected to the analog pin through
 * a 10K and a 2.2K resistors. These produce a 4v and a 2v input to the analog pins.
 * So, the readings are as follows :
 * 0v - straight key
 * 1-2.5 v - paddle dot
 * 2.5 to 4.5 v - paddle dash
 * 2.0 to 0.5 v - dot and dash pressed
 * 
 * The keyer is written to transparently handle all these cases
 * 
 * Generating CW
 * The CW is cleanly generated by unbalancing the front-end mixer
 * and putting the local oscillator directly at the CW transmit frequency.
 * The sidetone, generated by the Arduino is injected into the volume control
 */


// in milliseconds, this is the parameter that determines how long the tx will hold between cw key downs
//#define CW_TIMEOUT (600l)   //Change to CW Delaytime for value save to eeprom
#define PADDLE_DOT 1
#define PADDLE_DASH 2
#define PADDLE_BOTH 3
#define PADDLE_STRAIGHT 4

//we store the last padde's character 
//to alternatively send dots and dashes 
//when both are simultaneously pressed
char lastPaddle = 0;

//reads the analog keyer pin and reports the paddle
byte getPaddle(){
  int paddle = analogRead(ANALOG_KEYER);
  if (paddle > 2800)      return 0;               // above 4v is up
  if (paddle > 2400)      return PADDLE_DASH;     // 4-3v is dot
  else if (paddle > 1050) return PADDLE_DOT;      // 1-2v is dash
  else if (paddle > 700)   return PADDLE_BOTH;    // both are between 1 and 2v
  else                    return PADDLE_STRAIGHT; // less than 1v is the straight key
/**  if (paddle > 800)      return 0;              // above 4v is up
  if (paddle > 600)      return PADDLE_DASH;     // 4-3v is dot
  else if (paddle > 300) return PADDLE_DOT;      // 1-2v is dash
  else if (paddle > 50)  return PADDLE_BOTH;     // both are between 1 and 2v
  else                   return PADDLE_STRAIGHT; // less than 1v is the straight key**/
}

/**
 * Starts transmitting the carrier with the sidetone
 * It assumes that we have called cwTxStart and not called cwTxStop
 * each time it is called, the cwTimeOut is pushed further into the future
 */

void xtone(byte pin, unsigned int freq) { ledcWriteTone(0,conf.sideTone); } // generar tono de freq en el pin
  
void noTone(byte pin) { ledcWriteTone(0,0); }       // parar tono en el pin

void cwKeydown(){
  keyDown = 1;                  //tracks the CW_KEY
  xtone(CW_TONE, (int)conf.sideTone); 
  digitalWrite(CW_KEY, 1);     
  //Modified by KD8CEC, for CW Delay Time save to eeprom
  //cwTimeout = millis() + CW_TIMEOUT;
  conf.cwTimeout = millis() + conf.cwDelayTime * 10;  
}

/** Stops the cw carrier transmission along with the sidetone
 * Pushes the cwTimeout further into the future   */
void cwKeyUp(){
  keyDown = 0;    //tracks the CW_KEY
  noTone(CW_TONE);
  digitalWrite(CW_KEY, 0);    
  
  //Modified by KD8CEC, for CW Delay Time save to eeprom
  //cwTimeout = millis() + CW_TIMEOUT;
  conf.cwTimeout = millis() + conf.cwDelayTime * 10;
}

//Variables for Ron's new logic
#define DIT_L 0x01 // DIT latch
#define DAH_L 0x02 // DAH latch
#define DIT_PROC 0x04 // DIT is being processed
#define PDLSWAP 0x08 // 0 for normal, 1 for swap
#define IAMBICB 0x10 // 0 for Iambic A, 1 for Iambic B
enum KSTYPE {IDLE, CHK_DIT, CHK_DAH, KEYED_PREP, KEYED, INTER_ELEMENT };
static unsigned long ktimer;
unsigned char keyerState = IDLE;

//Below is a test to reduce the keying error. do not delete lines
//create by KD8CEC for compatible with new CW Logic
char update_PaddleLatch(byte isUpdateKeyState) {
  unsigned char tmpKeyerControl = 0;
  int paddle = analogRead(ANALOG_KEYER);
  if (paddle >= conf.cwAdcDashFrom && paddle <= conf.cwAdcDashTo)
    tmpKeyerControl |= DAH_L;
  else if (paddle >= conf.cwAdcDotFrom && paddle <= conf.cwAdcDotTo)
    tmpKeyerControl |= DIT_L;
  else if (paddle >= conf.cwAdcBothFrom && paddle <= conf.cwAdcBothTo)
    tmpKeyerControl |= (DAH_L | DIT_L) ;     
  else 
    {
//    if (Iambic_Key)
    if (conf.cwKeyType>0)
      tmpKeyerControl = 0 ;
    else if (paddle >= conf.cwAdcSTFrom && paddle <= conf.cwAdcSTTo)
      tmpKeyerControl = DIT_L ;
     else
       tmpKeyerControl = 0 ; 
    }
  
  if (isUpdateKeyState == 1)
    keyerControl |= tmpKeyerControl;

  return tmpKeyerControl;
}

/*****************************************************************************
// New logic, by RON
// modified by KD8CEC
******************************************************************************/
void cwKeyer(void){
  lastPaddle=0;
  bool continue_loop = true;
  unsigned tmpKeyControl = 0;
//  if (Iambic_Key ) 
  if (conf.cwKeyType>0)
    {
    while(continue_loop) {
      switch (keyerState) {
        case IDLE:
          tmpKeyControl = update_PaddleLatch(0);
          if ( tmpKeyControl == DAH_L || tmpKeyControl == DIT_L || 
            tmpKeyControl == (DAH_L | DIT_L) || (keyerControl & 0x03)) {
             update_PaddleLatch(1);
             keyerState = CHK_DIT;
          }else{
            if (0 < conf.cwTimeout && conf.cwTimeout < millis()){
              conf.cwTimeout = 0;
              stopTx();
            }
            continue_loop = false;
          }
          break;
        case CHK_DIT:
          if (keyerControl & DIT_L) {
            keyerControl |= DIT_PROC;
            ktimer = conf.cwSpeed;
            keyerState = KEYED_PREP;
          }else{
            keyerState = CHK_DAH;
          }
          break;
        case CHK_DAH:
          if (keyerControl & DAH_L) {
            ktimer = conf.cwSpeed*3;
            keyerState = KEYED_PREP;
          }else{
            keyerState = IDLE;
          }
          break;
        case KEYED_PREP:
          //modified KD8CEC
          /*
          ktimer += millis(); // set ktimer to interval end time
          keyerControl &= ~(DIT_L + DAH_L); // clear both paddle latch bits
          keyerState = KEYED; // next state
          if (!inTx){
            //DelayTime Option
            delay_background(delayBeforeCWStartTime * 2, 2,1);
            
            keyDown = 0;
            cwTimeout = millis() + cwDelayTime * 10;  //+ CW_TIMEOUT;
            startTx(TX_CW, 1);
          }
          */
          if (!inTx){
            //DelayTime Option
            delay_background(conf.delayBeforeCWStartTime * 2, 2,1);
            keyDown = 0;
            conf.cwTimeout = millis() + conf.cwDelayTime * 10;  //+ CW_TIMEOUT;
            startTx(TX_CW, 1);
          }
          ktimer += millis(); // set ktimer to interval end time
          keyerControl &= ~(DIT_L + DAH_L); // clear both paddle latch bits
          keyerState = KEYED; // next state
          cwKeydown();
          break;
        case KEYED:
          if (millis() > ktimer) { // are we at end of key down ?
           cwKeyUp();
           ktimer = millis() + conf.cwSpeed; // inter-element time
            keyerState = INTER_ELEMENT; // next state
          }else if (keyerControl & IAMBICB) {
            update_PaddleLatch(1); // early paddle latch in Iambic B mode
          }
          break;
        case INTER_ELEMENT:   // Insert time between dits/dahs
          update_PaddleLatch(1); // latch paddle state
          if (millis() > ktimer) { // are we at end of inter-space ?
            if (keyerControl & DIT_PROC) { // was it a dit or dah ?
              keyerControl &= ~(DIT_L + DIT_PROC); // clear two bits
              keyerState = CHK_DAH; // dit done, check for dah
            }else{
              keyerControl &= ~(DAH_L); // clear dah latch
              keyerState = IDLE; // go idle
            }
          }
          break;
      }
  
      //Check_Cat(2);
    } //end of while
    }
  else      // not Iambic_Key
    {
    while(1)
      {
      if (update_PaddleLatch(0) == DIT_L) 
        {
          s2("update_PaddleLatch");s2(crlf);
        // if we are here, it is only because the key is pressed
        if (!inTx)
          {
          //DelayTime Option
          delay_background(conf.delayBeforeCWStartTime*2, 2,1);
          keyDown = 0;
          conf.cwTimeout = millis() + conf.cwDelayTime * 10;  //+ CW_TIMEOUT; 
          startTx(TX_CW, 1);
          }
        cwKeydown();
        while (update_PaddleLatch(0) == DIT_L ) { delay_background(1,3,1); }
        cwKeyUp();
        }
      else
        {
        if (0 < conf.cwTimeout && conf.cwTimeout < millis())
          {
          conf.cwTimeout = 0;
          keyDown = 0;
          stopTx();
          }
        return;                   //Tx stop control by Main Loop
        }
      //Check_Cat(2);
      } //end of while
    }   //end of else
}

/*KD8CEC
  When using the basic delay of the Arduino, the program freezes.
  When the delay is used, the program will generate an error because it is not communicating, 
  so Create a new delay function that can do background processing. */
unsigned long delayBeforeTime = 0;
byte delay_background(unsigned delayTime, byte fromType, byte swr)
{ //fromType : 4 autoCWKey -> Check Paddle
  delayBeforeTime = millis();
  while (millis() - delayBeforeTime <= delayTime) 
    {
    if (fromType == 4)
      {
      //CHECK PADDLE
      if (getPaddle() != 0) //Interrupt : Stop cw Auto mode by Paddle -> Change Auto to Manual
        return 1;
      //Check PTT while auto Sending
      autoSendPTTCheck();
      //Check_Cat(3);
      }
    else if (fromType == 3)   // KEYER
      {
      if (millis()-lastswr>swrper)
        {
        SWR=readSWR(1);
        displaySWR(0);
        }
      }
    else
      {
      //Background Work      
     //Check_Cat(fromType);
      }
    if (swr==1)   // SWR on
      {
      if ((((millis()-delayBeforeTime) % 100)>=95) && (((millis()-delayBeforeTime) % 100)<=105))
        {
        SWR=readSWR(1);
        displaySWR(0);
        }
      }
    }
  return 0;
}
