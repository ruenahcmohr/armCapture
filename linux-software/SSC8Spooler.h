
#ifndef __SSC8Spooler
#define __SSC8Spooler

#include "servoArray.h"
#include "ssls3.h"
#include "returnValues.h"
#include <unistd.h>
#include <malloc.h>
#include "genServo.h"
#include <pthread.h>
#include "Stuff.h"

#define SERVOCOUNT  8

typedef struct SSC8Spooler_s {
  SSLS_t         serial;
  servoArray_t   servos;
  pthread_t      xmitter;
} SSC8Spooler_t;


Status_t SSC8SpoolerStartTransmitter(SSC8Spooler_t *this);
Status_t SSC8SpoolerInit(SSC8Spooler_t *this);
Status_t SSC8SpoolerFini(SSC8Spooler_t *this);
Status_t SSC8SpoolerOpenDevice(SSC8Spooler_t *this, char * filename);
void     *SSC8SpoolerTransmittLoop(void *this);

#endif

