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
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <string.h> // strncpy strnstr strcmp
#include <signal.h>
//-----------------------------------------------------------------------------------------
#include <string>
//-----------------------------------------------------------------------------------------
#include "JsonParserLib.h"
#include "ContainerLib.h"
#include "ThreadLibCurl.h"
//-----------------------------------------------------------------------------------------
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 1
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif

//-----------------------------------------------------------------------------------------
// NSCurl NAMESPACE function and callback
//-----------------------------------------------------------------------------------------
namespace NSCurl
{
TThreadLibCurl *curlparent = NULL;

/*static*/
size_t curl_write(void *ptr, size_t size, size_t nmemb, void *stream)
{  
  char *pdata = (char *)ptr;
  std::string strdata;
  strdata = std::string(pdata);

  std::size_t found = strdata.find("\"printercmd\"");
  if(found != std::string::npos)
  {
    debug(("--> %s\n", strdata.c_str()));

    TJsonParser *pJsonParser = new TJsonParser(strdata.c_str());
    std::string result = pJsonParser->Prop("printercmd")->Value();

    debug(("-->--> %s\n", result.c_str()));

    // add cmd to ContainerList
    debug(("Adding CURL response data to ContainerList\n"));
    if(curlparent->containerlist)
      curlparent->containerlist->AddElement(result.c_str());

    delete pJsonParser;
    pJsonParser = NULL;     
  }
  else
  {
    //printf(".\r\n");
  }

  return size*nmemb;
}
//-----------------------------------------------------------------------------------------

/*static*/
void* ThreadCurlRequest(void *arg)
{
  curlparent = ((TThreadLibCurl*)arg);

  debug(("Initialize Curl Request Thread\n"));
  curl_global_init(CURL_GLOBAL_ALL);

  curlparent->curl = curl_easy_init();
  if(curlparent->curl) {
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curlparent->curl, CURLOPT_HEADER, 1L);

    curl_easy_setopt(curlparent->curl, CURLOPT_WRITEFUNCTION, curl_write);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    // get data
    while(!curlparent->terminate) 
    {
      debug(("Making CURL HTTP GET request: %s\n", curlparent->rest_url.c_str()));
      curl_easy_setopt(curlparent->curl, CURLOPT_URL, curlparent->rest_url.c_str());
      //curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.109:8081/api/remoteprintercallback/12345");

      // Perform the request, res will get the return code 
      curlparent->res = curl_easy_perform(curlparent->curl);
      // Check for errors
      if(curlparent->res != CURLE_OK)
      {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curlparent->res));
        sleep(5);
      }
    }

    // always cleanup 
    curl_easy_cleanup(curlparent->curl);
  }
  debug(("Leaving Curl Request Thread\n"));
  pthread_exit(&(curlparent->iret1));
  return NULL;  
}
//-----------------------------------------------------------------------------------------
}

// constructor 
TThreadLibCurl::TThreadLibCurl(TContainerList *clist, const char *requesturl)
{
  debug(("Initialize Thread LibCurl Object\n"));
 
  //---------------------------------------------------------------------------------------
  // test jsonparser
  //---------------------------------------------------------------------------------------
  //test_jsonparser();

  //---------------------------------------------------------------------------------------
  // test containerlist
  //---------------------------------------------------------------------------------------
  //test_containerlist();

  //---------------------------------------------------------------------------------------
  // initialize variables
  //---------------------------------------------------------------------------------------
  terminate = false;

  if(clist)
    containerlist = clist;

  if(requesturl != NULL)
    rest_url = std::string(requesturl);
  else
    rest_url = std::string("http://192.168.1.110:8081/api/remoteprintercallback/12345");

  debug(("Thread LibCurl REST Url: %s\n", rest_url.c_str()));

}
//-----------------------------------------------------------------------------------------
// destructor 
TThreadLibCurl::~TThreadLibCurl()
{
  debug(("Destroy Thread LibCurl Object\n"));

  TerminateThread();
}
//-----------------------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------------------

void TThreadLibCurl::test_jsonparser()
{
  #if 0
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
  #endif  
}
//-----------------------------------------------------------------------------------------

void TThreadLibCurl::test_containerlist()
{
  #if 0
  TContainerList *contlist = new TContainerList();

  contlist->AddElementID(1, "ABC");
  contlist->AddElementID(2, "DEF");

  contlist->AddElement("AUTODATA");

  contlist->DisplayVectorData();

  delete contlist;
  contlist = NULL;
  #endif
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------------------

int TThreadLibCurl::StartThread()
{
  debug(("LibCurl Thread: StartThread!\n"));

  //---------------------------------------------------------------------------------------
  // launch thread APIs (extra interfaces)
  //---------------------------------------------------------------------------------------
  /* Create independent threads each of which will execute function */
  iret1 = pthread_create(&thread1, NULL, NSCurl::ThreadCurlRequest, (void*)this);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  //pthread_join( thread1, NULL);

  debug(("LibCurl Thread returns: %d\n",iret1));

  return iret1;
}
//-----------------------------------------------------------------------------------------

void TThreadLibCurl::TerminateThread()
{
  terminate = true;
  curl_easy_cleanup(curl);
}
//-----------------------------------------------------------------------------------------