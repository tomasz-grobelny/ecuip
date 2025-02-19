#include <Arduino.h>
#include "PacketSender.h"
#include "Packet.h"

void PacketSender::send(const uint8_t* buffer, size_t size)
{
  pinMode(3, OUTPUT);
  customSerial.write(buffer, size);
  pinMode(3, INPUT);
}

PacketSender::PacketSender(CustomSoftwareSerial& css) : customSerial(css), packetQueue(sizeof(Packet), 10, FIFO)
{
  customSerial.begin(9600, CSERIAL_8E1);
  lastOutputTime = 0;
}

void PacketSender::sendAck(uint8_t crc8)
{
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
  queue(newBuf, size+1);
}

void PacketSender::queue(const uint8_t* buffer, size_t size)
{
  Packet packet(buffer, size);
  packetQueue.push(&packet);
}

void PacketSender::processOutput(unsigned long lastInputTime)
{
  auto now = millis();
  if(now - lastInputTime > 5 && now - lastOutputTime > 5 && !packetQueue.isEmpty())
  {
    Packet packet;
    packetQueue.pop(&packet);
    send(packet.getBuffer(), packet.length());
    lastOutputTime = millis();
  }
}
