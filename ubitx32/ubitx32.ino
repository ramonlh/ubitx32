 
//Firmware Version
// Standar version - No tasks

#include "defines.h"              
#include "variables.h"                // include 
#include "commontexts.h"              
#include "ajaxcode.h"                  // include
#include <WebServer.h>
#include <Update.h>
#include <NTPClient.h>                // Local
#include "TimeLib.h"                  // Local
#include "FS.h"
#include "FFat.h"
//#include "LittleFS.h"
#include <HTTPClient.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>     // Graphics and font library for ILI9341 driver chip
//#include "lcd_backlight.hpp"
#include "ubitx.h"
#include "eemap.h"
#include <Adafruit_ADS1X15.h>
#include <PubSubClient.h>
#include <WebSocketsServer.h>

#include "esp_wifi.h"
//#include "driver/i2s.h"

#ifdef NTPCLIENT
  WiFiUDP ntpUDP;
#endif

TFT_eSPI tft=TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#ifdef NTPCLIENT
  NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");
#endif

#ifdef WEBSERVER
  WebServer server(webportdefault);
#endif

#ifdef PUBSUB
  //PubSubClient PSclient(espClient);
#endif

extern int enc_read(void);

//When the frequency is moved by the dial, the maximum value by KD8CEC
#define LOWEST_FREQ_DIAL  (3000l)
#define HIGHEST_FREQ_DIAL (60000000l)

//sideTonePitch + sideToneSub = sideTone
byte sideTonePitch=0;
byte sideToneSub = 0;

//DialLock
byte isTxType = 0;    //000000[0 - isSplit] [0 - isTXStop]

#define IAMBICB 0x10 // 0 for Iambic A, 1 for Iambic B
unsigned char keyerControl = IAMBICB;

int shiftDisplayAdjustVal = 0;  //

boolean txCAT = false;        //turned on if the transmitting due to a CAT command
boolean txTFT = false;        //turned on if the transmitting due to a TFT command
byte inTx = 0;                //it is set to 1 if in transmit mode (whatever the reason : cw, ptt or cat)
char keyDown = 0;             //in cw mode, denotes the carrier is being transmitted
byte isIFShift=0;             //1 = ifShift, 2 extend

#include "basicfunctions.h"           
#include "ftp_server.h"
#include "net_f.h"
#include "atu.h"
#include "si5351.h"
#include "ubitx.h"
#include "tft_320240.h"
#include "wifi_f.h"
#include "htmlfunctions.h"            
#include "cat_libs.h"
#include "ui.h"
#include "radio.h"
#include "ws_server.h"
#include "ads1x15_f.h"
#include "keyer.h"
#include "debug.h"
#include "ds18b20.h"
#include "menu.h"
#include "wspr.h"

unsigned countloop=0;

