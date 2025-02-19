# Findings
This page lists findings while working with Electrolux/AEG electric/induction cooktop/hob devices.

## Disclaimers
To the extent permitted by law I take no responsibility for any damage caused by following advice in this repository. You do everything on your own risk.

## Opening device
Tools you might need:
 * flat screwdriver for main hooks,
 * torx screwdriver for power connection box.

Suggested opening procedure:
 * turn the device upside down,
 * unhook all the hooks on the left, front and right side of the hob - there is no need to anything on the back side - it will act like a hinge,
 * before opening the device turn the device to normal position
 * lift the glass and take out the user interface board

## Hardware interface
The Main Power Board is connected to User Interface board using 3 wires labelled as GND, 5V and BUS. If you need a replacement, you can buy one [here](https://north.pl/karta/wiazka-kabli-do-plyty-indukcyjnej-do-electrolux-civ634-nk4,7EK-0E-C2M2-S0M.html). The cable uses Stocko Contact connectors [7234](https://www.stocko-contact.com/en/products-connector-system-pitch-2.5-mm-eco-tronic-mf-7234.php) (MPB side, keyed) and [7238](https://www.stocko-contact.com/en/products-connector-system-pitch-2.5-mm-eco-tronic-mf-7238.php) (UIB side, latched by socket). Unfortunately I haven't found any retailers for those contacts.

## Electrical interface, tooling and transport protocol
What we can conclude is that we have single wire for both TX and RX. My assumption (not confirmed yet by looking at oscilloscope) is that we are using 5V signalling. To analyze the protocol I cut the wires and connected cheap (about 10eur) 24Mhz logic analyzer (Saleae 8 clone) and [Pulseview](https://sigrok.org/wiki/PulseView).

Thanks to help from [Electrical Engineering user](https://electronics.stackexchange.com/questions/733542/tool-for-protocol-analysis) I checked that low level the protocol is UART with inverted input, 9600 baud rate, 8 bit data, one stop bit and odd parity. Applying these settings allows to analyze the protocol as 2 digit hex numbers from now on.

## Sample session dump
This session was collected by pressing the power button, pressing 4 for one of the fields, pressing 0 for the same field and pressing power button again.

ping:

C9 2C 24 0B 25 03 00 0C 0C 0C 0C 0E 0E C4 FD D5 -> 98 D5 4D

C9 24 2C 0C 2A 00 00 00 00 00 00 00 00 00 88 DD B2 -> 98 B2 2A

on:

C9 24 2C 0C 2A 00 01 00 00 00 00 00 00 00 CF 0E 27 -> 98 27 BF

C9 2C 24 0B 25 03 01 0C 0C 0C 0C 0E 0E C5 9A B2 -> 98 B2 2A

4:

C9 24 2C 0C 2A 00 03 00 **0C 00 00 00** 00 00 4B 4B E8 -> 98 E8 70

C9 2C 24 0B 25 03 03 **0D 0C 0C 0C** 0E 0E C6 0E 26 -> 98 26 BE

0:

C9 24 2C 0C 2A 00 01 00 **00 00 00 00** 00 00 CF 0E 27 -> 98 27 BF

C9 2C 24 0B 25 03 01 **0C 0C 0C 0C** 0E 0E C6 BC 97 -> 98 97 0F

off:

C9 24 2C 0C 2A 00 00 00 00 00 00 00 00 00 88 DD B2 -> 98 B2 2A

C9 2C 24 0B 25 03 00 0C 0C 0C 0C 0E 0E C7 53 78 -> 98 78 E0

ping:

C9 2C 24 0B 25 03 00 0C 0C 0C 0C 0E 0E C9 3B 1E -> 98 1E 86

C9 24 2C 0C 2A 00 00 00 00 00 00 00 00 00 88 DD B2 -> 98 B2 2A

## Protocol
Here are a couple of observations:
 * Messages can be initiated either by MPB and UIB
 * Ping is sent by both UIB and MPB roughly 10 seconds from last communication; "roughly" here means that the tmers seem to desynchronize - when there was user activity recently the pings from UIB and MPB are close to each other, when they were not so recent then they seem to be further apart
 * Each message starts with C9 byte and is confirmed by the receiving end with a message startng with 98 (ACK)
 * Next two bytes of the message denote direction with 24 2C meaning UIB to MPB and 2C 24 meaning MPB to UIB
 * Next byte seems to be length of the remaining message; "seems" here means that this theory matches the two examples I could collect (see 0B and 0C above)
 * Next few bytes seems to be action/state specific, 2A/25 seem to be fixed depending on message direction, a few protocol fields corresponding to cooking zones (in bold), note that UI values may need conversion to protocol values, eg. for my hob it looks like this:
   * 0->00
   * 1->02
   * 2->08
   * 3->0B
   * 4->0C
   * 5->0E
   * 6->0F
   * 7->10
   * 8->11
   * 9->12
   * 10->13
   * 11->14
   * 12->15
   * 13->16
   * 14->17
   * on top of that for some zones you may get +0x20 or +0x60 if it has subzones (pot size setting)
 * For MPB -> UIB communication the fields in bold contain eg. info about zone temperature (hot zone: 8C => 1 bar, 4c => 2 bars, cc => 3 bars), 0D is not clear to me, but maybe it just indicates that it is heating?
 * The last byte, for both main message and ACK is XOR of all previous bytes
 * ACK consists of byte 98, last byte of the acknowledged message and then XOR of the last two bytes
 * Bytes 3 and 2 from the end of the message seem to have different meaning depending on direction:
   * UIB -> MPB: CRC of all previous bytes (CRC RevEng identified it as: width=16  poly=0x1021  init=0x1d0f  refin=false  refout=false  xorout=0x0000  check=0xe5cc  residue=0x0000  name="CRC-16/SPI-FUJITSU"), see [calcCrc.cpp](calcCrc.cpp) for sample code
   * MPB -> UIB: timestamp with 20ms resolution
 
