#pragma once
#include <inttypes.h>
#include <stddef.h>

class Packet
{
  uint8_t buffer[31];
  int currentIndex;
public:
  Packet();
  Packet(const uint8_t* buffer, size_t size);
  void reset();
  uint8_t getByte(int offset) const;
  bool checkCrc8() const;
  uint8_t getCrc8() const;
  bool isDataPacket() const;
  int length() const;
  const uint8_t* getBuffer() const;
  void pushByte(uint8_t currentByte);
};