/* The settings are read from FILE  */
void initSettings(){
  memset(spval,0,sizeof(spval));
  if (btnDown())
    {
    ////////////////////////////////////////////////////
    tft.drawString("Reset Factory...",0,40);
    s2("Setting default values");s2(crlf);
    saveconf(); // para iniciar fichero 
    showSettings();
    }
  else
    {
    if (!checkfiles()) s2("Some file is missing");s2(crlf);
    int auxread=readconfEEPROM();
    s2("Read readconf():"); s2(auxread); s2(crlf);
    if(readmemo()!=sizeof(memo)) { savememo(); }
    conf.memMode=0;
    conf.EEip[3]=149;
    }
  if (conf.cwKeyType==1) keyerControl &= ~IAMBICB; else if (conf.cwKeyType==2) keyerControl |= IAMBICB;   
    
  //CW Key ADC Range ======= adjust set value for reduce cw keying error
  //by KD8CEC
  
  //Enable / Diable Check for CW Display Cofiguration Group 
  if ((conf.commonOption0 & 0x80) != 0x00)
    {
    //Adjust CW Mode Freq
    shiftDisplayAdjustVal = (conf.isShiftDisplayCWFreq & 0x3F) * 10;
    //check Minus
    if ((conf.isShiftDisplayCWFreq & 0x40) == 0x40)
      shiftDisplayAdjustVal = shiftDisplayAdjustVal * -1;
   //Shift Display Check (Default : 0)
    conf.isShiftDisplayCWFreq = (conf.isShiftDisplayCWFreq & 0x80) == 0?1:0;
    }
   //Stored IF Shift Option
  if ((conf.commonOption0 & 0x40) != 0x00)
    {
    isIFShift = conf.ifShiftValue != 0;
    }

  //default Value (for original hardware)
  if (conf.cwAdcSTFrom >= conf.cwAdcSTTo)
    {
    conf.cwAdcSTFrom = 0;
    conf.cwAdcSTTo = 50;
    }

  if (conf.cwAdcBothFrom >= conf.cwAdcBothTo) { conf.cwAdcBothFrom = 701; conf.cwAdcBothTo = 900;  }
  if (conf.cwAdcDotFrom >= conf.cwAdcDotTo) { conf.cwAdcDotFrom = 901; conf.cwAdcDotTo = 1300; }
  if (conf.cwAdcDashFrom >= conf.cwAdcDashTo) { conf.cwAdcDashFrom = 1800; conf.cwAdcDashTo = 2300; }
  //end of CW Keying Variables
  
  if (conf.cwDelayTime < 1 || conf.cwDelayTime > 250)  conf.cwDelayTime = 60;
//  if (conf.vfoA_mode < 2) conf.vfoA_mode = 2;   // LSB default
//  if (conf.isUSBB < 2) conf.isUSBB = 3;   // USB default
  
  //original code with modified by kd8cec
  if (conf.usbCarrier > 11060000l || conf.usbCarrier < 11048000l)
    conf.usbCarrier = 11056000l;   // EA4GZI
  if (conf.cwmCarrier > 11060000l || conf.cwmCarrier < 11048000l)
    conf.cwmCarrier = 11056000l;    // EA4GZI
  if (conf.frequencyA > 35000000l || 3500000l > conf.frequencyA) {
     conf.frequencyA = 7150000l;
     conf.isUSBA = 0; //LSB
    }
  if (conf.frequencyB > 35000000l || 3500000l > conf.frequencyB) {
    conf.frequencyB = 14150000l;  
    conf.isUSBB = 1; //USB
    }
  //end of original code section
  
  if (conf.sideTone < 100 || 2000 < conf.sideTone) conf.sideTone = 800;
  if (conf.cwSpeed < 10 || 1000 < conf.cwSpeed) conf.cwSpeed = 100;
  if (conf.sideTone < 300 || conf.sideTone > 1000) {
    sideTonePitch = 0;
    sideToneSub = 0;;
    }
  else{
    sideTonePitch = (conf.sideTone - 300) / 50;
    sideToneSub = conf.sideTone % 50;
    }
  initButtons();
}

void initPorts(){
  pinMode(ENC_A, INPUT_PULLUP);   // en ESP32 necesita pullup externo
  pinMode(ENC_B, INPUT_PULLUP);   // en ESP32 necesita pullup externo
  pinMode(FBUTTON, INPUT_PULLUP); // en ESP32 necesita pullup externo
  pinMode(PTT, INPUT_PULLUP);     // en ESP32 necesita pullup externo
  pinMode(ANALOG_KEYER, INPUT);   // Keyer

  pinMode(CW_TONE, OUTPUT);    digitalWrite(CW_TONE, 0);
  pinMode(TX_RX,OUTPUT);       digitalWrite(TX_RX, 0);
  pinMode(TX_LPF_A, OUTPUT);   digitalWrite(TX_LPF_A, 0);
  pinMode(TX_LPF_B, OUTPUT);   digitalWrite(TX_LPF_B, 0);
  pinMode(TX_LPF_C, OUTPUT);   digitalWrite(TX_LPF_C, 0);
  pinMode(CW_KEY, OUTPUT);     digitalWrite(CW_KEY, 0);
}

void task1() {
  tini=millis();
  readVIpower();
  countfaulttime++;   // si se hace mayor que TempDesactPrg,desactiva ejecucion programas dependientes de fecha
  if (inTx==1) {
    leevaloresOW();
    //sendTemp();
    }
  if ((tftpage!=21) && (tftpage!=22) &&(tftpage!=23))
    displayStatus();
  if(conf.rstper>0) { if(millis() > 3600000*conf.rstper) { Serial2.println("RESTART"); ESP.restart();  } }
  sendwsData(tcpvtotvalue);
  sendwsData(tcpitotvalue);
  sendwsData(tcpfechahoravalue);
  mact1=millis();
}

