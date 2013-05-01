#include <aiousb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <assert.h>

#include <iostream>
#include <zmq.hpp>
#include <sstream>

#define MAX_ODOURS_PER_LINE 5
#define MAX_ODOUR_PORTS 8
#define BITS_PER_PORT 8
#define MAX_ODOURS (BITS_PER_PORT * MAX_ODOUR_PORTS)
#define BLANK_NOT_SET -1

using namespace AIOUSB;

const int DEVICES_REQUIRED = 1;				// change this to 1 if only one device
const int BITS_PER_BYTE = 8;
const int MAX_DIO_BYTES = 4;				// a modest little assumption for convenience
const int MASK_BYTES = ( MAX_DIO_BYTES + BITS_PER_BYTE - 1 ) / BITS_PER_BYTE;
const int MAX_NAME_SIZE = 20;

// stuff for API calls
unsigned char mask[2];
unsigned char data[12];
int triState;
unsigned char byte;
unsigned char byteIdx;

// global variables
int myDevIdx;
FILE* fi;
FILE* fo;

// Functions
int odorPulses(char* configFileName);
int validateIndex(int devIdx);
int init();
void dataRest(int blankOdor);
int pulseSingleOdor(int odorNum, int stimTime, int delayTime, int blankOdor);

// Device info struct
static struct DeviceInfo {
  unsigned char outputMask[ MASK_BYTES ];
  unsigned char readBuffer[ MAX_DIO_BYTES ];		// image of data read from board
  unsigned char writeBuffer[ MAX_DIO_BYTES ];		// image of data written to board
  char name[ MAX_NAME_SIZE + 2 ];
  unsigned long productID;
  unsigned long nameSize;
  unsigned long numDIOBytes;
  unsigned long numCounters;
  __uint64_t serialNumber;
  int index;
} deviceTable[ DEVICES_REQUIRED ];

int init() {
  unsigned char mask[2];
  unsigned char data[12];
  int triState;
  int ret;
  
  mask[0] = 0xFF;
  mask[1] = 0x3;
  triState = 0;
	
  data[0]=0x01;	//sets the data lines. Defaults to position 0 on data 0 "open" to allow gas flow through the first blank
  data[1]=0x00;
  data[2]=0x00;
  data[3]=0x00;
  data[4]=0x00;
  data[5]=0x00;
  data[6]=0x00;
  data[7]=0x00;
  data[8]=0x00;
  data[9]=0x00;
  data[10]=0x00;
  data[11]=0x00;
	
  ret = DIO_Configure(myDevIdx, triState, mask, data);	
  if (ret > AIOUSB_SUCCESS) {
    printf("DIO_Configure Failed dev=0x%0x err=%d\015Have you run AccesLoader?\n",(unsigned int)myDevIdx,ret);
    return(0);
  } else {
    printf("Successfully initialized card!\n");
  }
  return(0);	
}

void dataReset(int blankOdor) {
  int ret;
  
  data[0]=0;
  data[1]=0;
  data[2]=0;
  data[3]=0;
  data[4]=0;
  data[5]=0;
  data[6]=0;
  data[7]=0;
  
  data[9]=0x00;
  
  if (blankOdor >= 0 && blankOdor < MAX_ODOURS ) {
    int port = blankOdor/BITS_PER_PORT;
    data[port]=pow(2, blankOdor % BITS_PER_PORT);
  }
  
  ret = DIO_WriteAll(myDevIdx, data);
  return;
}

int pulseSingleOdor(int odorNum, int stimTime, int delayTime, int blankOdor) {
  int port = odorNum/BITS_PER_PORT;

  if (odorNum >= 0 && odorNum < MAX_ODOURS) {
    dataReset(blankOdor);// set blank odor
  }

  data[9] = 1; // confirm received trigger
  data[port] = pow(2, odorNum % BITS_PER_PORT);
  sleep(delayTime);
  DIO_WriteAll(myDevIdx, data);
  sleep(stimTime);
  dataReset(blankOdor);
  printf("Applied odor %d for %d s, after %d s delay\n", odorNum, stimTime, delayTime);

}

int main(int argc, char** argv) {
  // Device initialization
  int MAX_NAME_SIZE = 20;
  char name[ MAX_NAME_SIZE + 2 ];
  unsigned long productID, nameSize = MAX_NAME_SIZE;             
  unsigned long result = AIOUSB_Init();

  struct DeviceInfo* device;
  if (result == AIOUSB_SUCCESS) {
    device = &deviceTable[0];
    device->nameSize = MAX_NAME_SIZE;
    result = QueryDeviceInfo(diFirst, &device->productID, &device->nameSize, device->name, &device->numDIOBytes, &device->numCounters );
    device->index = 0;
    myDevIdx = device->index;
    if (result == AIOUSB_SUCCESS) {      
      name[ nameSize ] = 0;
      printf("Found a device with product ID %#06x and name '%s'\n", ( unsigned ) device->productID, device->name );
    } else {
      printf("Error '%s' querying device at index %d. Check if firmware is loaded.\n", AIOUSB_GetResultCodeAsString( result ), device->index);
      AIOUSB_Exit();
    }
  }
  
  // main program
  AIOUSB_SetCommTimeout( device->index, 1000 );
  int tmp, ret;
  tmp = init();
  int blank = -1;

  // init zmq and listen to port 5556 over TCP
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.connect("tcp://localhost:5556");

  const char* filter ="";
  subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

  // main loop:
  // Block until something is receieved on port 5556
  // The data will come in the form:
  // odor stim delay blank 
  // which is then parsed into the appropriate variables, and used to control 
  // the ODD. (Pretty simple.)
  int odor, stim, delay, blank;
  while (1) {
    zmq::message_t update;
    subscriber.recv(&update);
    std::cout << "Received odor request at " << time(NULL) << std::endl;
    std::istringstream iss(static_cast<char*>(update.data()));
    std::cout << static_cast<char*>(update.data()) << std::endl;
    iss >> odor >> stim >> delay >> blank;
    pulseSingleOdor(odor, stim, delay, blank);   
  }

  return 0;
}

