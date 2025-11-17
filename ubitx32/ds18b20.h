
#pragma once

#include "OneWire.h"                  // Local
#include "DallasTemperature.h"        // Local

OneWire owire(W0);
DallasTemperature sensors0(&owire);

void leevaloresOW()
{
#ifdef DS18B20
  sensors0.requestTemperatures();
  for (byte i=0; i<maxTemp; i++)  
    {
    if (conf.nprobe[i]>0)
      {
      int auxI=sensors0.getTempC(conf.probecode[i])*100;
      if (auxI>0)
        {
        MbR[i]=auxI;
        MbRant[i]=MbR[i];
        }
      }
    }
#endif
}

void initDS18B20() 
{
#ifdef DS18B20
  sensors0.begin();
  sensors0.setResolution(9);
  nTemp=sensors0.getDeviceCount();
  if(nTemp>maxTemp) { nTemp=maxTemp; }

  Serial2.println(F("DS18B20 probes"));
  Serial2.print(b); Serial2.print(b); Serial2.print(t(sondastemp));  Serial2.print(dp);
  Serial2.print(nTemp); Serial2.print(crlf); Serial2.print(b); Serial2.print(b); Serial2.print(t(tModo));  Serial2.print(dp);
  Serial2.print(b); Serial2.print(b); 
  Serial2.println((sensors0.isParasitePowerMode())?c(tparasite):c(tpower));
  for(byte i=0; i<maxTemp; i++)       {   // busca sondas conectadas
    if (sensors0.getAddress(addr1Wire[i], i))    {
      Serial2.print(b);Serial2.print(b);
      for(uint8_t j=0; j<8; j++) { if(addr1Wire[i][j]<16) { Serial2.print(cero); } Serial2.print(addr1Wire[i][j]); }
      Serial2.println();
      }
    }
  Serial2.println(F("  Started"));

#endif
}

