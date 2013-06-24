/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2011, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <curl/curl.h>
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
#include "JsonParserLib.h"
#include "SerialPortLib.h"
//-----------------------------------------------------------------------------------------
volatile bool terminate = false;
//-----------------------------------------------------------------------------------------
// Thread
int iret1, iret2;
//-----------------------------------------------------------------------------------------
CURL *curl;
CURLcode res;
//-----------------------------------------------------------------------------------------
TSerialPort *spInterface = NULL;
TContainerList *cList = NULL;
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
  //close(fd);
  delete spInterface;
  spInterface = NULL;

  // close CURL
  curl_easy_cleanup(curl);

  return;
}
//-----------------------------------------------------------------------------------------

void test_jsonparser()
{
  // testing JSON Parser Lib
  //const char data[] = "{\"a\":1,\"d\":\"some data\",\"h\":true}";
  //const char data[] = "{\"a\":\"handshake\",\"d\":\"some data\",\"h\":\"true\"}";
  const char data[] = "{\"a\":\"handshake\",\"d\":\"some data\",\"h\":{\"cid\":3,\"t\":2}}";
  //const char data[] = "{\"a\":\"handshake\",\"d\":{\"protocolVersion\":\"1.0\",\"lib\":\"nodejs\",\"userAgent\":\"muzzley-sdk-js\",\"connection\":\"LAN\",\"contentType\":\"application/json\"},\"h\":{\"cid\":1,\"t\":1}}";
  //const char data[] = "{\"a\":{\"b\":{\"c\":\"Complex data\"}},\"d\":{\"protocolVersion\":\"1.0\",\"lib\":\"nodejs\",\"userAgent\":\"muzzley-sdk-js\",\"connection\":\"LAN\",\"contentType\":\"application/json\"},\"h\":{\"cid\":1,\"t\":{\"x\":\"datay\"}}}";

  TJsonParser *pJsonParser = new TJsonParser(data);

  std::string result;
  result = pJsonParser->Prop("a")->Value();
  result = pJsonParser->Prop("d")->Value();
  result = pJsonParser->Prop("h")->ValueJson()->Prop("cid")->Value();
  result = pJsonParser->Prop("h")->Prop("cid")->Value();

  // ERROR testing
  TJsonElement *pjsonelemdemo = pJsonParser->Prop("x");
  result = pJsonParser->Prop("y")->Value();
  TJsonParser *pjsonparser = pJsonParser->Prop("z")->ValueJson();
  TJsonElement *pjsonelem = pJsonParser->Prop("z")->ValueJson()->Prop("cid");
  result = pJsonParser->Prop("z")->ValueJson()->Prop("cid")->Value();
  result = pJsonParser->Prop("z")->Prop("cid")->Value();

  delete pJsonParser;
  pJsonParser = NULL;  
}

void test_containerlist()
{
  TContainerList *contlist = new TContainerList();

  contlist->AddElementID(1, "ABC");
  contlist->AddElementID(2, "DEF");

  contlist->AddElement("AUTODATA");

  contlist->DisplayVectorData();

  delete contlist;
  contlist = NULL;
}

size_t curl_write( void *ptr, size_t size, size_t nmemb, void *stream)
{  
  char *pdata = (char *)ptr;
  std::string strdata;
  strdata = std::string(pdata);

  std::size_t found = strdata.find("\"printercmd\"");
  if(found != std::string::npos)
  {
    debug(("--> %s", strdata.c_str()));

    TJsonParser *pJsonParser = new TJsonParser(strdata.c_str());
    std::string result = pJsonParser->Prop("printercmd")->Value();

    debug(("-->--> %s\n", result.c_str()));

    // add cmd to ContainerList
    cList->AddElement(result.c_str());

/*
    printf("Writing to Serial Port!\n");

    //tcflush(spInterface->GetFD(), TCIOFLUSH); // clear buffer
    write(spInterface->GetFD(), result.c_str(), result.length());
    //tcflush(spInterface->GetFD(), TCIOFLUSH); // clear buffer
*/
    delete pJsonParser;
    pJsonParser = NULL;     
  }
  else
  {
    //printf(".\r\n");
  }

  return size*nmemb;
}
//---------------------------------------------------------------------------------------

static void *ThreadCurlRequest(void *arg)
{
  printf("Init. Thread\n");
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if(curl) {
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    /* get data */
    while(!terminate) 
    {
      curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.110:8081/api/remoteprintercallback/12345");

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

      ((TSerialPort*)arg)->WriteDataLine("G91\n");
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  debug(("Leaving Thread\n"));
  pthread_exit(&iret1);
  //return NULL;  
}
//---------------------------------------------------------------------------------------

int main(void)
{
  //---------------------------------------------------------------------------------------
  // variables for threads
  //---------------------------------------------------------------------------------------
  pthread_t thread1, thread2;
  //int  iret1, iret2;
  //---------------------------------------------------------------------------------------
  spInterface = new TSerialPort();
  cList = new TContainerList();

  //---------------------------------------------------------------------------------------
  // aux variables for serial data parsing and grouping
  //---------------------------------------------------------------------------------------
  char temp_buf[2];
  //char str[1024];


  //---------------------------------------------------------------------------------------
  // test containerlist
  //---------------------------------------------------------------------------------------
  //test_containerlist();

  //---------------------------------------------------------------------------------------
  // test jsonparser
  //---------------------------------------------------------------------------------------
  //test_jsonparser();

  signal(SIGTERM, cleanup);
  signal(SIGINT, cleanup);
  signal(SIGKILL, cleanup);

  //---------------------------------------------------------------------------------------
  // launch thread APIs (extra interfaces)
  //---------------------------------------------------------------------------------------
  /* Create independent threads each of which will execute function */
  //iret1 = pthread_create( &thread1, NULL, ThreadCurlRequest, (void*) NULL);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  //pthread_join( thread1, NULL);

  //debug(("Thread 1 returns: %d\n",iret1));
  //---------------------------------------------------------------------------------------
  
 
  //---------------------------------------------------------------------------------------
  // main loop
  std::string result;
  std::string cmd;

  //tcflush(*spInterface->GetFD(), TCIOFLUSH); // clear buffer

  debug(("Starting main loop\n"));
  while(!terminate) 
  { 
    debug(("»"));

    // write data to printer
    if(cList->Count() > 0)
    {
      cmd = cList->PopFirstElement();

      debug(("Writing to Serial Port: %s\n", cmd.c_str()));

      spInterface->WriteDataLine(cmd);
    }

    if(spInterface->ReadDataLine())
    {
      result = spInterface->GetDataLine();
      spInterface->ClearDataLine();
      debug(("%s\n", result.c_str()));

      if(result.find("echo:SD init fail")!=std::string::npos)
      {
        debug(("Found Printer Init final cmd\nLaunch LibCURL Thread!\n"));
        iret1 = pthread_create( &thread1, NULL, ThreadCurlRequest, (void*) spInterface/*NULL*/);
        //pthread_join( thread1, NULL);
        debug(("Returned from Thread!\n"));

        spInterface->WriteDataLine("G91\n");
      }
    }    
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

  if(cList)
  {
    cList->DisplayVectorData();
    delete cList;
    cList = NULL;    
  }
  //---------------------------------------------------------------------------------------
  
  return 0;
}