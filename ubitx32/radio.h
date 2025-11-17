
#pragma once

byte getIndexHambanBbyFreq(unsigned long f)
{
  for (byte i = 0; i<conf.useHamBandCount; i++)
    if ((conf.hamBandRange[i][0]*1000 <= f) && (f <= conf.hamBandRange[i][1]*1000))
      return i;
  return 99;
}

void setFrequency(unsigned long f){
  //setTXFilters(f);
  //alternative to reduce the intermod spur
  IF1=conf.firstIF;
  if (conf.isUSB==1)
    {
    OSC1 = IF1 + conf.usbCarrier;
    if (conf.cwMode) { OSC2 = IF1 + f + conf.sideTone; }
    else             { OSC2 = IF1 + f; }
    }
  else
    {       // LSB
    OSC1 = IF1 - conf.usbCarrier;
    if (conf.cwMode) { OSC2 = IF1 + f + conf.sideTone;  }
    else             { OSC2 = IF1 + f;    }
    }
  si5351bx_setfreq(2, OSC2); 
  si5351bx_setfreq(1, OSC1);
  tini=millis();
  conf.frequency=f;
  conf.actualBand=getIndexHambanBbyFreq(f);
  if (conf.actualBand != 99) 
    conf.freqbyband[conf.actualBand][conf.vfoActive==VFO_A?0:1]=f; 
  if (conf.vfoActive==VFO_A) conf.frequencyA=f; else conf.frequencyB=f;  
  sendwsData(tcpfrequencyA);
  sendwsData(tcpfrequencyB);
  if (scanF==0)
    {
    if (!readingspectrum)
      {
      //sendFreq();
      //saveconf();
      }
    }
  else 
    {
    //sendFreq();
    }
}


void setSSB(byte value) 
  {
  if (value==conf.isUSB)  return;
  conf.isUSB=value; 
  sendwsData(tcpisUSB); 
  if (conf.vfoActive==VFO_A) {
    conf.isUSBA=conf.isUSB; 
    }
  else {
    conf.isUSBB=conf.isUSB;
    }
  btMainact[4]=conf.isUSB==1?1:0; 
  strcpy(btMaintext[4],conf.isUSB==1?"USB":"LSB"); 
  displayMain(); 
  setFrequency(conf.frequency);
  displayFreq(1,1,1,1); 
}
  

/* startTx is called by the PTT, cw keyer and CAT protocol to
 * put the uBitx in tx mode. It takes care of rit settings, sideband settings
 * Note: In cw mode, doesnt key the radio, only puts it in tx mode */
 

// when Band change step = just hamband,  moveDirection : 1 = next, -1 : prior
void setNextHamBandFreq(unsigned long f, int moveDirection)
{
  unsigned long resultFreq = 0;
  if (conf.actualBand == 99) 
    {
    conf.actualBand=3; 
    }   //out of hamband
  else if (moveDirection==1)
    {
    if (conf.actualBand < conf.useHamBandCount-1)
      conf.actualBand++;
    else
      conf.actualBand=0;
    }
  else if (moveDirection==-1)
    {
    if (conf.actualBand > 0)
      conf.actualBand--;
    else
      conf.actualBand=conf.useHamBandCount-1;
    }
  else
  {
    conf.actualBand = 99;
  }
  resultFreq = resultFreq & 0x1FFFFFFF;
  if ((resultFreq/1000) < conf.hamBandRange[conf.actualBand][0] || (resultFreq / 1000) > conf.hamBandRange[conf.actualBand][1])
    resultFreq = (unsigned long)(conf.hamBandRange[conf.actualBand][0]) * 1000;
  if (conf.actualBand != 99)
    {
    setFrequency(conf.freqbyband[conf.actualBand][conf.vfoActive==VFO_A?0:1]);
    }
  else
  {
    setFrequency(resultFreq);
  }
    
  if (conf.autoMode==1) { setSSB(resultFreq>=10000000?1:0); }

  displayFreq(0,1,1,1);
  saveconf();
    sendwsData(tcpfrequencyA);
  sendwsData(tcpfrequencyB);
}

