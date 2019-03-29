/*
Mohammad Gagai
Client for UAVForge

last updated : 03/07/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

void FatalError(const char *Program, const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!", stderr);
    exit(20);
} /* end of FatalError */

int main(int argc, char *argv[])
{

    char terminator[20] = "jerrrrybeanz";

    char finalName[100] = "done";
    //char done[10] = "done";
    char ext[10] = ".jpg";
    char number[5] = "1";
    int first = 0;
    int counter = 0;
    char c;


    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));

    int l, n;
    int ServSocketFD,	/* socket file descriptor for service */
	DataSocketFD,	/* socket file descriptor for data */
	PortNo;		/* port number */
    socklen_t ClientLen;
    struct sockaddr_in
	ServerAddress,	/* server address (this host) */
	ClientAddress;	/* client address we connect with */
    char SendBuf[256];	/* message buffer for sending a response */
    int Bye = 0,
	Shutdown = 0;

    printf("%s: Starting...\n", argv[0]);
    if (argc < 2)
    {   fprintf(stderr, "Usage: %s port\n", argv[0]);
	exit(10);
    }
    PortNo = atoi(argv[1]);	/* get the port number */

    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be greater 2000\n",
		argv[0], PortNo);
        exit(10);
    }

    ServSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   FatalError(argv[0], "service socket creation failed");
    }

    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(ServSocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(ServerAddress)) < 0)
    {   FatalError(argv[0], "binding the server to a socket failed");
    }
    printf("%s: Listening on port %d...\n", argv[0], PortNo);
    if (listen(ServSocketFD, 5) < 0)	/* max 5 clients in backlog */
    {   FatalError(argv[0], "listening on socket failed");
    }



	/* Main Loop */
    do{
        
        Shutdown = 0;
        ClientLen = sizeof(ClientAddress);
        DataSocketFD = accept(ServSocketFD, (struct sockaddr*)&ClientAddress,
                              &ClientLen);
        if (DataSocketFD < 0)
        {   FatalError(argv[0], "data socket creation (accept) failed");
        }
        printf("%s: Accepted connection from client.\n", argv[0]);

        do{
            /*Let user select which option he wants*/
            /*If none of these typed, will just send message to client*/
            Bye = 0;
            printf("\n%s: enter 'new' to receive an image,\n"
            "   enter 'bye' to close the client,\n"
            "   enter 'shutdown' to close the client and server:\n"
            "message: ", argv[0]);

            /*Sending message to Client*/
            fgets(SendBuf, sizeof(SendBuf), stdin);
            l = strlen(SendBuf);
            if (SendBuf[l-1] == '\n')
            {   SendBuf[--l] = 0;
            }
            if (l) {
                printf("%s: Sending message '%s'...\n", argv[0], SendBuf);
                /* write() pushes message from SendBuf --> Client RecvBuf */
                n = write(DataSocketFD, SendBuf, l);
                if (n < 0)
                {   FatalError(argv[0], "writing to socket failed");
                }
                printf("%s: Waiting for response...\n", argv[0]);
                /* End of write () */
            }
            
            
            /* If client types bye, loop doesn't run again*/
            if (0 == strcmp(SendBuf, "bye"))
            {   Bye = 1;
                strncpy(SendBuf, "server bye", sizeof(SendBuf)-1);
                SendBuf[sizeof(SendBuf)-1] = 0;
            }
            
            /* If client types shutdown, loop doesn't run again*/
            else if (0 == strcmp(SendBuf, "shutdown"))
            {   Shutdown = 1;
                strncpy(SendBuf, "server shutdown", sizeof(SendBuf)-1);
                SendBuf[sizeof(SendBuf)-1] = 0;
            }


            /* If client types 'new', wait to process*/
            if (0 == strcmp(SendBuf, "new")) {
         	/* Create file where data will be stored */
        	FILE *fp;
        	char fname[100];
        	read(DataSocketFD, fname, 256);
        	printf("File Name: %s\n",fname);
        	printf("Receiving file...");

		/* Output file renaming:  */
		if( first == 0) {
		   c = number[0];	
		   strcat(finalName,number);
                   strcat(finalName,ext);
		   counter++;

                   fp = fopen(finalName, "wb");
                   if(NULL == fp) {
                      printf("Error opening file");
                      return 1;
                   }
		}

		else {
    		   char done[10] = "done";
		   
		   if (counter == 9) {
                	c = 48;
		   }

		   counter++;
		   c = c + 1;
		   number[0] = c;
                   strcat(done,number);
                   strcat(done,ext);
	
                   fp = fopen(done, "wb");
                   if(NULL == fp) {
                      printf("Error opening file");
                      return 1;
                   }
		}

	    first = 1;

	    long double sz=1;

    	    /* Receive data in chunks of 1024 bytes (1KB) */
	    char test[20];
    	    while((bytesReceived = read(DataSocketFD, recvBuff, sizeof(recvBuff)-1  )) > 0) {
		if (bytesReceived != 1023)	//For Debugging
			//printf("bytereceived = %d",bytesReceived);
		sz++;

		for (int i = 0; i< 12; i++)
		test[i] = recvBuff[i];

	    if(!strcmp(test,terminator)) 
		break;
	
                fwrite(recvBuff, 1,bytesReceived,fp);
	    }
	
    	    if(bytesReceived < 0) {
        	printf("\n Read Error \n");
    	    }

	    printf("\nFile OK....Completed\n");

            fclose(fp);
            }

	} while(!Bye && !Shutdown);
	close(DataSocketFD);

    } while(!Shutdown);

    printf("%s: Shutting down.\n", argv[0]);
    close(ServSocketFD);
    return 0;
}

/* EOF server.c */

