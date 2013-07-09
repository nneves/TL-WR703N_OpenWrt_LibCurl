TL-WR703N_OpenWrt_LibCurl
=========================

TP-Link WR703N OpenWrt C++ app to test HTTP long pooling to serialport communication (tests)

This is still very experimental and dependents on my 3DPrinterInterface to serve as HTTP Gateway for the Long pooling testing, but the main idea is to hook up the TP-Link TL-WR703N (running OpenWrt custom firmware) with a 3D Printer (Via USB serial link)and run this app to fetch GCODE commands from remote server (aka, 3D Printer Interface) using long pooling via LibCurl library.
