#ifndef __THREADPRINTERH
#define __THREADPRINTERH
//---------------------------------------------------------------------------------------------
// Printer Thread
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date:      18-July-2013
// Objective: Printer Thread class
//
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
class TContainerList;
class TSerialPort;
//---------------------------------------------------------------------------------------------

namespace NSPrinter
{
  void signal_handler_IO(int status);
}
//---------------------------------------------------------------------------------------------

class TThreadPrinter
{  
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  pthread_t thread2;

  //-------------------------------------------------------------------------------------------
  public:  
  //-------------------------------------------------------------------------------------------
  int  iret2;  
  bool terminate;

  TContainerList *containerlist;
  TSerialPort *spinterface;

  TThreadPrinter(TContainerList *containerlist=NULL, const char *devicename=NULL);
  ~TThreadPrinter();

  int StartThread();
  void TerminateThread();

  void SendData(const char *gcodecmd);
};
//---------------------------------------------------------------------------------------------
#endif