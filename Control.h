/*
* File : P1_rx.h
*/
#ifndef __P1_RX_H_
#define __P1_RX_H_
#include "dcomm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500

/* Define receive buffer size */
#define RXQSIZE 8


/* Define minimum upperlimit */
#define MIN_UPPERLIMIT 4


/* Define maximum lowerlimit */
#define MAX_LOWERLIMIT 1


Byte rxbuf[RXQSIZE];
QTYPE rcvq = { 0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;


Byte sent_xonxoff = XON;
Boolean send_xon = false,
	send_xoff = false;
	
	
/* Socket */
int sockfd; // listen on sock_fd

void* consumerthread(void *threadArgs);

/* Functions declaration */
static Byte *rcvchar(int sockfd, QTYPE *queue);
static Byte *q_get(QTYPE *, Byte *);

#endif
