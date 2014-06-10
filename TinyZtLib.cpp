/*
  ZtLib.cpp - ZT module Drive Library for Wiring & Arduino
  Copyright (c) 2012 Alvin Li(Kozig/www.kozig.com).  All right reserved.
  This library is free software; 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  Version:V1.1
*/

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}
#include "TinyZtLib.h"
#include "TinyWireM.h"
#define Wire TinyWireM
#if defined(ARDUINO) && (ARDUINO >= 100) //scl
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

uint8_t i2cAd=0x27;

///ZT.SEG8B4A036A PART///-------------------------------------------------------------------s
unsigned char codetable[] = 
{
   0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F, 0x77, 0x7C,0x39,0x5E,0x79,0x71,0x40,0x00
};

// Public Methods //////////////////////////////////////////////////////////////
/* 
 * Function I2cInit
 * Desc     TWI/I2C init
 * Input    none
 * Output   none
 */

 TinyZtlib::TinyZtlib(uint8_t address)
 {
   i2cAd=address;
}

void TinyZtlib::I2cInit(void)
{
   Wire.begin();
}
/* 
 * Function Seg8b4a036aSleep
 * Desc     Set ZT.SEG8B4A036A Go to Sleep
 * Input    addr:ZT.SEG8B4A036A Address
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
 int TinyZtlib::WriteBuff(uint8_t *buff,int len){
  Wire.beginTransmission(i2cAd);
  for(int i=0;i<len;i++)  {
    Wire.send(buff[i]);
  } 
  return Wire.endTransmission();
 } 

int TinyZtlib::Seg8b4a036aSleep(uint8_t addr)
{
  uint8_t buff[8]={REG_SLEEP, SLEEP_ON, 0, 0, 0};
  return WriteBuff(buff,5);
}
/* 
 * Function Seg8b4a036aUnSleep
 * Desc     Set ZT.SEG8B4A036A Wait Up From Sleep
 * Input    addr:ZT.SEG8B4A036A Address
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aUnSleep(uint8_t addr)
{
  uint8_t buff[5]={REG_SLEEP, SLEEP_OFF, 0, 0, 0};
  return WriteBuff(buff, 5);
}
/* 
 * Function Seg8b4a036aReadState
 * Desc     Read ZT.SEG8B4A036A Status
 * Input    addr:ZT.SEG8B4A036A Address
 * Output   !=0xFF ZT.SC-I2CMx Status
 *          0xFF .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aReadState(uint8_t addr)
{
   uint8_t state = 0xFF;
   uint8_t temp;
   uint8_t buff[1] = {REG_STATUS};
   temp = WriteBuff(buff, 1); // no stop
   if (temp ==0)
   {
      Wire.requestFrom(i2cAd,1);
      uint8_t byte;
      int i=0;
      while(Wire.available()>0){
        buff[0]=Wire.receive();
      }  
      temp=i;
   }
   if (temp==1)
   {
      state = buff[0];
   }

   return state;
}
/* 
 * Function Seg8b4a036aReadVersion
 * Desc     Read ZT.SEG8B4A036A Fireware Version
 * Input    addr:ZT.SEG8B4A036A Address
            *buf:Version Buffer             
 * Output   .. number bytes of Version Read out
 */
