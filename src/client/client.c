/*
Copyright (C) 2016  Davidson Francis <davidsondfgl@gmail.com>
              2016  Ádamo Ludwig     <Ludwigadamo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <mts.h>

/* Modes. */
struct
{
	int set;
	int iterative;
	char address[30];
} args;

/**
 * Gets the time from the server.
 * @param sock Connection socket.
 */
void getTime(int sock)
{
	int n;               /* Return value.   */
	struct message msg;  /* Struct message. */
	char rtime[30];      /* Time.           */
  	struct tm *info;     /* Local time.     */
	
	/* Request time. */
	memset((void*)&msg, 0, sizeof(msg));
	msg.type = MSG_TIMEREQUEST;
	msg.data.a = time(NULL);
	n = send(sock,&msg,sizeof(msg),0);
	if (n < 0)
		printf("MTS: Error writing to socket");

	/* Receive the time. */
	n = recv(sock,&msg,sizeof(msg), 0);
	if (n < 0)
		printf("MTS: Error when receiving the time!\n");
		
	msg.data.b = time(NULL);
	
	/* Adjust time with Round Trip Time. */
	time_t systime = msg.data.x + RTT(msg);

	/* Converts the time. */
	info = localtime(&systime);
  	strftime(rtime, 30, "[%d/%m/%y - %H:%M:%S] ",info);
	
	/* Shows the server time. */
	printf("The time is: %" PRIu64 " or %s, with %d delay\n",msg.data.x, rtime, (int)(RTT(msg)));

	/* Sets the time system. */
	if (args.set)
	{
		if (stime(&systime) < 0)
			printf("MTS: An errror has occurred when changing the system time\n");	
	}
}

/**
 * Starts the client.
 */
void startClient()
{
	int sock; /* Current socket.       */
	int n;    /* Amount of read bytes. */
    struct sockaddr_in serv_addr;

	/* Create socket. */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        printf("MTS: Error when opening socket\n");

	memset((void*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(args.address);
    serv_addr.sin_port = htons(PORT);

	/* Connects. */
    if (connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        printf("MTS: Error when connecting!\n");

	/* Send 'HELLO' message. */
	struct message msg;
	msg.type = MSG_HELLO;
    n = send(sock,&msg,sizeof(msg),0);
    if (n < 0)
    	printf("MTS: Error writing to socket");

	/* Request the time according with the modes. */
	if (args.iterative)
	{
		printf("MTS: Iterative mode..\n");
		
		while (1)
		{
			char c;
			c = getchar();
		    fflush(stdin);
		    if (c == '\n')
		    	getTime(sock);
		}
	}
	else
		getTime(sock);
		
    close(sock);
}

/**
 * Prints the usage.
 * @param name Binary name.
 */
void usage(char *name)
{
	printf("Usage: %s <ip-address> [options]\n",name);
	printf("Brief: Performs client to get the current time on %d port.\n",PORT);
	printf("Options:\n");
	printf("  -i Iterative mode\n");
	printf("  -s Set mode, changes the system time (needs root)\n");
	printf("  -h Show this help\n");
}

/**
 * Gets the arguments.
 * @param argc Parameters amount.
 * @param argv Parameters.
 */
void getargs(int argc, char **argv)
{
	if (argc >= 2)
	{
		if (argc > 2)
		{
			for (int i = 2; i < argc; i++)
			{
				if (!strcmp(argv[i], "-i"))
					args.iterative = 1;
				
				else if (!strcmp(argv[i], "-s"))
					args.set = 1;
				
				else if (!strcmp(argv[i], "-h"))
				{
					usage(argv[0]);
					exit(0);
				}
			}
		
			if (args.set)
			{
				if (geteuid() != 0)
				{
					printf("The mode \"-s\" (Set Time), needs to be run as root!\n");
					usage(argv[0]);
					exit(-1);
				}
			}
		}
		strncpy(args.address, argv[1], strlen(argv[1]));
	}
	else
	{
		usage(argv[0]);
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	memset((void*)&args, 0, sizeof(args));
	getargs(argc, argv);
	startClient();
    return 0;
}
