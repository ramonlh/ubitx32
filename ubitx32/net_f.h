
#pragma once

#include "htmlfunctions.h"

void initWebserver() { 
  server.begin(); 
} 

void initTime() {
  ///// Time
  timeClient.begin();
  timeClient.setTimeOffset(timezoneval[conf.timezone]);
  if(timeClient.update()==1)
    { countfaulttime=0; setTime(timeClient.getEpochTime()); }
  else 
    { s2("timeclient.update:"); s2(timeClient.update()); }
}

void initNetServices()
{
  //task1();
  if ((conf.wifimode>0))  // 
    {
    s2("IP services\n");
    if (conf.webenable)
      {  
      initHTML();  s2("  HTML server started"); s2(crlf);
      initWebserver(); s2("  Web server started, port "); s2(conf.webPort);
      }
    else
      s2("  WEB server disabled");
    s2(crlf);
    }
    
    s2("  WS server "); 
    if (conf.wsenable)
      {
      initWS(); s2("iniciado en puerto "); s2(conf.wsPort);  s2(crlf);
      }
    else
      s2("disabled\n");

  if ((conf.wifimode==1) || (conf.wifimode==3))  // STA o AP+STA
    {
    int auxI=checkInternet();  s2("  Check Internet: ");s2(auxI==HTTP_CODE_OK?"OK\n":"ERROR\n");s2(crlf);
    internetON=(auxI==HTTP_CODE_OK); 
    if (internetON)
      {
      s2("Internet services\n");
      initTime(); s2("  initTime\n");
      int auxR=checkMyIP();   
         s2("  MyIP server: "); s2(conf.hostmyip);
         s2(auxR==HTTP_CODE_OK?" OK ":" ERROR "); s2(conf.myippub); s2(crlf);
      }
    }
}
