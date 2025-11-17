
#pragma once

#ifdef ADS1X15
  Adafruit_ADS1115 adsA;  /* Use this for the 16-bit version */
  Adafruit_ADS1115 adsB;  /* Use this for the 16-bit version */
#endif

void readVIpower()
{
#ifdef ADS1X15
  int16_t vtotadc=adsB.readADC_SingleEnded(VTOTp);    // es el valor leído del ADC sin convertir
  int16_t itotadc=adsB.readADC_SingleEnded(ITOTp);    // es el valor leído del ADC sin convertir
  float factorv=290.0;    // factor teórico
  float factori=1000.0;   // factor teórico
  float factorvr=1.0;     // factor corrección
  float factorir=0.75;   // factor corrección
  vtotvalue=factorvr*vtotadc*0.1875/factorv;
  itotvalue=-(factorir)*(itotadc*0.1875-2500)*10/factori;
#endif
}

void initADS()
{
#ifdef ADS1X15
  adsA.begin();
  adsA.setGain(GAIN_TWOTHIRDS);  // +/- 6.144V  1 bit = 0.1875mV (default)
  adsB.begin();
  //adsB.setGain(GAIN_TWOTHIRDS);  // +/- 6.144V  1 bit = 0.1875mV (default)
#endif
}

void readSmeter() 
{ 
#ifdef ADS1X15
  int16_t smeteradc=0;
  smeteradc=adsA.readADC_SingleEnded(SMETERp);    // es el valor leído del ADC sin convertir
  calSmeterReq=calSmeterReq || (smeteradc<conf.sMeterLevels[0]) || (smeteradc>conf.sMeterLevels[15]);
  smeteradc=smeteradc>smeterlast?((smeterlast*7+smeteradc*3))/10:   // valor ajustado para evitar variaciones rápidas
                                 ((smeterlast*9+smeteradc*1))/10;   // de VK2ETA
  smeterlast=smeteradc;     // guardar último valor leído
  if (smeteradc<=minsmeter) { minsmeter=smeteradc; }    // buscar mínimo      
  if (smeteradc>=maxsmeter) { maxsmeter=smeteradc; }    // buscar máximo    

  // convertir valor a escala 0-90
  byte i=0; boolean encontrado=false;
  while ((i<16) && (!encontrado))           // busca intervalo de 0 a 16
    {
    if (smeteradc<conf.sMeterLevels[i]) 
      encontrado=true;
    else
      i++;
    }
  if (i<15)
    smetervalue=((i-1)*6) + (smeteradc-conf.sMeterLevels[i])*6 / (conf.sMeterLevels[i]-conf.sMeterLevels[i-1]);
  else
    smetervalue=90; 
  #endif
}

float readSWR(int limit)
{
#ifdef ADS1X15
  float auxSWR=1;
  int16_t adc0, adc1;
  long ldc0=0; long ldc1=0; 
  for (byte i=0;i<conf.ATUIter;i++)
    {
    ldc0=ldc0+adsA.readADC_SingleEnded(VFORp); // VFORp=0
    ldc1=ldc1+adsA.readADC_SingleEnded(VREFp); // VREFp=1
    }
  adc0 = ldc0/conf.ATUIter; if (adc0<0) adc0=0;
  adc1 = ldc1/conf.ATUIter; if (adc1<0) adc1=0;
  
  vFORc=((float(adc0)*0.1875/1000)+0.25)*11*0.707;
  vREFc=((float(adc1)*0.1875/1000)+0.25)*11*0.707;
  //wFORc=vFORc*vFORc/50; wREFc=vREFc*vREFc/50;
  wFORc=vFORc*vFORc*0.707/50; wREFc=vREFc*vREFc*0.707/50;
  if ((vFORc-vREFc)>0) SWRreal=(vFORc+vREFc)/(vFORc-vREFc); else SWRreal=1.0;
  auxSWR=(SWRreal*conf.ATUFactor)+conf.ATUOffset;
  if (auxSWR<limit) auxSWR=1;
  return(auxSWR);
#endif
}
