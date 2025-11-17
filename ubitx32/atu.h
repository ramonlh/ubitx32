
#pragma once

void initATU()
{
  // Allow allocation of all timers
  //ESP32PWM::allocateTimer(0);
  //ESP32PWM::allocateTimer(1);
  //ESP32PWM::allocateTimer(2);
  //ESP32PWM::allocateTimer(3);
  //cap1.setPeriodHertz(50);    // standard 50 hz servo
  //cap2.setPeriodHertz(50);    // standard 50 hz servo
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings for an accurate 0 to 180 sweep
  //cap1.write(conf.posATUC1);
  //cap2.write(conf.posATUC2);
}

void acopla()
{
  /*
  clearTFT();
  s2("Tunning"); s2(crlf);
  // cap1
  cap1.write(180);              // tell servo to go to position in variable '0'
  cap2.write(180);              // tell servo to go to position in variable '0'
  delay(500);
  swrmin=999; posmin1=180; 
  for (int i=180; i>0; i--) {   // goes from 0 degrees to 180 degrees
    SWR=readSWR(0);
    displaySWR2();
  //  displaySWR(1);
    cap1.write(i);              // tell servo to go to position in variable 'pos'
    if (SWRreal<swrmin) 
      { 
      posmin1=i-1; swrmin=SWRreal; 
      }
    }
  delay(100);
  conf.posATUC1=posmin1;
  cap1.write(conf.posATUC1);            // tell servo to go to position in variable 'pos'
  saveconf();

  // cap 2
  swrmin=999; posmin2=180;
  for (int i=180; i>0; i--) {   // goes from 0 degrees to 180 degrees
    SWR=readSWR(0);
    displaySWR2();  
//    displaySWR(1);
    cap2.write(i);              // tell servo to go to position in variable 'pos'
    if (SWRreal<swrmin) 
      { 
      posmin2=i-1; 
      swrmin=SWRreal; 
      }
    }
  delay(100);
  conf.posATUC2=posmin2;
  saveconf();
  cap2.write(conf.posATUC2);            // tell servo to go to position in variable 'pos'
  delay(500);
  */
}

