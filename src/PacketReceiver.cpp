#include "PacketReceiver.h"
#include <Arduino.h>

PacketReceiver::PacketReceiver(CustomSoftwareSerial& css) : customSerial(css), packetQueue(sizeof(Packet), 10, FIFO), lastErrorCode(0)
{
  customSerial.begin(9600, CSERIAL_8E1);
}

void PacketReceiver::clearInput()
{
  while (customSerial.available())
    customSerial.read();
}

bool PacketReceiver::processInput()
{
  lastErrorCode=0;
  while (customSerial.available()) {
    uint8_t currentByte = customSerial.read();
    lastInputTime = millis();
    if(currentPacket.length()==0 && currentByte!=0xc9 && currentByte!=0x98)
    {
      lastErrorCode=10+currentByte;
      break;
    }
    if(currentPacket.length()>30)
    {
      lastErrorCode=2;
      break;
    }
    if(currentPacket.length()==0)
    {
      if(currentByte==0x98)
        remainingLength=3;
      else if(currentByte==0xc9)
        remainingLength=5;
    }
    else if(currentPacket.length()==3 && currentPacket.isDataPacket())
    {
      remainingLength=currentByte+2;
    }
    currentPacket.pushByte(currentByte);
    remainingLength--;

    if(remainingLength==0)
    {
      packetQueue.push(&currentPacket);
      currentPacket.reset();
    }
  }
  return lastErrorCode==0;
}

bool PacketReceiver::isPacketAvailable()
{
  return !packetQueue.isEmpty();
}

void PacketReceiver::dequeuePacket(Packet& packet)
{
  packetQueue.pop(&packet);
}

int PacketReceiver::getErrorCode()
{
  return lastErrorCode;
}

unsigned long PacketReceiver::getLastInputTime()
{
  return lastInputTime;
}
