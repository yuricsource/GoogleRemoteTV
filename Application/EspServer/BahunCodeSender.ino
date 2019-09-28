#include <Arduino.h>
#define IRLEDpin  6
#define BITtime   562
//put your own code here - 4 bytes (ADDR1 | ADDR2 | COMMAND1 | COMMAND2)
unsigned long IRcode = 0b10000000010011100111100010000111;

uint32_t IrCommand [] =
{
  0b10000000010011100101000010101111,
  0b10000000010011100110000010011111,
  0b10000000010011100111100010000111,
  0b10000000010011100001001011101101,
  0b10000000010011100010001011011101,
  0b10000000010011100011101011000101,
  0b10000000010011101110000000011111,
  0b10000000010011101011001001001101,
  0b10000000010011100011000011001111,
  0b10000000010011100011000011001111,
  0b10000000010011101100000000111111,
  0b10000000010011100100000010111111,
  0b10000000010011101111101000000101,
  0b10000000010011100000001011111101,
  0b10000000010011100111101010000101,
  0b10000000010011100001100011100111,
  0b10000000010011101100001000111101,
  0b10000000010011100111001010001101,
  0b10000000010011100111001010001101,
  0b10000000010011101111001000001101,
  0b10000000010011101001001001101101,
  0b10000000010011101000100001110111,
  0b10000000010011100101001010101101,
  0b10000000010011101110101000010101,
  0b10000000010011100010100011010111,
  0b10000000010011101010101001010101,
  0b10000000010011100110101010010101,
  0b10000000010011100101100010100111,
  0b10000000010011101111100000000111,
  0b10000000010011100001101011100101,
  0b10000000010011100110100010010111,
  0b10000000010011101101100000100111,
  0b10000000010011100010101011010101,
  0b10000000010011100100100010110111,
  0b10000000010011101110100000010111,
  0b10000000010011100000101011110101,
  0b10000000010011101100100000110111,
  0b10000000010011101101000000101111,
  0b10000000010011101001001001101101,
  0b10000000010011101111000000001111,
  0b10000000010011100101101010100101,
  0b10000000010011101100101000110101,
  0b10000000010011101001101001100101
};



void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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
  //Serial.println("start bit:");
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
  //Serial.println("");
  //Serial.println("End");
}
String inString = "";    // string to hold input
int inChar;
void loop()                           //some demo main code
{
  int commandId = 0;
  while (Serial.available() > 0) {
    inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      commandId = inString.toInt();
      Serial.print("Value:");
      Serial.print(commandId);
      Serial.println();
      // clear the string for new input:
      inString = "";
    }
  }
  if (inChar == '\n' && commandId != 0)
  {
    for (int i = 0; i < 5; i++)
    {
      Serial.println(IrCommand[commandId]);
      IRsendCode(IrCommand[commandId - 1]);
      delay(700);
      inChar = 0;
    }
  }
}
