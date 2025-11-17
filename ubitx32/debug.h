
#pragma once

void showSettings()
{
  s2("-----------------------------------------"); s2(crlf);
  s2("Size of CONF:"); s2(sizeof(conf)); s2(crlf);
  s2("LIBRE0:"); s2(conf.LIBRE0); s2(crlf);
  s2("calibration:"); s2(conf.calibration); s2(crlf);
  s2("usbCarrier:"); s2(conf.usbCarrier); s2(crlf);
  s2("cwmCarrier:"); s2(conf.cwmCarrier); s2(crlf);
  s2("splitOn:"); s2(conf.splitOn); s2(crlf);
  s2("isUSB:"); s2(conf.isUSB); s2(crlf);
  s2("cwMode:"); s2(conf.cwMode); s2(crlf);
  s2("cwSpeed:"); s2(conf.cwSpeed); s2(crlf);
  s2("cwTimeout:"); s2(conf.cwTimeout); s2(crlf);
  s2("ritOn:"); s2(conf.ritOn); s2(crlf);
  s2("cwModeA:"); s2(conf.cwModeA); s2(crlf);
  s2("cwModeB:"); s2(conf.cwModeB); s2(crlf);
  s2("attLevel:"); s2(conf.attLevel); s2(crlf);
  s2("sdrModeOn:"); s2(conf.sdrModeOn); s2(crlf);
  s2("TempAlarm:"); s2(conf.TempAlarm); s2(crlf);
  s2("TempTxOff:"); s2(conf.TempTxOff); s2(crlf);
  s2("framemode:"); s2(conf.framemode); s2(crlf);
  s2("frame1mode:"); s2(conf.frame1mode); s2(crlf);
  s2("frame2mode:"); s2(conf.frame2mode); s2(crlf);
  s2("frame3mode:"); s2(conf.frame3mode); s2(crlf);
  s2("vfoActive:"); s2(conf.vfoActive); s2(crlf);
  s2("isUSBA:"); s2(conf.isUSBA); s2(crlf);
  s2("ATUFactor:"); s2(conf.ATUFactor); s2(crlf);
  s2("ATUOffset:"); s2(conf.ATUOffset); s2(crlf);
  s2("sideTone:"); s2(conf.sideTone); s2(crlf);
  s2("cwDelayTime:"); s2(conf.cwDelayTime); s2(crlf);
  s2("delayBeforeCWStartTime:"); s2(conf.delayBeforeCWStartTime); s2(crlf);
  s2("SI5351BX_ADDR:"); s2(conf.SI5351BX_ADDR); s2(crlf);
  s2("cwKeyType:"); s2(conf.cwKeyType); s2(crlf);
  s2("tuneStepIndex:"); s2(conf.tuneStepIndex); s2(crlf);
  s2("commonOption0:"); s2(conf.commonOption0); s2(crlf);
  s2("addrADS1115A:"); s2(conf.addrADS1115A); s2(crlf);
  s2("addrADS1115B:"); s2(conf.addrADS1115B); s2(crlf);
  s2("useHamBandCount:"); s2(conf.useHamBandCount); s2(crlf);
  s2("tuneTXType:"); s2(conf.tuneTXType); s2(crlf);
  s2("isShiftDisplayCWFreq:"); s2(conf.isShiftDisplayCWFreq); s2(crlf);
  s2("hamBandsRange"); s2(crlf);
  for (byte i=0;i<10;i++)
    {
    s2(i);
    s2(":");s2(conf.hamBandName[i]);
    s2(":");s2(conf.hamBandRange[i][0]);
    s2("-"); s2(conf.hamBandRange[i][1]);
  } s2(crlf);
  s2("ifShiftValue:"); s2(conf.ifShiftValue);s2(crlf);
  s2("sMeterLevels (0-15):");
  for (byte i=0;i<16;i++) { s2(":"); s2(conf.sMeterLevels[i]); } s2(crlf);
  s2("userCallsignLength:"); s2(conf.userCallsignLength); s2(crlf);
  s2("CallSign:"); s2(conf.CallSign); s2(crlf);
  s2("WsprMSGCount:"); s2(conf.WsprMSGCount); s2(crlf);
  s2("frequency:"); s2(conf.frequency); s2(crlf);
  s2("frequencyA:"); s2(conf.frequencyA); s2(crlf);
  s2("frequencyB:"); s2(conf.frequencyB); s2(crlf);
  s2("ritRxFrequency:"); s2(conf.ritRxFrequency); s2(crlf);
  s2("ritTxFrequency:"); s2(conf.ritTxFrequency); s2(crlf);
  s2("scaledSMeter:"); s2(conf.scaledSMeter); s2(crlf);
  s2("wifimode:"); s2(conf.wifimode); s2(crlf);
  s2("canalAP:"); s2(conf.canalAP); s2(crlf);
  s2("ssidSTA:"); s2(conf.ssidSTA); s2(crlf);
  s2("passSTA:"); s2(conf.passSTA); s2(crlf);
  s2("ssidAP:"); s2(conf.ssidAP); s2(crlf);
  s2("passAP:"); s2(conf.passAP); s2(crlf);
  s2("EEip:"); for (byte i=0;i<4;i++) { s2(conf.EEip[i]);s2(" "); }  s2(crlf);
  s2("EEgw:"); for (byte i=0;i<4;i++) { s2(conf.EEgw[i]);s2(" "); }  s2(crlf);
  s2("EEmask:"); for (byte i=0;i<4;i++) { s2(conf.EEmask[i]);s2(" "); }  s2(crlf);
  s2("EEdns:"); for (byte i=0;i<4;i++) { s2(conf.EEdns[i]);s2(" "); }  s2(crlf);
  s2("EEdns2:"); for (byte i=0;i<4;i++) { s2(conf.EEdns2[i]);s2(" "); }  s2(crlf);
  s2("EEmac:"); for (byte i=0;i<6;i++) { s2(conf.EEmac[i]);s2(" "); }  s2(crlf);
  s2("myippub:"); s2(conf.myippub); s2(crlf);
  s2("staticIP:"); s2(conf.staticIP); s2(crlf);
  s2("ftpenable:"); s2(conf.ftpenable); s2(crlf);
  s2("webPort:"); s2(conf.webPort); s2(crlf);
  s2("firstIF:"); s2(conf.firstIF); s2(crlf);
  //s2("scanallf:"); s2(conf.scanallf); s2(crlf);
  s2("arTuneStep:"); 
  for (byte i=0;i<9;i++)
    { s2(i);s2(":");s2(conf.arTuneStep[i]); s2(" "); }
  s2(crlf);
  s2("latitude:"); s2(conf.latitud); s2(crlf);
  s2("longitude:"); s2(conf.longitud); s2(crlf);
  s2("lang:"); s2(conf.lang); s2(crlf);

  s2("rstper:"); s2(conf.rstper); s2(crlf);
  s2("usepassDev:"); s2(conf.usepassDev); s2(crlf);
  s2("probecode:"); s2(crlf);
  for (byte i=0;i<8;i++)
    { 
    s2(i);s2(":");
    for (byte j=0;j<8;j++) { s2(conf.probecode[i][j]); s2(" ");  }
    s2(crlf);
    }
  s2("userDev:"); s2(conf.userDev); s2(crlf);
  s2("passDev:"); s2(conf.passDev); s2(crlf);
  s2("iftttenabled:"); s2(conf.iftttenabled); s2(crlf);
  s2("iftttkey:"); s2(conf.iftttkey); s2(crlf);
// s2("iottweetenable:"); s2(conf.iottweetenable); s2(crlf);
//  s2("iottweetuser:"); s2(conf.iottweetuser); s2(crlf);
//  s2("iottweetkey:"); s2(conf.iottweetkey); s2(crlf);
  s2("mqttenabled:"); s2(conf.mqttenabled); s2(crlf);
  s2("mqttserver:"); s2(conf.mqttserver); s2(crlf);
  s2("mqttpath:");  s2(crlf);
  for (byte i=0;i<6;i++) { s2(conf.mqttpath[i]); s2("/"); } s2(crlf);;
  //s2("dweetenabled:"); s2(conf.dweetenabled); s2(crlf);
  s2("hostmyip:"); s2(conf.hostmyip); s2(crlf);
  s2("actualBand:"); s2(conf.actualBand); s2(crlf);
  s2("autoWiFi:"); s2(conf.autoWiFi); s2(crlf);
  s2("scanallf:"); s2(conf.scanallf); s2(crlf);
  s2("TXall:"); s2(conf.TXall); s2(crlf);
  s2("autoMode:"); s2(conf.autoMode); s2(crlf);
  s2("calData:");  s2(crlf);
  for (byte i=0;i<5;i++) { s2(conf.calData[i]); s2(" "); } s2(crlf);
  s2("memMode:"); s2(conf.TXall); s2(crlf);
  s2("lastmempos:"); s2(conf.lastmempos); s2(crlf);

  s2("ATUdelay:"); s2(conf.ATUdelay); s2(crlf);
  s2("TXall:"); s2(conf.TXall); s2(crlf);
  s2("nprobe:"); s2(conf.usepassDev); s2(crlf);
  for (byte i=0;i<8;i++)
    { 
    s2(i);s2(":");
    s2(conf.nprobe[i]); s2(" ");  
    }
  s2(crlf);
  s2("============================================="); s2(crlf);
}


