#include <CustomSoftwareSerial.h>
#include "Packet.h"
#include "PacketSender.h"
#include "PacketReceiver.h"
#include "VirtualMainBoard.h"

CustomSoftwareSerial customSerial(2, 3,  true);
PacketSender packetSender(customSerial);
PacketReceiver packetReceiver(customSerial);
VirtualMainBoard virtualMainBoard(packetReceiver, packetSender);

long lastDisplayed = 0;
Zone currentZone;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello to ECUIP");
  virtualMainBoard.start();
}

void loop() {
  virtualMainBoard.process();
  auto now = millis();
  if(now-lastDisplayed > 3000)
  {
    lastDisplayed=now;
    char buf[25];
    sprintf(buf, "z1: %02x, z2: %02x, z3: %02x, z4: %02x", virtualMainBoard.getSetting(Zone1), virtualMainBoard.getSetting(Zone2), virtualMainBoard.getSetting(Zone3), virtualMainBoard.getSetting(Zone4));
    Serial.println(buf);
  }
  while(Serial.available())
  {
    auto ch=Serial.read();
    char buf[25];
    sprintf(buf, "got: %02x", ch);
    Serial.println(buf);
    if(ch>='a' && ch<='d')
      currentZone=(Zone)(ch-'a'+Zone1);
    if(ch=='0')
      virtualMainBoard.setState(currentZone, Normal);
    if(ch=='1')
      virtualMainBoard.setState(currentZone, Hot1);
    if(ch=='2')
      virtualMainBoard.setState(currentZone, Hot2);
    if(ch=='3')
      virtualMainBoard.setState(currentZone, Hot3);
  }
}
