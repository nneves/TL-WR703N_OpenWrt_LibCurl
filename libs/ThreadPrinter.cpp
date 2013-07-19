 //-----------------------------------------------------------------------------------------
// Thread
#include <pthread.h>
// Http Server
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
// GPS Parser
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <string.h> // strncpy strnstr strcmp
#include <signal.h>
//-----------------------------------------------------------------------------------------
#include <string>
//-----------------------------------------------------------------------------------------
#include "ContainerLib.h"
#include "SerialPortLib.h"
#include "ThreadPrinter.h"
//-----------------------------------------------------------------------------------------
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 1
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif
//-----------------------------------------------------------------------------------------
// NSPrinter NAMESPACE 
//-----------------------------------------------------------------------------------------
namespace NSPrinter
{
//-----------------------------------------------------------------------------------------
  TThreadPrinter *printerparent = NULL;
  volatile int printer_ack = 0;
//-----------------------------------------------------------------------------------------
/*static */
void *ThreadPrinterCmd(void *arg)
{  
  printerparent = ((TThreadPrinter*)arg);

  printf("Initialize SerialCom Thread\n");
  std::string cmd;
  
  // get data
  while(!printerparent->terminate) 
  {
    // checks if printer is ready to recieve cmds
    if(printer_ack <= 0)
    {
      usleep(1000); // wait 1ms
      continue;
    }

    // check if Container List has data to be send
    if(printerparent->containerlist->Count() <= 0)
    {
      usleep(1000); // wait 1ms
      continue;
    }

    // printer is ready and containerlist data is available to be sent
    printer_ack--;

    cmd = printerparent->containerlist->PopFirstElement();
    debug(("=======================================================================================\n"));
    debug(("ThreadPrinterCmd: Found ContainerList Data, Writing to Serial Port: %s\n", cmd.c_str()));
    debug(("=======================================================================================\n"));

    printerparent->spinterface->WriteData(cmd);    
  }

  debug(("Leaving SerialCom Thread\n"));
  pthread_exit(&(printerparent->iret2));
  return NULL; 
}
//-----------------------------------------------------------------------------------------

/***************************************************************************
* signal handler. sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/

void signal_handler_IO(int status)
{
  if(!printerparent || !printerparent->spinterface)
    return;

  debug(("Received Serial Port SIGIO signal.\n"));
  debug(("SIGIO_IN\n"));
  while(printerparent->spinterface->ReadRxData() > 0);
  debug(("SIGIO_OUT\n"));

  // serach for "ok\n" ack string
  if(printerparent->spinterface->GetData().find("ok\n")!=std::string::npos)
  {
    debug(("signal_handler_IO: Found OK response from printer:\n%s", 
      printerparent->spinterface->GetData().c_str()));
    printerparent->spinterface->ClearData();
    printer_ack++;
  }
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------

// constructor 
TThreadPrinter::TThreadPrinter(TContainerList *clist)
{
  debug(("Initialize Thread Printer Object\n"));
 
  terminate = false;

  if(clist)
    containerlist = clist;

  spinterface = new TSerialPort(NSPrinter::signal_handler_IO);

  //debug(("Thread Serial Device: %s\n", device_name.c_str()));
}  
//-----------------------------------------------------------------------------------------
// destructor 
TThreadPrinter::~TThreadPrinter()
{
  debug(("Destroy Thread Printer Object\n"));

  TerminateThread();
}
//-----------------------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------------------

int TThreadPrinter::StartThread()
{
  debug(("Printer Thread: StartThread!\n"));

  //---------------------------------------------------------------------------------------
  // launch thread APIs (extra interfaces)
  //---------------------------------------------------------------------------------------
  /* Create independent threads each of which will execute function */
  //iret2 = pthread_create( &thread2, NULL, ThreadPrinterCmd, (void*) NULL);
  iret2 = pthread_create(&thread2, NULL, NSPrinter::ThreadPrinterCmd, (void*)this);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  //pthread_join( thread2, NULL);

  debug(("Printer Thread returns: %d\n",iret2));

  return iret2;
}
//-----------------------------------------------------------------------------------------

void TThreadPrinter::TerminateThread()
{
  terminate = true;
}
//-----------------------------------------------------------------------------------------