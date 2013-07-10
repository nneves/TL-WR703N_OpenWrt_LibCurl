#ifndef __SERIALPORTLIBH
#define __SERIALPORTLIBH
//---------------------------------------------------------------------------------------------
// Serial Port Lib class
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date:      25-May-2013
// Objective: Serial Port Warpper Class
//
//---------------------------------------------------------------------------------------------
#include <vector>
#include <string>
//---------------------------------------------------------------------------------------------

class TSerialPort
{
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  int fd;
  bool terminate;
  //bool rx_data_available;
  struct termios oldtio,newtio;
  struct sigaction saio;         // definition of signal action
  //---------------------------------------------------------------------------------------
  // aux variables for serial data parsing and grouping
  //---------------------------------------------------------------------------------------
  //char str[1024];
  
  #define MAXBUFSIZE 4096
  //char buffer[MAXBUFSIZE];
  std::string cmddata;

  //void signal_handler_IO(int status);
  char ReadDataChar();

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TSerialPort();
  ~TSerialPort();

  int *GetFD();
  bool ReadDataLine();
  std::string GetDataLine();
  void ClearDataLine();
  void WriteDataLine(std::string cmd);
  void TestPrinterGCODE();
  void FlushBuffer();

};
//---------------------------------------------------------------------------------------------
#endif