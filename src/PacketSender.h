#pragma once
#include <CustomSoftwareSerial.h>

class PacketSender
{
  void send(uint8_t* buffer, size_t size);
  CustomSoftwareSerial& customSerial;
public:
  PacketSender(CustomSoftwareSerial& css);
  void sendAck(uint8_t crc8);
  void sendWithCrc8(uint8_t* buffer, size_t size);
};
