
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

  s2(F("DS18B20 probes"));
  s2(b); s2(b); s2(t(sondastemp));  s2(dp);
  s2(nTemp); s2(crlf); s2(b); s2(b); s2(t(tModo));  s2(dp);
  s2(b); s2(b); 
  s2((sensors0.isParasitePowerMode())?c(tparasite):c(tpower)); s2(crlf);
  for(byte i=0; i<maxTemp; i++)       {   // busca sondas conectadas
    if (sensors0.getAddress(addr1Wire[i], i))    {
      s2(b);s2(b);
      for(uint8_t j=0; j<8; j++) { if(addr1Wire[i][j]<16) { s2(cero); } s2(addr1Wire[i][j]); }
      s2(crlf);
      }
    }
  s2(F("  Started"));s2(crlf);

#endif
}

