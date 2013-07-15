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
  
  #define MAXRXSIZE 1024
  #define MAXBUFSIZE 4096

  std::string cmddata;

  //-------------------------------------------------------------------------------------------
  public:
  //-------------------------------------------------------------------------------------------
  TSerialPort(__sighandler_t psignal_handler_io);
  ~TSerialPort();

  void signal_handler_IO(int status);

  int *GetFD();
  int ReadRxData();
  std::string GetData();
  void ClearData();
  void WriteData(std::string cmd);
  void TestPrinterGCODE();
  void FlushBuffer();
};
//---------------------------------------------------------------------------------------------
#endif