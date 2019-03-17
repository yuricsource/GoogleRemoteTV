// Credits  http://www.youtube.com/watch?v=BUvFGTxZBG8

#include <Arduino.h>
#define IRLEDpin  D6
#define BITtime   562
//put your own code here - 4 bytes (ADDR1 | ADDR2 | COMMAND1 | COMMAND2)
unsigned long IRcode=0b10000000010011100111100010000111;

void setup()
{
  Serial.begin(115200);
  IRsetup();
}

void IRsetup(void)
{
  pinMode(IRLEDpin, OUTPUT);
  digitalWrite(IRLEDpin, LOW);    //turn off IR LED to start
}

// Ouput the 38KHz carrier frequency for the required time in microseconds
// This is timing critial and just do-able on an Arduino using the standard I/O functions.
// If you are using interrupts, ensure they disabled for the duration.
void IRcarrier(unsigned int IRtimemicroseconds)
{
  for (int i = 0; i < (IRtimemicroseconds / 26); i++)
  {
    digitalWrite(IRLEDpin, HIGH);   //turn on the IR LED
    //NOTE: digitalWrite takes about 3.5us to execute, so we need to factor that into the timing.
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    digitalWrite(IRLEDpin, LOW);    //turn off the IR LED
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
  }
}

//Sends the IR code in 4 byte NEC format
void IRsendCode(uint64_t code)
{
  //send the leading pulse
  IRcarrier(9000);            //9ms of carrier
  delayMicroseconds(4500);    //4.5ms of silence
  Serial.println("start bit:");
  //send the user defined 4 byte/32bit code
  for (int i = 0; i < 32; i++)        //send all 4 bytes or 32 bits
  {
    IRcarrier(BITtime);               //turn on the carrier for one bit time
    if (code & 0x80000000)            //get the current bit by masking all but the MSB
    { 
      delayMicroseconds(3 * BITtime); //a HIGH is 3 bit time periods
    }
    else
    {
      delayMicroseconds(BITtime);     //a LOW is only 1 bit time period
    }
      
    code <<= 1;                      //shift to the next bit for this byte
  }
  IRcarrier(BITtime);                 //send a single STOP bit.
  Serial.println("");
  Serial.println("End");
}

void loop()                           //some demo main code
{
  IRsendCode(IRcode);
  delay(1000);
}
