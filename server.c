/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


void *connection_handler(void *socket_desc);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int connectionIndex = 0;
int connections[10];

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, newsockfd2;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int pid;

    if (argc < 2)
    {
        fprintf(stderr,"You need to provide a port!\n");
        exit(1);
    }

    //connectionNumber = atoi(argv[2]);
    //connections[] = malloc(sizeof(int) * connectionNumber);


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,10);
    clilen = sizeof(cli_addr);

    pthread_t thread_id;

    while ( (newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen)))
    {
        if (pthread_create(&thread_id, NULL, connection_handler, (void *) &newsockfd) < 0)
        {
            error("Can't create thread");
            exit(0);
        }
    }

    if (newsockfd < 0)
    {
        error("Error on accept");
        return 1;
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    connections[connectionIndex++] = sock;

    int read_size;
    char *message, client_message[2000];
    int n;
    int counter;

    //char *processed;

    while ( (read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        for (counter = 0; counter < connectionIndex; counter++)
        {
            if (connections[counter] != sock)
            {
                //processed = (char*)malloc(sizeof(char) * strlen(client_message));
                //strcpy(processed, client_message);

                printf("%s", client_message);
                n = write(connections[counter], client_message, strlen(client_message));
                if (n < 0) error("ERROR writing to socket");
            }
        }


        for (counter = 0 ; counter < 2000; counter++)
        {
            client_message[counter] = '\0';
        }
       // n = write(sock,"I got your message",18);
        //if (n < 0) error("ERROR writing to socket");
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("Read failed");
    }
}
