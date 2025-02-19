#pragma once
#include <CustomSoftwareSerial.h>
#include <cppQueue.h>

class PacketSender
{
  void send(const uint8_t* buffer, size_t size);
  void queue(const uint8_t* buffer, size_t size);
  CustomSoftwareSerial& customSerial;
  cppQueue packetQueue;
  unsigned long lastOutputTime;
public:
  PacketSender(CustomSoftwareSerial& css);
  void sendAck(uint8_t crc8);
  void sendWithCrc8(uint8_t* buffer, size_t size);
  void processOutput(unsigned long lastInputTime);
};