void startTx(byte txMode, byte isDisplayUpdate){
  //Check Hamband only TX //Not found Hamband index by now frequency
  tftpage=0;
  unsigned long auxfreq;
/**  if ((isTxType & 0x01) != 0x01)
    {
    digitalWrite(TX_RX, 1);
    }
  inTx = 1;**/
  
  if (conf.ritOn)
    {
    //save the current as the rx frequency
    conf.ritRxFrequency = conf.frequency;
    auxfreq=conf.ritTxFrequency;
//    setFrequency(conf.ritTxFrequency);
    }
  else 
    {
    if (conf.splitOn == 1) { setVFO(conf.vfoActive==VFO_A?VFO_B:VFO_A); }
    auxfreq=conf.frequency;
    } //end of else
  byte auxf=getIndexHambanBbyFreq(auxfreq);
  if (conf.TXall==0)    // TX only ham bands
    if ((auxfreq<conf.hamBandRange[auxf][0]*1000) || (auxfreq>conf.hamBandRange[auxf][1]*1000)) 
      {
      tftErrormsg("OUT OF BAND","Modify parameter","TX range");
      //tftpage=2;
      return;
      } 
  if ((isTxType & 0x01) != 0x01) { digitalWrite(TX_RX, 1); }
  inTx = 1;
  
  setFrequency(auxfreq);
  SetCarrierFreq();
  if (txMode == TX_CW)
    {
    //turn off the second local oscillator and the bfo
    OSC0 = 0;  OSC1 = 0;
    si5351bx_setfreq(0, OSC0);
    si5351bx_setfreq(1, OSC1);

    //shif the first oscillator to the tx frequency directly
    //the key up and key down will toggle the carrier unbalancing
    //the exact cw frequency is the tuned frequency + sidetone
    if (conf.cwMode == 0)
      {
      if (conf.isUSB==1)
        OSC2 = conf.frequency + conf.sideTone;
      else
        OSC2 = conf.frequency - conf.sideTone;
      }
    else if (conf.cwMode == 1) //CWL
      {
      OSC2 = conf.frequency - conf.sideTone;
      }
    else  //CWU
      {
      OSC2 = conf.frequency + conf.sideTone;
      }
    si5351bx_setfreq(2, OSC2);
  }
  //reduce latency time when begin of CW mode
  if (isDisplayUpdate == 1)
    updateDisplay(1);
}

void stopTx(void){
  inTx = 0;
  digitalWrite(TX_RX, 0);           //turn off the tx
  SetCarrierFreq();
  if (conf.ritOn)
    setFrequency(conf.ritRxFrequency);
  else
    {
    if (conf.splitOn == 1) { setVFO(conf.vfoActive==VFO_A?VFO_B:VFO_A); }
    setFrequency(conf.frequency);
    } //end of else
  updateDisplay(1);
}


/** Basic User Interface Routines. These check the front panel for any activity */

/**
 * The PTT is checked only if we are not already in a cw transmit session
 * If the PTT is pressed, we shift to the ritbase if the rit was on
 * flip the T/R line to T and update the display to denote transmission */

void checkPTT(){  
  //we don't check for ptt when transmitting cw
  //if (conf.cwTimeout > 0) return;
  if (digitalRead(PTT) == 0 && inTx == 0)
    {
    startTx(TX_SSB, 1);  
    delay(50); //debounce the PTT
    }
  if (digitalRead(PTT) == 1 && inTx == 1)
    {
    stopTx(); 
    }
}

void checkButton(){
 //only if the button is pressed
 int keyStatus = getBtnStatus();
 if (keyStatus == -1) return;
// delay(50);
 keyStatus = getBtnStatus();   //will be remove 3 lines
 if (keyStatus == -1) return;
 Check_Cat(0);
}

/************************************
Replace function by KD8CEC
prevent error controls applied Threshold for reduct errors,  dial Lock, dynamic Step
 *************************************/
