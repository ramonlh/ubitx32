
#pragma once

// radio.h
void setFrequency(unsigned long f);
extern void setFrecuencyB(unsigned long f);
void setVFO(byte value);
void setSSB(byte value);
void setCW(byte value);
void setNextHamBandFreq(unsigned long f, int moveDirection);
void setRIT(byte value);
void setSPLIT(byte value);


// tft_320240.h
void displayFreq(byte fast, byte fA, byte fB, byte fAux);

#include <ArduinoJson.h>
#include <WebSocketsServer.h>

#ifdef WEBSOCKETS
  WebSocketsServer wsserver(WSPort);
#endif

template <typename T>
void sendws(const char* tipo, T valor) {
  StaticJsonDocument<128> doc;
  doc["t"] = tipo;
  doc["v"] = valor;
  String json;
  serializeJson(doc, json);
  //s2("Sent:"); s2(json); s2(crlf);
  wsserver.broadcastTXT(json);
}

void procesaWSrec(String datarec)
{
  if(datarec=="f")
    {
    s2(ltoa(conf.frequency,buff,10));
    //wsserver.sendTXT(ltoa(conf.frequency,buff,10));     // send data
    }
  else 
    {
    s2(datarec);
    //wsserver.sendTXT(datarec);     // send data
    }
  s2(crlf);
}

void sendwsData(uint8_t c) {
  //Serial2.print("sendwsData:");Serial2.println(c);
  //if (keylock==1) return;
  char data[60];
  if (c==tcpsplitOn) { sendws("spl", conf.splitOn==1?"ON":"OFF"); }
  else if (c==tcpisUSB) { 
    if (conf.vfoActive=VFO_A) {
      sendws("ssb",conf.isUSBA==1?0:1); 
      }
    else
      sendws("ssb",conf.isUSBB==1?0:1); 
      }
  else if (c==tcpfechahoravalue) { sendws("date",getFechaHora()); }
  else if (c==tcpsmeter) { sendws("s", smetervalue); }
  else if (c==tcpritOn) { sendws("rit", conf.ritOn==1?"ON":"OFF");  }
  else if (c==tcpcwMode) { sendws("cw", conf.cwMode==1?1:0);  }
  else if (c==tcpvfoActive) { sendws("vfo", conf.vfoActive==VFO_A?"A":"B"); }
 // else if (c==tcpcwmodeA) { strcpy(data,itoa(conf.cwModeA,buff,10)); }
 // else if (c==tcpcwmodeB) { strcpy(data,itoa(conf.cwModeB,buff,10)); }
  else if (c==tcpCallSign) { sendws("cs", conf.CallSign); }
  else if (c==tcpfrequencyA) { sendws("fA", conf.frequencyA); }
  else if (c==tcpfrequencyB) { sendws("fB", conf.frequencyB); }
  else if (c==tcpwifi) { strcpy(data,WiFi.isConnected()?"1":"0"); }
  else if (c==tcpscanst) { strcpy(data,itoa(scanF,buff,10)); }
  else if (c==tcpkeylock) { strcpy(data,itoa(keylock,buff,10)); }
  else if (c==tcptemp1) { strcpy(data,itoa(MbR[0],buff,10)); }
  else if (c==tcptemp2) { strcpy(data,itoa(MbR[1],buff,10)); }
  else if (c==tcptemp3) { strcpy(data,itoa(MbR[2],buff,10)); }
  else if (c==tcptunestep) {  sendws("step", conf.arTuneStep[conf.tuneStepIndex]); }
  else if (c==tcpminsmeter) { strcpy(data,itoa(minsmeter,buff,10)); }
  else if (c==tcpmaxsmeter) { strcpy(data,itoa(maxsmeter,buff,10)); }
  else if (c==tcpattlevel) { strcpy(data,itoa(conf.attLevel,buff,10)); }
  else if (c==tcpifShiftVal) { strcpy(data,itoa(conf.ifShiftValue,buff,10)); }
  else if (c==tcpvtotvalue) { 
    dtostrf(vtotvalue,8,2,data); 
    sendws("vt",data);
    }
  else if (c==tcpitotvalue) { 
    dtostrf(itotvalue,8,2,data); 
    sendws("it",data);
    }

  else { strcpy(data,"999"); 
  }
}

void senwsdataini() {
  sendwsData(tcpsplitOn);
  sendwsData(tcp_is_USB);
  sendwsData(tcpritOn);
  sendwsData(tcpcwMode);
  sendwsData(tcpvfoActive);
  sendwsData(tcpfrequencyA);
  sendwsData(tcpfrequencyB);
  sendwsData(tcptunestep);
}

void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      IPAddress ip = wsserver.remoteIP(clientNum);
      s2("Cliente "); s2(clientNum); s2(" conectado desde "); s2(ip.toString().c_str());s2(crlf);
      //Serial2.printf("Cliente #%u conectado desde %s\n", clientNum, ip.toString().c_str());
      // 🔄 Podés enviar estado inicial aquí:
      senwsdataini();
      break;
      }
    case WStype_DISCONNECTED:
      s2("Cliente "); s2(clientNum); s2(" desconectado"); s2(crlf);
      break;
    case WStype_TEXT:
      Serial2.printf("Mensaje de #%u: %s\n", clientNum, payload);
        // Parsear JSON
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      // podés manejar comandos aquí también
      if (error) {
        s2("Error al parsear JSON"); s2(crlf);
        return;
        }

  String tipo = doc["t"];
  String valor = doc["v"];

  Serial2.printf("Comando recibido: tipo=%s, valor=%s\n", tipo.c_str(), valor.c_str());

  // Lógica para cada comando
  if (tipo == "fA") {
    setFrequency(valor.toInt());
    displayFreq(1, 1, 1, 1);
    } 
  if (tipo == "fB") {
    setFrecuencyB(valor.toInt());
    displayFreq(1, 1, 1, 1);
    } 
  else if (tipo == "vfo") {
    setVFO(conf.vfoActive==VFO_A?VFO_B:VFO_A);  // Cambiar el VFO activo
    } 
  else if (tipo == "ssb") {
    setSSB(conf.isUSB==0?1:0); // Cambiar el modo (LSB, USB)
    } 
  else if (tipo == "cw") {
    setCW(conf.cwMode==0?1:0);    // cambiar CW on,off
    } 
  else if (tipo == "ba-") {
    setNextHamBandFreq(conf.frequency,-1);
    } 
  else if (tipo == "ba+") {
    setNextHamBandFreq(conf.frequency,+1);
    } 
  else if (tipo == "bb-") {
    setNextHamBandFreq(conf.frequency,-1);
    } 
  else if (tipo == "bb+") {
    setNextHamBandFreq(conf.frequency,+1);
    } 
  else if (tipo == "rit") {
    setRIT(conf.ritOn==1?0:1); // Activar/desactivar RIT
    }        
  else if (tipo == "spl") {
    setSPLIT(conf.splitOn==1?0:1); // Activar/desactivar SPLIT
    }        
   break;
    // otros tipos opcionales: WStype_BIN, WStype_ERROR...
  }
}

void initWS() { 
  // Configurar servidor WebSocket
  wsserver.begin();
  wsserver.onEvent(onWebSocketEvent);  // Establecer el manejador de eventos
  }



