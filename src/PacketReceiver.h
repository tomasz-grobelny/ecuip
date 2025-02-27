#pragma once
#include <CustomSoftwareSerial.h>
#include <cppQueue.h>
#include "Packet.h"

class PacketReceiver
{
  CustomSoftwareSerial& customSerial;
  Packet currentPacket;
  int remainingLength;
  cppQueue packetQueue;
  int lastErrorCode;
  unsigned long lastInputTime=0;
public:
  PacketReceiver(CustomSoftwareSerial& css);
  void clearInput();
  bool processInput();
  bool isPacketAvailable();
  void dequeuePacket(Packet& packet);
  int getErrorCode();
  unsigned long getLastInputTime();
};