byte threshold = 2;  //noe action for count
unsigned long lastEncInputtime = 0;
int encodedSumValue = 0;
byte lastMovedirection = 0;     //0 : stop, 1 : cw, 2 : ccw
#define encodeTimeOut 1000

void doScanF()
{
  if (scanF==1)   // down
    {
    conf.frequency -= (conf.arTuneStep[conf.tuneStepIndex]);
    if (conf.scanallf==0)
      if (conf.frequency<conf.hamBandRange[conf.actualBand][0]*1000)
        conf.frequency=conf.hamBandRange[conf.actualBand][1]*1000;
    }
  else if (scanF==2)     // up
    {
    conf.frequency += (conf.arTuneStep[conf.tuneStepIndex]);
    if (conf.scanallf==0)
      if (conf.frequency>conf.hamBandRange[conf.actualBand][1]*1000)
        conf.frequency = conf.hamBandRange[conf.actualBand][0]*1000;
    }
  setFrequency(conf.frequency);
  displayFreq(1,1,0,0);
}

void saveVFOtoMem()   
{
  int i=0;
  while ((i<maxMem) && (memo.act[i]==1)) i++;
  if (i<maxMem)
    {
    strcpy(memo.descr[i], "");
    int auxI=getCharTFT(memo.descr[i],10); 
    if (auxI !=-1) 
      {
      strcpy(memo.descr[i],auxtft);  
      memo.act[i]=1;
      memo.vfoActive[i]=conf.vfoActive;
      memo.isUSB[i]=conf.isUSB;
      memo.cwMode[i]=conf.cwMode;
      memo.ritOn[i]=conf.ritOn;
      memo.splitOn[i]=conf.splitOn;
      memo.frequency[i]=conf.frequency;
      memo.ritTxFrequency[i]=conf.ritOn==1?conf.ritTxFrequency:conf.frequency;
      memo.cwModespl[i]=conf.cwModeB;
      memo.ftxspl[i]=conf.frequencyB;
      savememo();
      }
    }
}


void setRIT(byte value) 
  { 
  conf.ritOn=value; 
  btMainact[6]=value;
  sendwsData(tcpritOn); 
  if (conf.ritOn==1) 
    { 
    setSPLIT(0); 
    conf.ritTxFrequency = conf.frequency;
    }
  displayMain();
  displayFreq(1,1,1,1);
  }

void setFrecuencyB(unsigned long f) {
  conf.frequencyB = f;
  sendwsData(tcpfrequencyB);
}

void setSPLIT(byte value) 
  { 
  conf.splitOn=value;
  btMainact[7]=value;
  sendwsData(tcpsplitOn); 
  if (conf.splitOn==1) 
    { 
    setRIT(0); 
    setFrequency(conf.ritTxFrequency);
    }
  displayMain();
  displayFreq(1,1,1,1);
  }

void setCW(byte value) 
  { 
  conf.cwMode=value; 
  sendwsData(tcpcwMode); 
  if (conf.vfoActive==VFO_A) conf.cwModeA=conf.cwMode; else conf.cwModeB=conf.cwMode;
  btMainact[5]=(conf.cwMode>0);
  displayMain(); 
  }

void setVFO(byte value) 
  { 
  conf.vfoActive=value; 
  sendwsData(tcpvfoActive); 
  if (conf.vfoActive==VFO_A) 
    {
    conf.isUSB=conf.isUSBA; 
    conf.frequency = conf.frequencyA;
    }
  else 
    {
    conf.isUSB=conf.isUSBB;
    conf.frequency = conf.frequencyB;
    }
  setSSB(conf.vfoActive==VFO_A?conf.isUSBA:conf.isUSBB);
  setCW(conf.vfoActive==VFO_A?conf.cwModeA:conf.cwModeB);
  setFrequency(conf.frequency);
  displayFreq(1,1,1,1); 
  }

void setSCAN(byte value)
{
  if (value==scanF) return;
  scanF=value; 
  displayFlot();
}