void handleWebclient() { server.handleClient(); }

void initFFat(boolean testfiles, boolean format) {
  s2("FFat");s2(b);
  if (FFat.begin(format)) { s2("OK\n"); } else  { s2("ERROR\n"); }
    size_t total = FFat.totalBytes();
  size_t used = FFat.usedBytes();
  Serial2.printf("FFat total: %u bytes, usados: %u bytes, libres: %u bytes\n",
                total, used, total - used);
  if (testfiles) {
    File dir=FFat.open("/");
    File file=dir.openNextFile();
    while(file) 
      {
      s2(b); s2(file.name()); s2(b); s2(file.size());s2(crlf); 
      file.close();
      file=dir.openNextFile(); 
      }
  }
}

void execcomdebug() {
  char command=cinput;
  String param;
  param=sinput.substring(sinput.indexOf(",")+1);
  boolean done=false;
  if (command=='a') { conf.autoWiFi=conf.autoWiFi==0?1:0; done=true; }
  else if( command=='f') {
    File dir=SPIFFS.open(barra);
    File file=dir.openNextFile();
    while(file) { s2(file.name()); s2(b); s2(file.size()); s2(crlf); file=dir.openNextFile(); }
    s2("files ");
    s2(checkfiles()?"OK":"ERROR"); s2(crlf);} 
  else if (command=='F') { initFab(); }
  else if (command=='h') { printhelp(); } 
  else if (command=='p') { param.toCharArray(conf.passSTA,20); done=true; }
  else if (command=='P') { param.toCharArray(conf.passAP,20); done=true; }
  else if (command=='r') { ESP.restart(); }
  else if (command=='s') { param.toCharArray(conf.ssidSTA,20); done=true; }
  else if (command=='S') { param.toCharArray(conf.ssidAP,20); done=true; }
  else if (command=='t') { SPIFFS.format(); }
  else if (command=='u') { printstatus(); } 
  else if (command=='w') { if ((param.toInt()>=0) &&(param.toInt()<=3)) { conf.wifimode=param.toInt(); done=true; }}
  else { s2("Command not found"); s2(crlf); printhelp(); }
  if (done) { saveconf(); s2("Done"); s2(crlf); printstatus(); }
}

void handleSerial()
{
  if (Serial2.available()) 
    {
    boolean ejec=false;
    char tChar = Serial2.read(); 
    if (tChar=='\r')          // si es #13, CR
      {
      char tCharTrash=Serial2.read();       // leemos #10 y ejecutamos
      ejec=true;
      }
    else if (tChar=='\n') 
      { ejec=true; }    // si es #10, hemos terminado y ejecutamos
    else                    // si no es #10, lo añadimos
      {
      if (cinput=='\0') { cinput=tChar; }   // nuevo comando
      else { sinput=sinput+tChar; }         // añadir al parámetro
      } 
    if (ejec)
      {
      if (conf.serial2Mode==0)    // modo debug
        execcomdebug();
      else
        {
        if ((conf.connMode==1) || (conf.connMode==2) || (conf.connMode==3))  // modo IP or Serial2-IP, mod Manager
          {
          //handleRecDataIP(cinput, sinput);  
          }
        }
      sinput=""; cinput='\0';  
      }
    }
}

void initTone()
{
  ledcAttachChannel(CW_TONE, conf.sideTone, 12, 0);
  //ledcSetup(0,conf.sideTone,12);    Espressiff changed
  //ledcAttachPin(CW_TONE,0);   Espressiff changed
}

void initI2C()
{
  Wire.begin(SDA,SCL);
}

