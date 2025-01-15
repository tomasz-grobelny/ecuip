#include <Arduino.h>
#include "PacketSender.h"

void PacketSender::send(uint8_t* buffer, size_t size)
{
  pinMode(3, OUTPUT);
  customSerial.write(buffer, size);
  pinMode(3, INPUT);
}

PacketSender::PacketSender(CustomSoftwareSerial& css) : customSerial(css)
{
  customSerial.begin(9600, CSERIAL_8E1);
}

void PacketSender::sendAck(uint8_t crc8)
{
  delay(5);
  uint8_t buf[]={0x98, crc8};
  sendWithCrc8(buf, sizeof(buf));
}

void PacketSender::sendWithCrc8(uint8_t* buffer, size_t size)
{
  uint8_t newBuf[size+1];
  memcpy(newBuf, buffer, size);
  uint8_t crc8=0;
  for(auto i=0;i<size;i++)
    crc8=crc8^buffer[i];
  newBuf[size]=crc8;
  send(newBuf, size+1);
}
