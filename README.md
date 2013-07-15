TL-WR703N_OpenWrt_LibCurl
=========================

TP-Link WR703N OpenWrt C++ app to test HTTP long pooling to serialport communication (tests)

This is still very experimental and dependents on my 3DPrinterInterface to serve as HTTP Gateway for the Long pooling testing, but the main idea is to hook up the TP-Link TL-WR703N (running OpenWrt custom firmware) with a 3D Printer (Via USB serial link)and run this app to fetch GCODE commands from remote server (aka, 3D Printer Interface) using long pooling via LibCurl library.

WORK_IN_PROGRESS: Managed to remote control the 3d Printer via (updated) https://github.com/nneves/3DPrinterInterface , this interface serves only for testing purpose, HTTP long pooling is provided by the REST api interface on the 3DPI node.js app.js (launch this with the default config profile to use the serial port as /dev/null - aka CloudPrinterServer emulator).

NOTE: This is still higly experimental, so lots of documentation missing (OpenWrt cross-compile environment, etc, but it is possible to compile this under Linux - tested with Debian7) ... 