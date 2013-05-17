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
#include "JsonParserLib.h"
 //-----------------------------------------------------------------------------------------

#define BAUDRATE1 B4800
#define BAUDRATE2 B38400
#define BAUDRATE3 B57600
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
//-----------------------------------------------------------------------------------------
#define FALSE 0
#define TRUE 1
//-----------------------------------------------------------------------------------------
#define NMEA_NA    0
#define NMEA_GPGGA 1
#define NMEA_GPRMC 2
//-----------------------------------------------------------------------------------------
#define TCPIP_PORT  3090
#define MAXURLPATH  250
//-----------------------------------------------------------------------------------------
void PERROR(char* msg);
#define PERROR(msg) { perror(msg); abort(); }
#define MAXBUF  4096
char buffer[MAXBUF];
//-----------------------------------------------------------------------------------------
static volatile int exec_end = FALSE;
//-----------------------------------------------------------------------------------------
short int debugmode;
//-----------------------------------------------------------------------------------------
CURL *curl;
CURLcode res;
//-----------------------------------------------------------------------------------------

void cleanup(int sig)
{
  if(debugmode)
    printf("recieved signal %d\n", sig);
  exec_end = TRUE;
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

size_t curl_write( void *ptr, size_t size, size_t nmemb, void *stream)
{
  //printf("Received data: %d\r\n", size*nmemb);
  //printf("--> %s\r\n", ((char *)ptr));

  /*
  char *pdata = (char *)ptr;

  if( size*nmemb > 13 &&
    *(pdata+0) == '{' && 
    *(pdata+1) == '\"' &&
    *(pdata+2) == 'p' &&
    *(pdata+3) == 'r' &&
    *(pdata+4) == 'i' &&
    *(pdata+5) == 'n' &&
    *(pdata+6) == 't' &&
    *(pdata+7) == 'e' &&
    *(pdata+8) == 'r' &&
    *(pdata+9) == 'c' &&
    *(pdata+10) == 'm' &&
    *(pdata+11) == 'd' &&
    *(pdata+12) == '\"' &&
    *(pdata+13) == ':')
  {
    printf("--> %s\r\n", ((char *)ptr));
  }
  else
  {
    printf(".");
  }
  */
  
  char *pdata = (char *)ptr;
  std::string strdata;
  strdata = std::string(pdata);

  std::size_t found = strdata.find("\"printercmd\"");
  if(found != std::string::npos)
  {
    printf("--> %s", strdata.c_str());

    TJsonParser *pJsonParser = new TJsonParser(strdata.c_str());
    std::string result;
    result = pJsonParser->Prop("printercmd")->Value();

    printf("-->--> %s\n", result.c_str());

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
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    /* get data */
    while(exec_end==FALSE) 
    {
      curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.110:8081/api/remoteprintercallback/12345");

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  printf("Leaving Thread\n");
  return NULL;  
}
//---------------------------------------------------------------------------------------

int main(void)
{
  //---------------------------------------------------------------------------------------
  // variables for threads
  //---------------------------------------------------------------------------------------
  pthread_t thread1, thread2;
  int  iret1, iret2;
  //---------------------------------------------------------------------------------------
  // variables for serial port communication
  //---------------------------------------------------------------------------------------
  int fd = 0;
  int res = 0;
  struct termios oldtio,newtio;
  //---------------------------------------------------------------------------------------
  // aux variables for serial data parsing and grouping
  //---------------------------------------------------------------------------------------
  char temp_buf[2];
  //char str[1024];

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
  iret1 = pthread_create( &thread1, NULL, ThreadCurlRequest, (void*) NULL);

  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  //pthread_join( thread1, NULL);

  printf("Thread 1 returns: %d\n",iret1);
  //---------------------------------------------------------------------------------------
  
  //---------------------------------------------------------------------------------------
  // save current port settings
  tcgetattr(fd,&oldtio); 
  //---------------------------------------------------------------------------------------
  // open serial port
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
  if(fd<0) 
  {
    printf("Error: open serial port\r\n");
    perror(MODEMDEVICE);
    exit(-1); 
  }
  //---------------------------------------------------------------------------------------
  // setting serial port configurations
  //bzero(&newtio, sizeof(newtio));
  memset(&newtio, 0, sizeof(newtio));
  newtio.c_cflag = BAUDRATE2 | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;   
  newtio.c_lflag = 0;          // set input mode (non-canonical, no echo,...)
  newtio.c_cc[VTIME]    = 0;   // inter-character timer unused
  newtio.c_cc[VMIN]     = 1;   // blocking read until x chars received
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio); 
  //---------------------------------------------------------------------------------------
  //write(fd, "Starting GPS Parser!", 20);
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  // serial port main loop
  while(exec_end==FALSE) 
  { 
    // loop for input
    res = read(fd,temp_buf,1);   // returns after 1 chars have been input (newtio.c_cc[VMIN]=1)
    temp_buf[1]=0;

    // check for errors - abort when receiving code 0x00 
    if(temp_buf[0]=='z') 
    {
      exec_end=TRUE;
      break;
    }

//printf("[%c]",temp_buf[0]);
//printf("[%x]",temp_buf[0]);



    // verify if gps command is completed ($......\n\n or \r\n)
    // (this point flag_start_cmd is always TRUE)
    // (just need to test if flag_end_cmd is FALSE)
/*
    if(flag_end_cmd == FALSE)
    {
      ptr_endcmd1=strstr(buffer,"\r");
      ptr_endcmd2=strstr(buffer,"\n");      

      if(ptr_endcmd1!=NULL || ptr_endcmd2!=NULL)
      {
        printf("found_complete_command\r\n");

        flag_end_cmd = TRUE;
      }
    }  
*/    

  }// end while - main loop
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // close serial port
  close(fd);
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // restore serial port old configurations
  tcsetattr(fd,TCSANOW,&oldtio);
  //---------------------------------------------------------------------------------------

  return 0;
}