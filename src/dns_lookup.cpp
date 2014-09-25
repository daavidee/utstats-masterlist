#include <string>
#include <winsock.h>

std::string dns_lookup(char query[50]) {
 	WSADATA wsaData; 
	hostent *host;           
    in_addr ipHost;        
	int i, isIP = 0;            
	char buffer[50][3];
    if(WSAStartup(0x0101, &wsaData) != 0)
    {
      	WSACleanup();  
		return "failed";
    }
    host = gethostbyname(query);
    if(!host)
    {
   	    WSACleanup();
        return "failed";
    }
    sprintf(buffer[0], "%s",
    inet_ntoa(*( (struct in_addr *)host->h_addr) ) );
	WSACleanup();
    return buffer[0];
}
