/*
Mohammad Gagai
Client for UAVForge

last updated: 03/07/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

char fname[100];	//Name of the file
int SocketFD;		//socket file descriptor 
char terminator[20] = "jerrrrybeanz";

int s;
pthread_t tid;

void* SendFileToClient(void *arg)
{

       write(SocketFD, fname,256);
        FILE *fp = fopen(fname,"rb");
        if(fp==NULL)
        {   
            printf("File opern error");
            return (int*)1;
        }
        /* Read data from file and send it */
        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[1024]={0};
            int nread = fread(buff,1,1024,fp);
            
            /* If read was success, send data. */
            if(nread > 0)
            {   
                write(SocketFD, buff, nread);
            }
            if (nread < 1024)
            {   
                if (feof(fp))
                {   
                    printf("End of file\n");
                    printf("File transfer completed for id: %d\n",SocketFD);
                }
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }

	sleep(1);
	write(SocketFD, terminator, 12);
	printf("Closing Connection for id: %d\n",SocketFD);
	sleep(2);
	return 0;
}


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

    int n;
    int PortNo;		/* port number */
    struct sockaddr_in
	ServerAddress;	/* server address we connect with */
    struct hostent
	*Server;	/* server host */
    char RecvBuf[256];	/* message buffer for receiving a response */



/*	Start of Socket code: 	*/
    printf("%s: Starting...\n", argv[0]);
    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
	exit(10);
    }

    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", argv[0], argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);

    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be greater 2000\n",
		argv[0], PortNo);
        exit(10);
    }

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {   FatalError(argv[0], "socket creation failed");
    }

    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    memcpy(&ServerAddress.sin_addr.s_addr,
		Server->h_addr_list[0], Server->h_length);
    printf("%s: Connecting to the server...\n", argv[0]);
    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(ServerAddress)) < 0)
    {   FatalError(argv[0], "connecting to server failed");
    }
/*      End of Socket code   */


/*	Main Code	*/
	do{
        
        /* Process the request sent by server (new, bye, shutdown) */
        n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
        if (n < 0) {
            FatalError(argv[0], "reading from data socket failed");
            }
        RecvBuf[n] = 0;

        /*If server sent string "new"*/
        if( !strcmp(RecvBuf,"new") ) {
           printf("Enter file name to send: ");
           gets(fname);

	   int err;
           err = pthread_create(&tid, NULL, &SendFileToClient, &SocketFD);

	   sleep(2);
	   pthread_cancel(tid);

	   void *res;

	   s = pthread_join(tid, &res);
    	   if (s != 0)
		printf("pthread_join error\n");

	   /*For Debugging
	   if (res == PTHREAD_CANCELED)
        	printf("main(): thread was canceled\n");
    	   else
        	printf("main(): thread wasn't canceled (shouldn't happen!)\n");
	   */
	}

        /*If server sent string "bye"*/
        if( !strcmp(RecvBuf,"bye") )
		break;

        /*If server sent string "shutdown"*/
        if( !strcmp(RecvBuf,"shutdown") )
		break;
        
    } while(  (0 != strcmp("bye", RecvBuf)) &&(0 != strcmp("shutdown", RecvBuf)));
    
    printf("%s: Exiting...\n", argv[0]);
    close(SocketFD);

    return 0;
}

/* EOF client.c */

