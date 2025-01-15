#pragma once
#include <CustomSoftwareSerial.h>
#include "Fsm.h"
#include <cppQueue.h>
#include "Packet.h"
#include "PacketSender.h"
#include "PacketReceiver.h"

#define START_HANDSHAKE 1
#define RECEIVED_HANDSHAKE_RESPONSE 2
#define SEND_RESPONSE 3

enum Zone
{
  Zone1 = 0,
  Zone2 = 1,
  Zone3 = 2,
  Zone4 = 3,
};

enum ZoneState
{
  Normal = 0,
  Hot1 = 0x80,
  Hot2 = 0x40,
  Hot3 = 0xc0,
};

class VirtualMainBoard
{
  PacketSender& packetSender;
  PacketReceiver& packetReceiver;
  int zoneSetting[4];
  int zoneState[4];

  bool overallOn=false;
  bool atLeastOneOn=false;

  State initial;
  State handshake;
  State idle;
  Fsm fsm;

  void processPacket(class Packet& packet);

  static void handshakeEnterWrapper(void* context)
  {
    ((VirtualMainBoard*)context)->handshakeEnter();
  }
  void handshakeEnter();

  unsigned long millisAtHandshakeExit;
  static void handshakeExitWrapper(void* context)
  {
    ((VirtualMainBoard*)context)->handshakeExit();
  }
  void handshakeExit();

  static void idleEnterWrapper(void* context)
  {
    ((VirtualMainBoard*)context)->idleEnter();
  }
  void idleEnter();

  void initComm();
public:
  VirtualMainBoard(PacketReceiver& pr, PacketSender& ps);
  void start();
  void process();
  void setState(Zone zone, ZoneState state);
  int getSetting(Zone zone);
};
