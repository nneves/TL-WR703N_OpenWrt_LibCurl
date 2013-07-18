#ifndef __THREADLIBCURLH
#define __THREADLIBCURLH
//---------------------------------------------------------------------------------------------
// LibCurl Thread
//---------------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
// Date:      17-July-2013
// Objective: LibCurl Thread class
//
//---------------------------------------------------------------------------------------------
#include <curl/curl.h>
//---------------------------------------------------------------------------------------------

//class CURL;
//class CURLcode;
class TContainerList;
//---------------------------------------------------------------------------------------------

class TThreadLibCurl
{  
  //-------------------------------------------------------------------------------------------
  private:
  //-------------------------------------------------------------------------------------------
  pthread_t thread1;


  //size_t curl_write(void *ptr, size_t size, size_t nmemb, void *stream);
  //static void* ThreadCurlRequest(void *arg);

  void test_jsonparser();
  void test_containerlist();

  //-------------------------------------------------------------------------------------------
  public:  
  //-------------------------------------------------------------------------------------------
  int  iret1;  
  bool terminate;

  CURL *curl;
  CURLcode res;

  TContainerList *containerlist;
  std::string rest_url;

  TThreadLibCurl(TContainerList *containerlist=NULL, const char *requesturl=NULL);
  ~TThreadLibCurl();

  int StartThread();
  void TerminateThread();
};
//---------------------------------------------------------------------------------------------
#endif