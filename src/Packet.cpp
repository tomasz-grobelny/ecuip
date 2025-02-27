#include "Packet.h"
#include <Arduino.h>

Packet::Packet()
{
  reset();
}

Packet::Packet(const uint8_t* inputBuffer, size_t size)
{
  memcpy(buffer, inputBuffer, size);
  currentIndex = size;
}

void Packet::reset()
{
  currentIndex=0;
}

uint8_t Packet::getByte(int offset) const
{
  return buffer[offset];
}

bool Packet::checkCrc8() const
{
  uint8_t pxor=0;
  for(auto i=0;i<currentIndex;i++)
  {
    pxor = pxor ^ buffer[i];
  }
  return pxor==0;
}

uint8_t Packet::getCrc8() const
{
  return buffer[currentIndex-1];
}

bool Packet::isDataPacket() const
{
  return buffer[0]==0xc9;
}

int Packet::length() const
{
  return currentIndex;
}

const uint8_t* Packet::getBuffer() const
{
  return buffer;
}

void Packet::pushByte(uint8_t currentByte)
{
  buffer[currentIndex++]=currentByte;
}