void setup()
{
  initSerial2(115200);  
  EEPROM.begin(EEPROM_SIZE);

  initConf();
  conf.connMode=0;
  s2("========== Init =========");s2(crlf);
  Init_Cat(38400, SERIAL_8N1);  s2("  Serial 1 started\n");
  s2("  Serial 2 started\n");
  s2("  Vers.:"); s2(FIRMWARE_VERSION_INFO); s2(crlf);
  delay(10);
  initTFT();          s2("TFT started");s2(crlf);
  DisplayVersionInfo(FIRMWARE_VERSION_INFO);
  initFFat (true,true);  
  
  initPorts();        s2("Ports OK\n");  
  initSettings();    
  conf.attLevel=0;      //
  conf.ifShiftValue=0;   //
  byte auxconnMode=conf.connMode;
  conf.connMode=0;
  //showSettings();

  initI2C();          s2("I2C started\n");;
  initOscillators();  s2("Oscillators started\n");s2(crlf);
  initTone();         s2("Pin Tone OK\n");s2(crlf);
  setFrequency(conf.frequency);  
  initDS18B20(); 
  leevaloresOW();  
  initADS();          s2("ADS1115 started\n");
  initATU();          s2("ATU started\n");
  updateDisplay(1);
  initWiFi();     

  conf.wsenable=1;    
  initNetServices();

  smetervalue=0;  maxsmeter=0;  minsmeter=32000;
  int8_t power;
  int auxerr=esp_wifi_get_max_tx_power(&power);
  s2(F("END SETUP\n"));
  s2("============================\n");
  s2(F("Type 'h' to help\n"));

  s2("----------------------------\n");
  conf.connMode=auxconnMode;

    // Crear tarea FTP
  xTaskCreate(ftp_task, "ftp_task", 10000, NULL, 1, NULL);
}

void task01() {
  tini=millis();
  if ((inTx==0) && (tftpage==0) && (conf.framemode<=1))
    {
    if (conf.framemode==0) 
      {
      readSmeter();
      sendwsData(tcpsmeter);
      displaySmeter(190,210,50,1);
      }
    else if (conf.framemode==1) 
      {   
      readSmeter();   // valores de 0 a 90
      displaybarSmeter(40,186,0,90,69);
      }
    }
  if (tftpage==23) { updateDisplay(0); }

  mact01=millis();
}

void task10() {
  tini=millis();
  if (inTx==0) leevaloresOW();
  //sendTemp();
  displayWiFiSt();
  mact10=millis();
}

void loop()
{ 
  countloop++;  
  tini=millis();
  handleSerial();  

  if (conf.webenable) handleWebclient();
  if (conf.wsenable) wsserver.loop(); 

  if (isCWAutoMode==0)    //when CW AutoKey Mode, disable this process
    {
    if ((!txCAT) && (!txTFT)) { checkPTT(); }  
    handletfttouch();     // este proceso dura unos 11 ms
    }
  if (conf.cwMode!=0) 
    { 
    //decodeCW();
    cwKeyer(); 
    }

  if (tftpage==0)
    {
    readSmeter();     // este proceso dura unos 9 ms
    if (inTx==1)  // TX
      { 
      SWR=readSWR(1); 
      displaySWR(0); 
      }
    else    // RX
      {
      if (conf.memMode==0)
        {
        if (isCWAutoMode == 0 || cwAutoDialType == 1)
          {
          if (scanF>0) 
            { 
            doScanF();  
            readSmeter();   // valores de 0 a 90
            displayFrame();
            if (conf.scanmode>0)
              {
              if (smetervalue>conf.squelchval) { setSCAN(0); }
              }
            }
          else
            {
            if (conf.ritOn) {  doRIT();    }
            else 
              { 
              if (keylock==0) 
                {
                doTuningWithThresHold();   // este proceso dura unos 10 ms  
                }
              }
            }
          }
        }
      } //end of check TX Status
    //we check CAT after the encoder as it might put the radio into TX
    Check_Cat(inTx?1:0); // s2("Check_Cat");s2(crlf);
    }
  else if (tftpage==22) 
    { doMem(); }    // Mem Mode
  if((millis() > (mact01 + 100))) { task01(); }                     // tareas que se hacen cada 0.1 segundo
  if((millis() > (mact1 + 1000))) { task1(); }                      // tareas que se hacen cada segundo
  if((millis() > (mact10 + 10000))) { task10(); }                   // tareas que se hacen cada 10 segundos
////////////////////////////////////////////
    // RemoteDebug handle
}
