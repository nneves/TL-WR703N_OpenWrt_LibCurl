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
//#include <string>
//-----------------------------------------------------------------------------------------
#include "ContainerLib.h"
#include "SerialPortLib.h"
#include "ThreadLibCurl.h"
#include "ThreadPrinter.h"
//-----------------------------------------------------------------------------------------
namespace NSInterfaces
{
  //---------------------------------------------------------------------------------------
  volatile bool terminate = false;
  //---------------------------------------------------------------------------------------
  TSerialPort *spInterface = NULL;
  TContainerList *cList = NULL;
  TThreadLibCurl *thrdLibCurl = NULL;
  TThreadPrinter *thrdPrinter = NULL;
  //---------------------------------------------------------------------------------------
  void clearobjects()
  {
    // destroy objects
    if(spInterface)
    {
      delete spInterface;
      spInterface = NULL;
    }

    if(thrdLibCurl)
    {
      delete thrdLibCurl;
      thrdLibCurl = NULL; 
    }

    if(thrdPrinter)
    {
      delete thrdPrinter;
      thrdPrinter = NULL;
    }  

    if(cList)
    {
      cList->DisplayVectorData();
      delete cList;
      cList = NULL;    
    }
  }
  //---------------------------------------------------------------------------------------  
}
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 1
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif
//-----------------------------------------------------------------------------------------

void cleanup(int sig)
{
  debug(("recieved signal %d\n", sig));
  NSInterfaces::terminate = true;

  NSInterfaces::clearobjects();

  return;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
int main(void)
{
  //---------------------------------------------------------------------------------------
  std::string url = "http://192.168.1.110:8081/api/remoteprintercallback/12345";

  NSInterfaces::spInterface = new TSerialPort(NSPrinter::signal_handler_IO);
  NSInterfaces::cList = new TContainerList();
  NSInterfaces::thrdLibCurl = new TThreadLibCurl(NSInterfaces::cList, url.c_str());
  NSInterfaces::thrdPrinter = new TThreadPrinter(NSInterfaces::cList, NSInterfaces::spInterface);
 
  signal(SIGTERM, cleanup);
  signal(SIGINT, cleanup);
  signal(SIGKILL, cleanup);

  //---------------------------------------------------------------------------------------
  // Launch Threads
  //---------------------------------------------------------------------------------------  
  NSInterfaces::thrdLibCurl->StartThread();
  NSInterfaces::thrdPrinter->StartThread();
 
  //---------------------------------------------------------------------------------------
  // main loop

  debug(("Starting main loop\n"));
  sleep(10);
  // send initial cmd (relative move), forces printer to respond 
  NSInterfaces::spInterface->WriteData("G91\n");

  std::string cmd;
  while(!NSInterfaces::terminate) 
  {
    debug(("©\n")); // heart beat output
    sleep(5);
  
  }// end while - main loop
  //---------------------------------------------------------------------------------------
  debug(("Exit mainloop\n"));

  NSInterfaces::clearobjects();
  //---------------------------------------------------------------------------------------
  
  return 0;
}