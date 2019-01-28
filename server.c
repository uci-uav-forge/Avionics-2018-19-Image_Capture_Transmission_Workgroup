/*
Mohammad Gagai
Client for UAVForge

last updated : 12/28/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>


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
            printf("\n%s: enter 'pull' to receive an image,\n"
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

          
            /* If client types 'pull', wait to process*/
            if (0 == strcmp(SendBuf, "pull")) {
                
                /*Read Picture Size*/
                //printf("Reading Picture Size\n");
                int size;
                read(DataSocketFD, &size, sizeof(int));
                //printf("Size = %d\n",size);
                
                /*Store entire buffer into 'done.png'*/
                FILE *fp;
                fp = fopen("done.png","w");
                char buffer[size];
                read(DataSocketFD , buffer , size);
                
                for(int i = 0; i < size; i++) {
                    //printf("sendbuffer[%d] = %c \n",i,buffer[i]); //This line is for debugging only
                    fprintf(fp , "%c" , buffer[i]);     //Store char by char into done.png
                }
                fclose(fp);
                
                printf("The file was received successfully\n");
            }

	} while(!Bye && !Shutdown);
	printf("%s: Received last message from client, closing data connection.\n", argv[0]);
	close(DataSocketFD);

    } while(!Shutdown);

    printf("%s: Shutting down.\n", argv[0]);
    close(ServSocketFD);
    return 0;
}

/* EOF server.c */

