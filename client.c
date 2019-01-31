/*
Mohammad Gagai
Client for UAVForge

last updated: 01/25/2019
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
    int n;
    int SocketFD,	/* socket file descriptor */
	PortNo;		/* port number */
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
        
        /* Process the request sent by server (pull, bye, shutdown) */
        n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
        if (n < 0) {
            FatalError(argv[0], "reading from data socket failed");
            }
        RecvBuf[n] = 0;
        printf("%s: Received message: %s\n", argv[0], RecvBuf);

        
        /*If server sent string "pull"*/
        if( !strcmp(RecvBuf,"pull") ) {
            //printf("TRYING TO PUSH IMAGE\n");
            
            /*Getting size of image:*/
            //printf("Getting Picture Size\n");
            FILE *filePointer;
            filePointer = fopen("star.png", "r");
            int size;
            fseek(filePointer, 0, SEEK_END);
            size = ftell(filePointer);
            //printf("size of the image is = %d\n",size);

            /*Sets the file pointer to the beginning of the image*/
            fseek(filePointer, 0, SEEK_SET);
            rewind(filePointer);
            
            /*Send the size of the image to server*/
            //printf("Sending Picture Size\n");
            write(SocketFD, &size, sizeof(size));

            /*Sending Image here : */
            //printf("Sending Picture as Byte Array\n");
            char send_buffer[size]; // For now, this send_buffer is same as the file size (sending in one chunk)
            int nb = fread(send_buffer, 1, sizeof(send_buffer), filePointer);
            
            while(!feof(filePointer)) {   // This while loop only runs once for now cuz sending in 1 chunk
                //printf("nb = %d\n",nb);
                write(SocketFD, send_buffer, nb);   //Sending entire image to server
                
                /*This For Loop is only for debugging*/
                /*Shows all the characters in the buffer one by one*/
                //for(int i = 0; i < size; i++)
                    //printf("sendbuffer[%d] = %c \n",i,send_buffer[i]);
                
                nb = fread(send_buffer, 1, sizeof(send_buffer), filePointer);
            }
            //printf("DONE WHILE LOOP\n");
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

