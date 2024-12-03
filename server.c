#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, const char * argv[]) {
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Example:\n\t%s 8888\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buf[1024] = {0};
	int port = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

//	int priority = 2;
//	setsockopt(sockfd, SOL_SOCKET, SO_PRIORITY, &priority, sizeof(priority));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    memset(buf, 0, sizeof(buf));

    struct timeval start, end, begin, finish;
    long received_bytes = 0;
    double elapsed_time, delta_time;
    unsigned long int recv_total = 0;
    int numbytes = 0;
	while(1) {
		numbytes = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &client_addr, &client_len); 
        if(numbytes > 0 && recv_total == 0) {
            gettimeofday(&start, NULL);
            begin = start;
        }

        recv_total += numbytes;
        gettimeofday(&end, NULL);
        elapsed_time = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0);
        if(elapsed_time >= 1) {
            gettimeofday(&finish, NULL);
            delta_time = ((finish.tv_sec - begin.tv_sec) + (finish.tv_usec - begin.tv_usec) / 1000000.0);
            printf("Recevive %ld bytes, BandWidth=%fMbit/sec\n", 
                    recv_total, recv_total * 8 / delta_time / 1024 / 1024.0);
            gettimeofday(&start, NULL);
		}	
	}

    close(sockfd);
    return 0;
}
