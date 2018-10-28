#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "MQTTClient.h"

#define ENDPOINT "iot.eclipse.org"
#define PORT     1883

Network N;
MQTTClient client;

unsigned char send_buf[512];
unsigned char read_buf[512];

int reconnect(void);
int publish_msg(char* buf, int len, char* Topic);

void delay(int number_of_seconds)
{
	// Converting time into milli_seconds 
	int milli_seconds = 1000 * number_of_seconds;
	// Stroing start time 
	clock_t start_time = clock();
	// looping till required time is not acheived 
	while (clock() < start_time + milli_seconds);
}

void messageArrived(MessageData* md)
{
	MQTTMessage* m = md->message;
	/*
	 * Data sent in format "immobilised,locked" e.g: "1,0"
	 */
	printf("Data: %s\nReceived from Topic: %s\n", (char*)m->payload, (char*)md->topicName);
}

void main()
{
	srand(time(0));
	NewNetwork(&N);
	MQTTClientInit(&client, &N, 20000, (unsigned char*)&send_buf, sizeof(send_buf), (unsigned char*)&read_buf, sizeof(read_buf));
	if (NetworkConnect(&N, ENDPOINT, PORT) == 0)
		puts("Client Connected");
	int nowTick = 0,prevTick = 0;
	Timer my_timer;
	TimerInit(&my_timer);
	TimerCountdownMS(&my_timer, 2000);
	while (1)
	{
		/*if (TimerIsExpired(&my_timer))
		{
			printf("Publishing\n");
			TimerCountdownMS(&my_timer, 2000);
		}*/
		if (!client.isconnected)
			reconnect();
		printf("Yield: %d\n", MQTTYield(&client, 10));
		/*
		nowTick = GetTickCount();
		if (nowTick - prevTick > 2500)
		{
			prevTick = nowTick;
			printf("Publishing\n");
			//publish_msg("Hello", strlen("Hello"), "Altamash");
		}*/
	}
}

int reconnect(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 1;
	data.willFlag = 1;
	data.MQTTVersion = 4; //3 for 3.1 and 4 for 3.1.1
	int rand_num = rand();
	char arr[20];
	sprintf(arr,"%s-%d", DEVICE_ID, rand_num);
	data.clientID.cstring = arr;
	data.keepAliveInterval = KEEP_ALIVE;
	data.cleansession = 1; //DONE FOR AWS IOT (message broker does not support persistent sessions)
	char last_will[300];
	sprintf(last_will, "{\"deviceId\":\"%s-%d\",\"Tick\": %ld}", DEVICE_ID, rand_num, GetTickCount());
	data.will.message.cstring = (char*)last_will;
	data.will.qos = 0;
	data.will.retained = 0;
	data.will.topicName.cstring = WILL_TOPIC;
	rc = MQTTConnect(&client, &data);
	if (rc == SUCCESS)
	{
		puts("MQTT Connected");
		delay(2);
		rc = MQTTSubscribe(&client, SUB_TOPIC, QOS0, messageArrived);
	}
	else
	{
		puts("Mqtt Failed");
		return -1;
	}
	return rc;
}

int publish_msg(char* buf, int len, char* Topic)
{
	static MQTTMessage pubmsg;
	pubmsg.payload = buf;
	pubmsg.payloadlen = len;
	pubmsg.qos = QOS0;
	pubmsg.retained = 0;
	pubmsg.dup = 0;
	return MQTTPublish(&client, Topic, &pubmsg);
}