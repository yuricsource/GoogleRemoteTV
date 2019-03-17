
#define IRLEDpin  D5              //the arduino pin connected to IR LED to ground. HIGH=LED ON
#define BITtime   562            //length of the carrier bit in microseconds

enum NecProtocol
{
  Bit_0,
  Bit_1,
  StartBit,
  EndBit
};

enum bitStatus
{
  Starting,
  Receiving,
  Ending,
  Finished
};

NecProtocol received[100];
uint8_t receiveIndex = 0;
void readNecCommand(void)
{
  receiveIndex = 0;
  uint16_t counter = 0; // 65535

  for (;;)
  {
    bitStatus bitstage = bitStatus::Starting;
    for (counter = 0; counter < 20000; counter ++)
    {
      switch(bitstage)
      {
        case bitStatus::Starting:
          if (digitalRead(IRLEDpin) == LOW)
          {
            bitstage = bitStatus::Receiving;
          }
          break;
          
        case bitStatus::Receiving:
          if (digitalRead(IRLEDpin) == HIGH)
          {
            bitstage = bitStatus::Ending;
          }
          break;
          
        case bitStatus::Ending:
          if (digitalRead(IRLEDpin) == LOW)
          {
            bitstage = bitStatus::Finished;
          }
          break;
      }
      
      if (bitstage == bitStatus::Finished)
      {
        break;
      }
      delayMicroseconds(1);
    }
    Serial.println(counter);
    if (counter < 500) 
    {
      received[receiveIndex++] = NecProtocol::Bit_0;
      //Serial.print("0");
    }
    else if (counter < 1000)
    {
      //Serial.print("1");
      received[receiveIndex++] = NecProtocol::Bit_1;
    }
    else if (counter < 15000)
    {
      received[receiveIndex++] = NecProtocol::StartBit;
      //Serial.print("S");
    }
    else
    {
      received[receiveIndex++] = NecProtocol::EndBit;
      //Serial.print("E");
      return;
    }
  }
}

void setup()
{
  pinMode(IRLEDpin, INPUT);
  Serial.begin(115200);
  Serial.println("NEC Reader.");
}

void loop()
{
  if (digitalRead(IRLEDpin) == LOW)
  {
    Serial.println("Started.");
    // Start reading
    receiveIndex = 0;
    readNecCommand();
    Serial.print("Length: ");
    Serial.println((int)receiveIndex);
    for (uint8_t i = 0; i < receiveIndex; i ++)
    {
      Serial.print((int)received[i]);
    }
  }
}
