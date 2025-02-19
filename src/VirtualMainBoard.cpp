#include "VirtualMainBoard.h"

void VirtualMainBoard::processPacket(class Packet& packet)
{
  char str[25];
  sprintf(str, "Packet: %d", packet.length());
  Serial.println(str);

  if(packet.isDataPacket() && packet.getByte(4)==0x21 && packet.getByte(6)==0x06)
  {
    fsm.trigger(RECEIVED_HANDSHAKE_RESPONSE);
  }
  if(packet.isDataPacket() && packet.getByte(4)==0x2A && packet.getByte(6)==0x00)
  {
    overallOn=false;
    atLeastOneOn=false;
    for(auto i=0;i<4;i++)
      zoneSetting[i]=0;
    idleEnter();
  }
  if(packet.isDataPacket() && packet.getByte(4)==0x2A && packet.getByte(6)==0x01)
  {
    overallOn=true;
    atLeastOneOn=false;
    for(auto i=0;i<4;i++)
      zoneSetting[i]=0;
    idleEnter();
  }
  if(packet.isDataPacket() && packet.getByte(4)==0x2A && packet.getByte(6)==0x03)
  {
    overallOn=true;
    atLeastOneOn=true;
    for(auto i=0;i<4;i++)
      zoneSetting[i]=packet.getByte(8+i);
    idleEnter();
  }
}

void VirtualMainBoard::handshake0Enter()
{
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  packetReceiver.clearInput();
}

void VirtualMainBoard::handshake1Enter()
{
  digitalWrite(3, LOW);
}

void VirtualMainBoard::handshakeCompleteEnter()
{
  pinMode(3, INPUT);
  packetReceiver.clearInput();

  uint8_t buf[]={0xC9, 0x2C, 0x24, 0x04, 0x21, 0x02, 0x00, 0x02};
  packetSender.sendWithCrc8(buf, sizeof(buf));
}

void VirtualMainBoard::handshakeCompleteExit()
{
  millisAtHandshakeExit = millis();
  uint8_t buf[]={0xC9, 0x2C, 0x24, 0x0B, 0x25, 0x03, 0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0E, 0x0E, 0x00, 0x18};
  packetSender.sendWithCrc8(buf, sizeof(buf));
  Serial.println("handshake complete");
}

void VirtualMainBoard::idleEnter()
{
  unsigned long ts = (millis() - millisAtHandshakeExit)/20+0x18;
  uint8_t overallStatus=0x00;
  if(overallOn)
    overallStatus=0x01;
  if(atLeastOneOn)
    overallStatus=0x03;
  //0xFF - placeholder
  uint8_t buf[]={0xC9, 0x2C, 0x24, 0x0B, 0x25, 0x03, overallStatus, 0xFF, 0xFF, 0xFF, 0xFF, 0x0E, 0x0E, (uint8_t)((ts >> 8) & 0xFF), (uint8_t)(ts & 0xFF)};
  for(auto i=0;i<4;i++)
    buf[7+i]=(uint8_t)(zoneSetting[i]?0x0d:(0x0c | zoneState[i]));
  packetSender.sendWithCrc8(buf, sizeof(buf));
}

VirtualMainBoard::VirtualMainBoard(PacketReceiver& pr, PacketSender& ps) : 
  packetSender(ps), 
  packetReceiver(pr),
  initial(NULL, NULL, NULL),
  handshake0([](void* context){((VirtualMainBoard*)context)->handshake0Enter();}, NULL, NULL),
  handshake1([](void* context){((VirtualMainBoard*)context)->handshake1Enter();}, NULL, NULL),
  handshakeComplete([](void* context){((VirtualMainBoard*)context)->handshakeCompleteEnter();}, NULL, [](void* context){((VirtualMainBoard*)context)->handshakeCompleteExit();}),
  idle([](void* context){((VirtualMainBoard*)context)->idleEnter();}, NULL, NULL),
  fsm(&initial, this)
{
  fsm.add_transition(&initial, &handshake0, START_HANDSHAKE, NULL);
  fsm.add_timed_transition(&handshake0, &handshake1, 130, NULL);
  fsm.add_timed_transition(&handshake1, &handshakeComplete, 50, NULL);
  fsm.add_transition(&handshakeComplete, &idle, RECEIVED_HANDSHAKE_RESPONSE, NULL);
  fsm.add_timed_transition(&idle, &idle, 10000, NULL);
  for(auto i=0;i<4;i++)
    zoneState[i]=0;
}

void VirtualMainBoard::start()
{
  fsm.run_machine();
  pinMode(3, INPUT);
  fsm.trigger(START_HANDSHAKE);
}

void VirtualMainBoard::process()
{
  if(!packetReceiver.processInput())
  {
    Serial.print("Error: ");
    Serial.print(packetReceiver.getErrorCode());
    Serial.println();
  }
  while(packetReceiver.isPacketAvailable())
  {
    Packet packet;
    packetReceiver.dequeuePacket(packet);
    if(!packet.checkCrc8())
      Serial.println("Invalid CRC8!");
    if(packet.isDataPacket())
    {
      packetSender.sendAck(packet.getCrc8());
      processPacket(packet);
    }
  }
  packetSender.processOutput(packetReceiver.getLastInputTime());
  fsm.run_machine();
}

void VirtualMainBoard::setState(Zone zone, ZoneState state)
{
  zoneState[zone]=state;
  char buf[25];
  sprintf(buf, "setState: %02x, %02x", zone, state);
  Serial.println(buf);
  idleEnter();
}

int VirtualMainBoard::getSetting(Zone zone)
{
  return zoneSetting[zone];
}
