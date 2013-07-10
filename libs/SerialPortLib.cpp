//-----------------------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/signal.h>

#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <string.h> // strncpy strnstr strcmp
#include <unistd.h> // open, close, read
//#include <signal.h>

//#include <vector>
//#include <string>
//-----------------------------------------------------------------------------------------
#include "SerialPortLib.h"
//-----------------------------------------------------------------------------------------
#define BAUDRATE1 B4800
#define BAUDRATE2 B38400
#define BAUDRATE3 B57600
#define BAUDRATE4 B115200
#define BAUDRATE5 B230400
#define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
//-----------------------------------------------------------------------------------------
#define TCPIP_PORT  3090
#define MAXURLPATH  250
//-----------------------------------------------------------------------------------------
void PERROR(char* msg);
#define PERROR(msg) { perror(msg); abort(); }
//-----------------------------------------------------------------------------------------
// Enable/Disable printf debug (avoid sending data to USB Serial Port when not required)
#if 1
  #define debug(a) printf a
#else
  #define debug(a) (void)0
#endif
//-----------------------------------------------------------------------------------------

/***************************************************************************
* signal handler. sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/
bool rx_data_available = false; 

void signal_handler_IO(int status)
{
  debug(("Received Serial Port SIGIO signal.\n"));
  rx_data_available = true;
}
//---------------------------------------------------------------------------------------------  

//---------------------------------------------------------------------------------------------
// constructor 
TSerialPort::TSerialPort()
{
  debug(("Initialize Serial Port Object"));
  //---------------------------------------------------------------------------------------
  // initialize variables
  terminate = false;
  //rx_data_available = false;
  fd = 0;

  //---------------------------------------------------------------------------------------
  // save current port settings
  tcgetattr(fd,&oldtio); 
  //---------------------------------------------------------------------------------------
  // open serial port
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(fd<0) 
  {
    debug(("Error: open serial port\r\n"));
    perror(MODEMDEVICE);
    exit(-1); 
  }
  //---------------------------------------------------------------------------------------
  // install the signal handler before making the device asynchronous 
  saio.sa_handler = signal_handler_IO;
  //saio.sa_mask = 0;
  sigemptyset(&(saio.sa_mask));
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO, &saio, NULL);
  //---------------------------------------------------------------------------------------
  // allow the process to receive SIGIO
  fcntl(fd, F_SETOWN, getpid());
  /* Make the file descriptor asynchronous (the manual page says only 
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
  fcntl(fd, F_SETFL, FASYNC);
  //---------------------------------------------------------------------------------------
  // setting serial port configurations
  tcgetattr(fd,&oldtio); // save current port settings
  // set new port settings for canonical input processing
  newtio.c_cflag = BAUDRATE4 | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;   // set input mode (non-canonical, no echo,...)
  newtio.c_cc[VMIN] = 1;     // inter-character timer unused
  newtio.c_cc[VTIME] = 0;    // blocking read until x chars received
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
  //---------------------------------------------------------------------------------------
  //write(fd, "Starting GPS Parser!", 20);
  //---------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------------------------
// destructor 
TSerialPort::~TSerialPort()
{
  debug(("Destroy Serial Port Object"));

  // close serial port
  close(fd);

  // restore serial port old configurations
  tcsetattr(fd,TCSANOW,&oldtio);
}
//---------------------------------------------------------------------------------------------
// Private functions
//---------------------------------------------------------------------------------------------
    
char TSerialPort::ReadDataChar()
{  
  char temp_buf[2];
  int res = 0;

  // read data from serial port - will wait for data
  res = read(fd, temp_buf, 1);   // returns after 1 chars have been input
  temp_buf[1] = 0;

  debug(("%c",temp_buf[0]));

  // check for errors - abort when receiving code 0x00 
  //if(temp_buf[0] == 'z') 
  //  return '\0';

  return temp_buf[0];
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------------------------
int *TSerialPort::GetFD()
{
  return &fd;
}
//---------------------------------------------------------------------------------------------

bool TSerialPort::ReadDataLine()
{  
  char cdata;

  if(!rx_data_available)
  {
    //debug(("ReadDataLine: No Rx data available!\n"));
    return false;
  }

  while(!terminate)
  {
    debug(("-»\n"));
    cdata = ReadDataChar();
    debug(("«-\n"));

    cmddata.push_back(cdata);

    if(cmddata.length() > MAXBUFSIZE)
    {
      debug(("ReadDataLine: Buffer reached MAXBUFSIZE!\n"));
      rx_data_available = false;
      return false;
    }
 
    if(cdata == '\0' || cdata == '\r' || cdata == '\n')
    {
      debug(("ReadDataLine: Found ENDLINE char\n"));
      rx_data_available = false;
      return true;
    }
  }

  debug(("ReadDataLine: Terminate!\n"));
  rx_data_available = false;
  return false;
}
//---------------------------------------------------------------------------------------------

std::string TSerialPort::GetDataLine()
{
  return cmddata;
}
//--------------------------------------------------------------------------------------------

void TSerialPort::ClearDataLine()
{
  cmddata.clear();
}
//---------------------------------------------------------------------------------------------

void TSerialPort::WriteDataLine(std::string cmd)
{
  //tcflush(fd, TCIOFLUSH); // clear buffer
  write(fd, cmd.c_str(), cmd.length());  

/*
    std::string cmd1 = "G28\n";
    printf("Send Printer cmd: %s\r\n", cmd1.c_str());
    write(fd, cmd1.c_str(), cmd1.length());

  tcflush(fd, TCIOFLUSH); // clear buffer

    cmd1 = "G1 X10 Y20 F8000\n";
    printf("Send Printer cmd: %s\r\n", cmd1.c_str());
    write(fd, cmd1.c_str(), cmd1.length());    

  tcflush(fd, TCIOFLUSH); // clear buffer
  */
}
//---------------------------------------------------------------------------------------------

