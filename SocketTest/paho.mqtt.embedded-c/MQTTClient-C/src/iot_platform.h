/*******************************************************************************
 * Vecmocon Technologies.
 * @author: Altamash Abdul Rahim
 *******************************************************************************/

#ifndef __IOT_PLATFORM_
#define __IOT_PLATFORM_


#define MAX_SIZE 50

typedef struct Timer Timer;

struct Timer {
	unsigned long systick_period;
	unsigned long end_time;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

void TimerInit(Timer*);

int read(Network*, unsigned char*, int, int);
int write(Network*, unsigned char*, int, int);
void disconnect(Network*);
void NewNetwork(Network*);
int NetworkConnect(Network* n, char* endpoint, int port);

#endif