void printhelp()
{
  //s2("alias,aliasdevice      (set alias)");s2(crlf);
  //s2("debug                  (debug ON/OFF)");s2(crlf);
  s2("a               (auto WiFi: yes/no)\n");
  s2("f               (check files)\n");
  s2("F               (reset factory)\n");
  s2("h               (help)\n"); 
  s2("p,SSIDpass      (set passSTA)\n");
  s2("P,SSID-APpass   (set pass AP)\n");
  s2("r               (reset)\n"); 
  s2("s,SSIDname      (set SSID STA)\n");
  s2("S,SSID-AP       (set SSID AP)\n");
  s2("t               (format FFat)\n");
  s2("u               (show status)\n");
  s2("w,n             (wifimode:0,1,2,3=OFF,STA,AP,STA+AP)\n");
  s2("---------------------------------------\n");
}

void printstatus()
{
  s2("WiFi Mode: "); s2(conf.wifimode==0?"OFF":conf.wifimode==1?"STA":conf.wifimode==2?"AP":"AP+STA");s2(crlf);
  s2("IP: ");        s2(WiFi.localIP());s2(crlf);
  s2("Port: ");      s2(conf.webPort);s2(crlf);
  s2("mask: ");      s2(WiFi.subnetMask());s2(crlf);
  s2("GW: ");        s2(WiFi.gatewayIP());s2(crlf);
  s2("ssid: ");      s2(conf.ssidSTA);s2(crlf);
  s2("pass: ");      s2(conf.passSTA);s2(crlf);
  s2("ssidAP: ");    s2(conf.ssidAP);s2(crlf);
  s2("passAP: ");    s2(conf.passAP);s2(crlf);
  s2("autoWiFi: ");   s2(conf.autoWiFi==0?"NO":"YES");s2(crlf);
  s2("Conn: ");      s2(WiFi.isConnected()?ok:"NO");s2(crlf);
  s2("----------------\n");
}

