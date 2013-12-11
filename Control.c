//Header file
#include "Control.h"
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//for socket client
struct sockaddr_storage clntAddr;
socklen_t clntAddrLen = sizeof(clntAddr);

// global variable
int co = 0;		//count total byte which want to be read
int con = 0;	//count total byte which want to be consumed
Byte current_byte;			//value of byte read

int main(int argc, char *argv[])
{	
	pthread_t consumert;	//thread for buffer container
	
	//check if argument valid
	if (argc != 2) {
		printf("Parameter(s)" "<Server Port/Service>\n");
		return 0;
	}
	
	//save the value of port
	char *service = argv[1];
	
	// Construct the server address structure
	struct addrinfo addrCriteria;						// Criteria for address
	memset(&addrCriteria, 0, sizeof(addrCriteria));		// Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;					// Any address family
	addrCriteria.ai_flags = AI_PASSIVE;					// Accept on any address/port
	addrCriteria.ai_socktype = SOCK_DGRAM;				// Only datagram socket
	addrCriteria.ai_protocol = IPPROTO_UDP;				// Only UDP socket
	
	
	struct addrinfo *servAddr; // List of server addresses
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0) {
		printf("getaddrinfo() failed");
		return 0;
	}
	
	// create socket
	sockfd = socket(servAddr->ai_family, servAddr->ai_socktype,
	servAddr->ai_protocol);
	if (sockfd < 0) {
		printf("socket() failed");
		return 0;
	}
	
	// Bind to local address
	if (bind(sockfd, servAddr->ai_addr, servAddr->ai_addrlen) < 0) {
		printf("bind() failed");
		return 0;
	}

	// Free address list allocated by getaddrinfo()
	freeaddrinfo(servAddr);
	
	/* Create new thread */
	pthread_create(&consumert,NULL,consumerthread,NULL);
	
	/*** Main Thread Code ***/
	while(true) {
		//read byte from socket
		current_byte = *(rcvchar(sockfd, rxq));
	}
	
	//close socket
	close(sockfd);
	return 0;
}

/*** Consumer Thread Code ***/
void* consumerthread(void *threadArgs)
{
	while (true) {
		//consume thread
		Byte * test = q_get(rxq,&current_byte);
		
		//if count > 0
		if (test != NULL) {
			//front not in 0
			if (rxq->front > 0) {
				if (rxq->data[rxq->front-1] != Endfile && rxq->data[rxq->front-1] != CR && rxq->data[rxq->front-1] != LF) {
					printf("Consuming byte %i: '%c'\n",++con,rxq->data[rxq->front-1]);
				} else if (rxq->data[rxq->front-1] == Endfile) {
					//if endfile
					printf("End of File accepted.\n");
					exit(0);
				}
			} else {
				if (rxq->data[7] != Endfile && rxq->data[7] != CR && rxq->data[7] != LF) {
					printf("Consuming byte %i: '%c'\n",++con,rxq->data[7]);
				} else if (rxq->data[7] == Endfile) {
					//if endfile
					printf("End of File accepted.\n");
					exit(0);
				}
			}
		}
		//delay
		usleep(DELAY*1000);
	}
	return NULL;
}

Byte tes[2];
static Byte *rcvchar(int sock, QTYPE *queue)
{
	Byte *buffer;
	if (!send_xoff) {
		//read from socket & push it to queue
		ssize_t numBytesRcvd = recvfrom(sockfd, tes, sizeof(tes), 0,
			(struct sockaddr *) &clntAddr, &clntAddrLen);
			
		if (numBytesRcvd < 0) {
			//if error
			printf("recvfrom() failed\n");
		} else {
			//fill the circular
			queue->data[queue->rear] = tes[0];
			queue->count++;
			if (queue->rear < 7) {
				queue->rear++;
			} else {
				queue->rear = 0;
			}
			co++;
		}
		
		if (tes[0] != Endfile && tes[0] != CR && tes[0] != LF) {
			printf("Received byte %i\n",co);
		}
		
		//if buffer size excess minimum upperlimit
		if (queue->count > MIN_UPPERLIMIT && sent_xonxoff == XON) {
			sent_xonxoff = XOFF;
			send_xoff = true;
			send_xon = false;
			printf("Buffer > minimum upperlimit. Send XOFF.\n");
			char test[2];
			test[0] = XOFF;
			//send XOFF to transmitter
			ssize_t numBytesSent = sendto(sockfd, test, sizeof(test), 4,
				(struct sockaddr *) &clntAddr, sizeof(clntAddr));
			if (numBytesSent < 0)
				printf("sendto() failed)");
		}
		
		return &tes[0];
		
	} else {
		*buffer = 0;
		return buffer;
	}
}


/* q_get returns a pointer to the buffer where data is read or NULL if
* buffer is empty.
*/
static Byte *q_get(QTYPE *queue, Byte *data)
{
	//if count == 0
	if (!queue->count) {
		return (NULL);
	} else {
		do {
			//obtain buffer
			if (queue->count > 0) {
				(*data) = queue->data[queue->front];
				queue->count--;
				if (queue->front < 7) {
					queue->front++;
				} else {
					queue->front = 0;
				}
			}
			//check if data valid
		} while ((*data < 32) && (*data != LF) && (queue->count > 0));
		
		//if count reach the maksimum lowerlimit
		if (queue->count < MAX_LOWERLIMIT && sent_xonxoff == XOFF) {
			sent_xonxoff = XON;
			send_xon = true;
			send_xoff = false;
			printf("Buffer < maksimum lowerlimit. Send XON\n");
			char test[2];
			test[0] = XON;
			//send XON to transmitter
			ssize_t numBytesSent = sendto(sockfd, test, sizeof(test), 4,
			(struct sockaddr *) &clntAddr, sizeof(clntAddr));
			if (numBytesSent < 0)
				printf("sendto() failed\n");
		}
		
		return data;
	}
}