int TinyZtlib::Seg8b4a036aReadVersion(uint8_t addr, uint8_t *buf)
{
   uint8_t state = 0xFF;
   uint8_t temp;
   uint8_t regv[1] = {REG_VERSION};
   temp = WriteBuff(regv,1);//
   if (temp ==0)
   {
      Wire.requestFrom(i2cAd,19);
      uint8_t byte;
      int i=0;
      while(Wire.available()>0){
        *(buf+i)=Wire.receive();
      }  
      temp=i;
   }
   return temp;
}
/* 
 * Function Seg8b4a036aDisplayDec
 * Desc     ZT.SEG8B4A036A Display decimal numeral
 * Input    addr:ZT.SEG8B4A036A Address
            val: Display Val
            bitnum:Display Bit Number
            dotbit: Dot Display
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aDisplayDec(uint8_t addr,unsigned short val, uint8_t bitnum, uint8_t dotbit)
{
  uint8_t i;
  uint8_t segnum[5];
  if (val>9999) return 0xFF;
  
  segnum[0] = REG_DAT;
  segnum[1] = val%10;
  segnum[2] = (val%100)/10;
  segnum[3] = (val/100)%10;
  segnum[4] = val/1000;
  for (i=1; i<5; i++)
  {
      segnum[i] = codetable[segnum[i]];
      if (dotbit&0x01)
      {
          segnum[i] |= 0x80; 
      }
      dotbit >>= 1;
  }
  
  if (bitnum==DISP_0BIT)      {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;segnum[1] = 0;}
  else if (bitnum==DISP_1BIT) {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;}
  else if (bitnum==DISP_2BIT) {segnum[4] = 0;segnum[3] = 0;}
  else if (bitnum==DISP_3BIT) {segnum[4] = 0;}
  else if (bitnum==DISP_AUTO)
  {
     if (val<10)        {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;}
     else if (val<100)  {segnum[4] = 0;segnum[3] = 0;}
     else if (val<1000) {segnum[4] = 0;}
  }
  
  return WriteBuff(segnum,5);
}

/* 
 * Function Seg8b4a036aDisplayDec
 * Desc     ZT.SEG8B4A036A Display decimal numeral
 * Input    addr:ZT.SEG8B4A036A Address
            val: Display Val
            bitnum:Display Bit Number
            dotbit: Dot Display
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aDisplayDecAndCode(uint8_t addr,unsigned short val,uint8_t code, uint8_t bitnum, uint8_t dotbit)
{
  uint8_t i;
  uint8_t segnum[5];
  if (val>999) return 0xFF;
  
  segnum[0] = REG_DAT;
  segnum[1] = val%10;
  segnum[2] = (val%100)/10;
  segnum[3] = (val/100)%10;
  segnum[4] = val/1000;
  for (i=1; i<4; i++)
  {
      segnum[i] = codetable[segnum[i]];
      if (dotbit&0x01)
      {
          segnum[i] |= 0x80; 
      }
      dotbit >>= 1;
  }
  segnum[4] = code;
  if (bitnum==DISP_0BIT)      {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;segnum[1] = 0;}
  else if (bitnum==DISP_1BIT) {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;}
  else if (bitnum==DISP_2BIT) {segnum[4] = 0;segnum[3] = 0;}
  else if (bitnum==DISP_3BIT) {segnum[4] = 0;}
  else if (bitnum==DISP_AUTO)
  {
      if (val<10)        {segnum[4] = segnum[4];segnum[3] = 0;segnum[2] = 0;}
      else if (val<100)  {segnum[4] = segnum[4];segnum[3] = 0;}
      else if (val<1000) {segnum[4] = segnum[4];}
  }
  
  return WriteBuff(segnum,5);
}

/* 
 * Function Seg8b4a036aDisplayHex
 * Desc     Read ZT.SEG8B4A036A Display hexadecimal number
 * Input    addr:ZT.SEG8B4A036A Address
            val: Display Val
            bitnum:Display Bit Number
            dotbit: Dot Display
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aDisplayHex(uint8_t addr,unsigned short val, uint8_t bitnum, uint8_t dotbit)
{
  uint8_t i;
  unsigned short temp;
  uint8_t segnum[5];
  segnum[0] = REG_DAT;
  temp = val;
  for (i=1; i<5; i++)
  {
      segnum[i] = temp&0x000F;
      temp >>= 4;
      segnum[i] = codetable[segnum[i]];
      if (dotbit&0x01)
      {
          segnum[i] |= 0x80; 
      }
      dotbit >>= 1;
  }
  
  if (bitnum==DISP_0BIT)      {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;segnum[1] = 0;}
  else if (bitnum==DISP_1BIT) {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;}
  else if (bitnum==DISP_2BIT) {segnum[4] = 0;segnum[3] = 0;}
  else if (bitnum==DISP_3BIT) {segnum[4] = 0;}
  else if (bitnum==DISP_AUTO)
  {
     if (!(val&0xFFF0))      {segnum[4] = 0;segnum[3] = 0;segnum[2] = 0;}
     else if (!(val&0xFF00)) {segnum[4] = 0;segnum[3] = 0;}
     else if (!(val&0xF000)) {segnum[4] = 0;}
  }
  
  return WriteBuff(segnum, 5);
}
/* 
 * Function Seg8b4a036aSetBrightness
 * Desc     Set ZT.SEG8B4A036A Brightness
 * Input    addr:ZT.SEG8B4A036A Address
            OnDelay: 
            OffDelay:
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aSetBrightness(uint8_t addr, uint8_t OnDelay, uint8_t OffDelay)
{
  uint8_t buff[5] = {REG_BRIGHTNESS, OnDelay, OffDelay, 0, 0};
  return WriteBuff(buff,5);
}
/* 
 * Function Seg8b4a036aSetAddress
 * Desc     Set ZT.SEG8B4A036A New Address
 * Input    val:ZT.SEG8B4A036A Address New Address
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aSetAddress(uint8_t val)
{
   uint8_t buff[2] = {REG_ADDRESS, val};
   return 0;//  Wire.BeginTransmission(ZTSEG8B4A036A_DADDR, buff, 2, 1, 1);
}
/* 
 * Function Seg8b4a036aDisplayBuff
 * Desc     Set ZT.SEG8B4A036A Brightness
 * Input    addr:ZT.SEG8B4A036A Address
            *buf: Display buffer
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
int TinyZtlib::Seg8b4a036aDisplayBuff(uint8_t addr,uint8_t *buf)
{
  uint8_t buff[5]={REG_DAT, buf[0], buf[1], buf[2], buf[3]};
  
  return WriteBuff(buff, 5);
}


// Preinstantiate Objects //////////////////////////////////////////////////////

TinyZtlib ZT((uint8_t) 0x51);

