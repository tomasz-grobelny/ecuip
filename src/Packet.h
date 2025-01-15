#pragma once
#include <inttypes.h>
#include <stddef.h>

class Packet
{
  uint8_t buffer[50];
  int currentIndex;
  int remainingLength;
public:
  Packet();
  void reset();
  uint8_t getByte(int offset) const;
  bool checkCrc8() const;
  uint8_t getCrc8() const;
  bool isDataPacket() const;
  int length() const;
  void pushByte(uint8_t currentByte);
};