void TSerialPort::TestPrinterGCODE()
{
  /*
  // serial port main loop
  char trigger[]="echo:SD init fail";
  char *ptrigger = &trigger[0];
  bool printerinit = false;
  int printerackcounter = 0;
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

printf("%c",temp_buf[0]);
//printf("[%x]",temp_buf[0]);
//  if(temp_buf[0] != 0x0a && temp_buf[0] != 0)
//    printf("%c", temp_buf[0]);

  if(printerinit)
    printerackcounter++; // count 3 response chars : OK\n


  if(temp_buf[0] == *ptrigger)
    ptrigger++; // increment pointer
  else 
    ptrigger = &trigger[0]; // reset pointer

  if(*ptrigger == '\0')
  {
    printerinit = true;
    ptrigger = &trigger[0];
    printf("\r\nFound Printer Init. END\r\n");

    std::string cmd = "G28\n";
    printf("Send Printer cmd: %s\r\n", cmd.c_str());
    write(fd, cmd.c_str(), cmd.length());
  }

  if(printerackcounter == 3*1) // 2nd cmd
  {
    std::string cmd = "G1 X0 Y0 Z10 F6000\n";
    printf("Send Printer cmd: %s\r\n", cmd.c_str());
    write(fd, cmd.c_str(), cmd.length());
  }
  else if(printerackcounter == 3*2) // 3rd cmd
  {
    std::string cmd = "G1 X10 Y20 F8000\n";
    printf("Send Printer cmd: %s\r\n", cmd.c_str());
    write(fd, cmd.c_str(), cmd.length());
  }  
  else if(printerackcounter == 3*3) // 4th cmd
  {
    std::string cmd = "G91\n";
    printf("Send Printer cmd: %s\r\n", cmd.c_str());
    write(fd, cmd.c_str(), cmd.length());
  }   
  else if(printerackcounter == 3*4) // 5th cmd
  {
    iret1 = pthread_create( &thread1, NULL, ThreadCurlRequest, (void*) NULL);
    
    pthread_join( thread1, NULL);

    printf("Thread 1 returns: %d\n",iret1);
  }

  }// end while - main loop
  */
}
//---------------------------------------------------------------------------------------------

void TSerialPort::FlushBuffer()
{
  tcflush(fd, TCIFLUSH);
}
//---------------------------------------------------------------------------------------------
