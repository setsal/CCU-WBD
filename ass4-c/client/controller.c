#include <WINSOCK2.H>
#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] )
{
    WSADATA wsaData;
    WORD versionRequired=MAKEWORD(1,1);
    WSAStartup(versionRequired,&wsaData);
    SOCKET sock_fd;
    SOCKADDR_IN  servaddr;

    char buff[100];
    char sendbuff[20];
    memset( sendbuff, 0, sizeof(sendbuff) );
    strcat( sendbuff, argv[1] );
    strcat( sendbuff, " " );
    strcat( sendbuff, argv[2] );
    printf("[INFO] Request: %s\n", sendbuff);


    const char *str = sendbuff;
    const char *serverip = "127.0.0.1";
    short port = 6666;
    sock_fd = socket(PF_INET,SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.S_un.S_addr = inet_addr(serverip);
    memset(buff,0,sizeof(buff));
    sendto(sock_fd, str, strlen(str), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    recvfrom(sock_fd, buff, sizeof(buff), 0, NULL, NULL);
    printf("%s\n", buff);
    closesocket(sock_fd);
    WSACleanup();
    return 0;
}
