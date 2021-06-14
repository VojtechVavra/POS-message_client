#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h> // socket
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // write
#include <string.h>     // strlen
#include <cstdlib>

#define TCPPORT 8000 
#define TCPIP   "158.196.145.111"
#define UDPSERVERPORT 8010
#define MSG_SIZE 255

void *tcpClient(void* args);
void *udpServer(void* port);

struct Args {
    char* ip;
    int port;
};

int main()
{
    Args args = { .ip = TCPIP, .port = TCPPORT};
    pthread_t tcpThread_id, udpThread_id; 
    int udpServerPort = UDPSERVERPORT;

    if (pthread_create(&tcpThread_id, NULL, tcpClient, (void *)&args) != 0) {
        printf("Failed to create first thread\n");
    }
    if (pthread_create(&udpThread_id, NULL, udpServer, (void *)&udpServerPort) != 0) {
        printf("Failed to create second thread\n");
    }

    // wait for both threads
    pthread_join(tcpThread_id, NULL);
    pthread_join(udpThread_id, NULL);

    printf("End of program\n"); 

    return 0;
}

void *tcpClient(void* newArgs)
{
    Args server = *((Args *)newArgs);
    printf("First thread\nip: %s, port: %d\n", server.ip, server.port);

    int sock = 0, valread; 
	struct sockaddr_in server_addr; 
	char msg_buffer[MSG_SIZE] = {0}; 
    char server_reply[MSG_SIZE];
    char* host = NULL;

    // Create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error - "); 
		printf("Exitting tcp thread!\n");
        pthread_exit(NULL); 
	} 

    puts("client socket created");

	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(server.port);
	
	// Convert IPv4 and IPv6 addresses from text to binary form into server_addr struct
	if(inet_pton(AF_INET, server.ip, &server_addr.sin_addr) <= 0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		pthread_exit(NULL); 
	} 

    // Connect to remote server
	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		pthread_exit(NULL); 
	}
    puts("Connected\n");

    do {
        memset(msg_buffer, '\0', MSG_SIZE);
        fgets(msg_buffer, MSG_SIZE, stdin);

        if (!strcmp(msg_buffer, "quit")) {
            write(sock, msg_buffer, strlen(msg_buffer));
            close(sock);
            pthread_exit(NULL);
        }
        else {
            msg_buffer[MSG_SIZE] = '\0';
            msg_buffer[strlen(msg_buffer)] = '\n';
            write(sock, msg_buffer, strlen(msg_buffer));
        }
    } while (1);

	close(sock);

    printf("Exitting tcp thread!\n");
    pthread_exit(NULL);
}

void *udpServer(void* newPort)
{
    int port = *((int *) newPort);
    printf("UDP server port: %d\n", port);

    while(1) {
        int sock, i;
        struct sockaddr_in saMy, saHis;
        char buf[MSG_SIZE] = { 0 };
        long addr;
        int bytesRead;  //, fromLen; // fromLen nesmi byt int, ale socklen_t
        socklen_t fromLen;

        // otevreme datagramovy socket v rodine Internet
        sock = socket(PF_INET, SOCK_DGRAM, 0);

        if (sock == -1) {
            printf("Server: Error Opening socket %d\n", errno);
            pthread_exit(NULL);
        }

        // inicializace lokalni adresy
        saMy.sin_family = AF_INET;
        saMy.sin_port = htons(port);
        saMy.sin_addr.s_addr = INADDR_ANY;

        // navazani socketu na lokalni adresu
        if (bind(sock, (struct sockaddr *) &saMy, sizeof(saMy)) == -1) {
            printf("bind() failed with error %d\n", errno);
            close(sock);
            pthread_exit(NULL);
        }

        printf("Waiting for datagram at port %d\n", port);

        fromLen = sizeof(struct sockaddr); 

        // pockame na datagram, az prijde, ulozime do 'buf',
        // zdrojovou adresu do 'saHis'

        bytesRead = recvfrom(sock, buf, MSG_SIZE, 0, (struct sockaddr*) &saHis, &fromLen);

        if (bytesRead == -1) { 
            printf("Error reading datagram\n"); 
            close(sock); 
            pthread_exit(NULL);
        }

        // parsovani dat
        char fromIp[200] = { 0 };
        sprintf(fromIp, "%u.%u.%u.%u", 
            (unsigned char)buf[0], (unsigned char)buf[1], (unsigned char)buf[2], (unsigned char)buf[3]);

        unsigned long int timeStampMsg = (((unsigned char)buf[4] << 24) | 
            ((unsigned char)buf[5] << 16) | ((unsigned char)buf[6] << 8) | (unsigned char)buf[7]);
        time_t send_time = timeStampMsg;
        struct tm* dt;
        dt = localtime(&send_time);

        //printf ("Current local time and date: %s", asctime(dt));
        printf("ip: %s, date: %s", fromIp, asctime(dt));
        printf("msg: ");
        for (i = 9; i < bytesRead; i++) {
            printf("%c", buf[i]);
        }
        printf("\n\n");

        close(sock);
    }


    printf("Exitting udp thread!\n");
    pthread_exit(NULL);
}
