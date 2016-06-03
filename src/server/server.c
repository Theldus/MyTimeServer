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
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <mts.h>

/* Definitions. */
void processClient(int,struct sockaddr_in);
void startServer();
char* verbose();
void usage(char*);
void getargs(int, char**);

/* Modes. */
struct
{
	int verbose;
	int log;
} args = {0, 0};

/* Standard output. */
FILE *fout;

/**
 * Threats individually each client connected in
 * the server.
 * @param sock Current sock.
 * @param client Current client connection.
 */
void processClient(int sock, struct sockaddr_in client)
{
	int n;              /* Number of bytes received. */
	struct message msg; /* Message.                  */
	unsigned hello;     /* Client confirmation.      */
	char *clientIP;     /* Client IP Address.        */
	pid_t pid;          /* Client PID identifcation. */

	pid = getpid();
	hello = 0;
	clientIP = inet_ntoa(client.sin_addr);

	/* Receives message until get some error. */
	while( (n = recv(sock, (void*)&msg, sizeof(msg), 0)) > 0 )
	{
		switch(msg.type)
		{
			/* Sends the server time to client. */
			case MSG_TIMEREQUEST:
				if(!hello)
					goto invalidClient;

				/* Gets actual time. */
				msg.data.x = time(NULL);

				/* Wait a while. */
				srand(time(NULL));
				usleep( (rand() % 100) * 1000 );

				/* Gets actual time again. */
				msg.data.y = time(NULL);

				/* Send the message. */
				msg.type = MSG_TIMERESPONSE;
				send(sock, &msg, sizeof(msg), 0);

				fprintf(fout,"%sMTS: Client (#%d - %s) time requirement: %" PRIu64 "\n",
					verbose(), pid, clientIP, msg.data.x);
				fflush(fout);
				break;

			/* Client identification. */
			case MSG_HELLO:
				if(!hello)
				{
					fprintf(fout,"%sMTS: Client (#%d) connected!: %s\n", 
						verbose(), pid, clientIP);
					fflush(fout);
					hello = MSG_HELLO;
				}
				break;

			/* Unknown message, invalid client. */
			default:
				goto invalidClient;
				break;
		}
		memset((void*)&msg, 0, sizeof(msg));
	}

	fprintf(fout,"%sMTS: Client (#%d) disconnected!: %s\n", verbose(), pid, clientIP);
	fflush(fout);
	return;

invalidClient:
	fprintf(fout,"%sMTS: Invalid client (#%d) disconnected!: %s\n", verbose(), 
		pid, clientIP);
	fflush(fout);
}

/**
 * Starts the server.
 */
void startServer()
{
	int sock;                  /* Current socket.        */
	int new_sock;              /* New opened connection. */
	struct sockaddr_in server; /* Server.                */
	struct sockaddr_in client; /* Client.                */
	int len;                   /* Length of sockaddr.    */
	pid_t child;               /* Client PID.            */

	/* Create socket. */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	    perror("MTS: Could not create socket");

	/* Prepare the sockaddr_in structure. */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	/* Bind. */
	if( bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0 )
		perror("MTS: Bind failed");

	/* Listen. */
	listen(sock, MAX_CLIENTS);

	/* Wait for incoming connections. */
	fprintf(fout,"%sMTS: Waiting for incoming connections...\n",verbose());
	fflush(fout);

	len = sizeof(struct sockaddr_in);

	/* Ignore death of childs. */
	signal(SIGCHLD,SIG_IGN);

	/* Accept connections. */
	while (1)
	{
		/* Accept. */
		new_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&len);
		if (new_sock < 0)
		{
			perror("MTS: Error on accepting conections..");
			exit(-1);
		}

		/* Creates an child. */
		child = fork();
		if (child < 0)
		{
			perror("MTS: Occur error when forking child..");
			exit(-1);
		}

		/* Process client messages. */
		else if(child == 0)
		{
			close(sock);
			processClient(new_sock, client);
			exit(0);
		}

		/* Closes the connection to create another. */
		else
			close(new_sock);
	}
}

/**
 * Verifies if verbose mode is enabled.
 * @return Current time if enabled, or empty string
 * otherwise.
 */
char* verbose()
{
	char *rtime;
	time_t current_time;
  	struct tm *info;

	if (args.verbose)
	{
  		rtime = malloc(sizeof(char)*30);
  		time(&current_time);
  		info = localtime (&current_time);
  		strftime(rtime, 30, "[%d/%m/%y - %H:%M:%S] ",info);
  		return rtime;
	}
	else
		return "";
}

/**
 * Prints the usage.
 * @param name Binary name.
 */
void usage(char *name)
{
	fprintf(fout,"Usage: %s [options]\n",name);
	fprintf(fout,"Brief: Performs server to listening on port %d\n",PORT);
	fprintf(fout,"Options:\n");
	fprintf(fout,"  -l Enables log file\n");
	fprintf(fout,"  -v Enables verbose mode\n");
	fprintf(fout,"  -h Show this help\n");
}

/**
 * Gets the arguments.
 * @param argc Parameters amount.
 * @param argv Parameters.
 */
void getargs(int argc, char **argv)
{
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-l"))
				args.log = 1;
				
			else if (!strcmp(argv[i], "-v"))
				args.verbose = 1;
				
			else if (!strcmp(argv[i], "-h"))
			{
				usage(argv[0]);
				exit(0);
			}
		}
	}

	if (args.log)
	{
		fout = fopen("mts.log", "a+");
		if (fout == NULL)
		{
			fprintf(stdout,"%sMTS: An error has ocurred when open log file..\n",
				verbose());
			exit(-1);
		}
	}
}

int main(int argc, char **argv)
{	
	fout = stdout;
	getargs(argc, argv);
	startServer();
	return 0;
}