void setMEMtoVFO(int pos)
{
  if (memo.act[pos]==1)
    {
    setVFO(memo.vfoActive[pos]);
    setSSB(memo.isUSB[pos]);
    setCW(memo.cwMode[pos]);
    setRIT(memo.ritOn[pos]);
    setSPLIT(memo.splitOn[pos]);
    conf.frequency=memo.frequency[pos];
    conf.ritTxFrequency=memo.ritTxFrequency[pos];
    conf.cwModeB=memo.cwModespl[pos]=0;
    conf.frequencyB=memo.ftxspl[pos];
    setFrequency(conf.frequency);
    conf.lastmempos=pos;
    saveconf();
    }
}

void setFreq(int s)
{
  conf.frequency += (conf.arTuneStep[conf.tuneStepIndex] * s);  //applied weight (s is speed) //if want need more increase size, change step size
  if (conf.vfoActive==VFO_A) conf.frequencyA=conf.frequency; else conf.frequencyB=conf.frequency;
  setFrequency(conf.frequency);
  displayFreq(1,1,1,1);
}

void doTuningWithThresHold(){
  int s=0;
  //unsigned long prev_freq;
  s = enc_read();
  //if time is exceeded, it is recognized as an error, ignore exists values, because of errors
  if (s == 0) {
    if (encodedSumValue != 0 && (millis() - encodeTimeOut) > lastEncInputtime)
      encodedSumValue = 0;
    lastMovedirection = 0;
    return;
    }
  lastEncInputtime = millis();
  lasttimeknob=millis();        // reset temp to read spectrum
  //for check moving direction
  encodedSumValue += (s > 0 ? 1 : -1);
  //check threshold and operator actions (hold dial speed = continous moving, skip threshold check)
  //not use continues changing by Threshold
  if (((encodedSumValue *  encodedSumValue) <= (threshold * threshold)))
    return;
  //Valid Action without noise
  encodedSumValue = 0;
  setFreq(s);
  //conf.frequency += (conf.arTuneStep[conf.tuneStepIndex] * s);  //applied weight (s is speed) //if want need more increase size, change step size
  //if (conf.vfoActive==VFO_A) conf.frequencyA=conf.frequency; else conf.frequencyB=conf.frequency;
  //setFrequency(conf.frequency);
  //displayFreq(1,1,1,1);
}


void doMem()
{
  if (getBtnStatus()==1) 
    {
    if (memo.act[mempos]==1)
      {
      tftpage=0;
      conf.memMode=0;
      updateDisplay(1);
      return;
      }
    }
  boolean cambio=false;
  int knob = enc_read();
  if (knob < -1) 
    {
    if (memlin>0) memlin--;
    if (mempos>0) { mempos--; cambio=true; }
    }
  else if (knob > 1) 
    {
    if (memlin<5) memlin++;
    if (mempos<maxMem) { mempos++; cambio=true; }
    }
  Serial2.println(mempos);
  if ((cambio) || (firstmem))
    { 
    firstmem=false;
    displayFreq(0,1,1,1);
    displayYN(1,1,0);
    displayMemList(); 
    setMEMtoVFO(mempos); 
    }
}

/* RIT only steps back and forth by 100 hz at a time */
void doRIT(){
  int knob = enc_read();
  unsigned long old_freq = conf.frequency;
  if (conf.tuneStepIndex<6) setSTEP(6);
  if (knob < 0) 
    conf.frequency -= (conf.arTuneStep[conf.tuneStepIndex]);  
  else if (knob > 0) 
    conf.frequency += (conf.arTuneStep[conf.tuneStepIndex]);  
  if (conf.frequency>conf.ritTxFrequency+maxRitdev)conf.frequency=old_freq;
  if (conf.frequency<conf.ritTxFrequency-maxRitdev)conf.frequency=old_freq;
  if (old_freq != conf.frequency){
    setFrequency(conf.frequency);
    displayFreq(0,1,1,1);
    }
}

