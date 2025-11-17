
#pragma once

#include "WiFi.h"

void connectSTA()
{
  s2("  Connecting: ");  s2(conf.ssidSTA);
  s2("/");  s2(conf.passSTA);
  displayMsg("Connecting a",conf.ssidSTA,"********",40,140,210,75);
  byte cont = 0;
  while((!WiFi.isConnected()) && (cont++ < 20))  { delay(500); s2("."); }
  clearMsg(40,140,210,75);
  displayWiFiSt();
  s2(WiFi.isConnected()?" OK":" ERROR"); s2(crlf);
  s2("  STA IP: ");  s2(WiFi.localIP()); s2(crlf);
  s2("  STA MAC: ");  s2(WiFi.macAddress()); s2(crlf);
  s2("  Subnet Mask: ");  s2(WiFi.subnetMask()); s2(crlf);
  s2("  Gateway IP: "); s2(WiFi.gatewayIP()); s2(crlf);
  s2("  DNS: ");  s2(WiFi.dnsIP()); s2(crlf);
}


void initWiFi() {
  
  if(conf.wifimode==0) { WiFi.mode(WIFI_OFF); }
  else if (conf.wifimode==1) { WiFi.mode(WIFI_STA); }
  else if (conf.wifimode==2) { WiFi.mode(WIFI_AP); }
  else if (conf.wifimode==3) { WiFi.mode(WIFI_AP_STA); }
  else { WiFi.mode(WIFI_AP); }
  
  //WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  s2("WiFiMode: "); s2(wifimodetext[conf.wifimode]);s2(crlf);
  WiFi.enableAP((conf.wifimode==2) || (conf.wifimode==3));
  if ((conf.wifimode==2) || (conf.wifimode==3)) {       // AP o AP+STA
    WiFi.channel(conf.canalAP);
    WiFi.softAP(conf.ssidAP, conf.passAP, conf.canalAP, false, 2);
    s2("AP mode:");s2(crlf);
    s2("  Channel:");s2(WiFi.channel());s2(crlf);
    s2("  AP MAC:"); s2(WiFi.softAPmacAddress());s2(crlf);
    s2("  IP:");s2(WiFi.softAPIP());s2(crlf);
    }
  if ((conf.wifimode==1) || (conf.wifimode==3))        // STA o AP+STA
    {
    s2("STA mode:");s2(crlf);
    if(conf.staticIP==1) { 
      WiFi.config(conf.EEip, conf.EEgw, conf.EEmask, conf.EEdns, conf.EEdns2); 
        }
//      WiFi.begin(conf.ssidSTA, conf.passSTA);
      WiFi.begin(conf.ssidSTA, conf.passSTA, true);
      s2("  Static IP:");s2(conf.staticIP?"YES ":"NO ");
      for(byte i=0; i<4; i++) { s2(conf.EEip[i]); s2("."); }  s2(crlf);
      s2("  Gateway:"); for(byte i=0; i<4; i++) { s2(conf.EEgw[i]); s2("."); } s2(crlf);
      s2("  EEmask:"); for(byte i=0; i<4; i++) { s2(conf.EEmask[i]); s2("."); } s2(crlf);
      s2("  EEdns:"); for(byte i=0; i<4; i++) { s2(conf.EEdns[i]); s2("."); } s2(crlf);
      s2("  EEdns2:"); for(byte i=0; i<4; i++) { s2(conf.EEdns2[i]); s2("."); } s2(crlf);
    }
  if (conf.autoWiFi==1)
    {
    if ((conf.wifimode==1) || (conf.wifimode==3))
      {
      connectSTA();
      updateDisplay(1);
      }
    }
}

void setWiFi()
{
   if (WiFi.isConnected())
     { 
     WiFi.disconnect();  
     displayWiFiSt();
     }
   else
     {
     initWiFi();
     if ((conf.wifimode==1) || (conf.wifimode==3))  
       connectSTA();
     if (conf.wifimode>0) 
       initNetServices();
     }
}


