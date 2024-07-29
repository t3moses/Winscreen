# Winscreen
 Winscreen code

The Winscreen product comprises hardware and software for an instrumented sailboat.

It implements an interface into the Raymarine Seatalk bus for reading boat speed and heading,
apparent wind speed and angle and rudder angle.

It also contains a GNSS module and an interface to an active antenna for reading SOG and COG.

It provides a Wi-FI access point and HTTP server.

These facilities allow for processing of basic boat data and display of the resulting information
on a connected Wi-Fi device.

The hardware is based on an Espressif ESP32C3.  The source code is C++.

To dos

1. Get all pages to display.
2. Get SNS and GRS directly from the GNSS module instead of from a series of fixes.
3. Calculate and display course based on GRS.
4. Calculate and display leeway based on GRS.
5. Remove the wind-up and bow-up displays.

