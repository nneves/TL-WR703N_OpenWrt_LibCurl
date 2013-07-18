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
//-----------------------------------------------------------------------------------------
volatile bool terminate = false;
volatile int printer_ack = 0;
//-----------------------------------------------------------------------------------------
// Thread
int iret1, iret2;
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
TSerialPort *spInterface = NULL;
TContainerList *cList = NULL;
TThreadLibCurl *thrdLibCurl = NULL;
//-----------------------------------------------------------------------------------------
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
  terminate = true;

  // close serial port
  if(spInterface)
  {
    delete spInterface;
    spInterface = NULL;
  }

  // close CURL
  if(thrdLibCurl)
  {
    delete thrdLibCurl;
    thrdLibCurl = NULL; 
  }

  if(cList)
  {
    cList->DisplayVectorData();
    delete cList;
    cList = NULL;    
  }

  return;
}
//-----------------------------------------------------------------------------------------

static void *ThreadPrinterCmd(void *arg)
{  
  printf("Initialize SerialCom Thread\n");
  std::string cmd;
  
  // get data
  while(!terminate) 
  {
    // checks if printer is ready to recieve cmds
    if(printer_ack <= 0)
    {
      usleep(1000); // wait 1ms
      continue;
    }

    // check if Container List has data to be send
    if(cList->Count() <= 0)
    {
      usleep(1000); // wait 1ms
      continue;
    }

    // printer is ready and cList data is available to be sent
    printer_ack--;

    cmd = cList->PopFirstElement();
    debug(("=======================================================================================\n"));
    debug(("ThreadPrinterCmd: Found ContainerList Data, Writing to Serial Port: %s\n", cmd.c_str()));
    debug(("=======================================================================================\n"));

    spInterface->WriteData(cmd);    
  }

  debug(("Leaving SerialCom Thread\n"));
  pthread_exit(&iret2);
  //return NULL; 
}
//---------------------------------------------------------------------------------------

/***************************************************************************
* signal handler. sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/

void signal_handler_IO(int status)
{
  debug(("Received Serial Port SIGIO signal.\n"));
  debug(("SIGIO_IN\n"));
  while(spInterface->ReadRxData() > 0);
  debug(("SIGIO_OUT\n"));

  // serach for "ok\n" ack string
  if(spInterface->GetData().find("ok\n")!=std::string::npos)
  {
    debug(("signal_handler_IO: Found OK response from printer:\n%s", spInterface->GetData().c_str()));
    spInterface->ClearData();
    printer_ack++;
  }
}
//---------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------- 
int main(void)
{
  //---------------------------------------------------------------------------------------
  // variables for threads
  //---------------------------------------------------------------------------------------
  pthread_t thread1, thread2;
  //int  iret1, iret2;
  //---------------------------------------------------------------------------------------
  std::string url = "http://192.168.1.110:8081/api/remoteprintercallback/12345";

  spInterface = new TSerialPort(signal_handler_IO);
  cList = new TContainerList();
  thrdLibCurl = new TThreadLibCurl(cList, url.c_str());
 
  signal(SIGTERM, cleanup);
  signal(SIGINT, cleanup);
  signal(SIGKILL, cleanup);

  //---------------------------------------------------------------------------------------
  // Launch LibCurl Thread
  //---------------------------------------------------------------------------------------  
  thrdLibCurl->StartThread();

  //---------------------------------------------------------------------------------------
  // launch thread APIs (extra interfaces)
  //---------------------------------------------------------------------------------------
  /* Create independent threads each of which will execute function */
  //iret2 = pthread_create( &thread2, NULL, ThreadPrinterCmd, (void*) NULL);
  iret2 = pthread_create( &thread2, NULL, ThreadPrinterCmd, (void*)spInterface);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  //pthread_join( thread2, NULL);

  debug(("Thread 2 returns: %d\n",iret2));
  //---------------------------------------------------------------------------------------  
 
  //---------------------------------------------------------------------------------------
  // main loop

  debug(("Starting main loop\n"));
  sleep(10);
  // send initial cmd (relative move), forces printer to respond 
  spInterface->WriteData("G91\n");

  std::string cmd;
  while(!terminate) 
  {
    debug(("©\n")); // heart beat output
    sleep(5);

    /*
    for(int j=0; j<3; ++j)
    {
    spInterface->WriteData("G1 X10.000000 Y0.000000 Z0.000000 F4000\n");
    sleep(1); //usleep(500000);
    }

    for(int j=0; j<3; ++j)
    {
    spInterface->WriteData("G1 X0.000000 Y10.000000 Z0.000000 F4000\n");
    sleep(1); //usleep(500000);
    }

    for(int j=0; j<3; ++j)
    {
    spInterface->WriteData("G1 X-10.000000 Y0.000000 Z0.000000 F4000\n");
    sleep(1); //usleep(500000);
    }

    for(int j=0; j<3; ++j)
    {
    spInterface->WriteData("G1 X0.000000 Y-10.000000 Z0.000000 F4000\n");
    sleep(1); //usleep(500000);
    }
    */
  
  }// end while - main loop
  //---------------------------------------------------------------------------------------
  debug(("Exit mainloop\n"));

  //---------------------------------------------------------------------------------------
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

  if(cList)
  {
    cList->DisplayVectorData();
    delete cList;
    cList = NULL;    
  }
  //---------------------------------------------------------------------------------------
  
  return 0;
}