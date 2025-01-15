# ECUIP
Electrolux Cooktop User Interface Protocol

## Background
Electrolux Cooktop/Hob home appliances consist of 3 main parts:
 * User interface board (UIB)
 * Main power board (MPB)
 * Effectors

What's interesting from the point of view of this project is that we can sniff and alter communication between UIB and MPB to achieve additional functionality of the cooktop.

## Devices
This project delas with both electric (ceramic) and indiction cooktops make by Electrolux. This should potentially also include AEG. Devices that will be of primary interest for me are the following:
 * EHF6747FOK - this is an old device from around 2011, for now it will be main investigation and development device. Primary investigation done and all further technical details will be based on this device.
 * CIV634 - this is a newer device I bought around 2021 and still available in shops as of December 2024. I haven't yet opened this device, but my hope is that whatever I find about EHF6747FOK can also be applied for this device.

If you have any (potentially broken) Electrolux electric/induction cooktop/hob device to spare for the project please let me know. We might want to:
 * Get the device sent to me - especially if you are located in Poland or nearby - otherwise the logistic costs might not be worth it,
 * Arrange for a session to collect device specific information on the spot where the device is located; note this requires some technical knowledge and (relatively cheap) equipment on your part.

## Objectives
There are two main objectives of the project:
 * Provide reasonably complete protocol information
 * Implement device that could be put between UIB and MPB and provide additional functionality such as:
   * reading detailed information about cooktop usage, eg. consider turning hood (makes sense for devices that don't have IR communication aka hob2hood),
   * dynamic power output adjustment, eg. consider sous vide cooking,
   * turning device on/off remotely, eg. to make you dinner ready when you are back home. 

## Current status
Preliminary protocol investigation has been done. While not all details are 100% understood yet, the understanding should be enough to start doing some proof of concept implementation. Due to limited time for this project do not expect working code anytime soon. For details of what is known at this stage please refer to [FINDINGS](FINDINGS.md) page.
