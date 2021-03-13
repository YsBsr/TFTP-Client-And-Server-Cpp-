#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef TFTPSERVER
#define TFTPSERVER

#include "TFTP_protocol.h"

class TFTPserver {
private:
	SOCKET serverSocket;
	int status, bytes, addr_size;
	char buffer[TFTP_BUFLEN], databuf[TFTP_DATALEN];
	struct sockaddr_in server_info;
	struct sockaddr_storage serverStorage;
	string tbuf;
	TFTP_Proto tftp;

public:
	TFTPserver();
	~TFTPserver();
	bool createSocket();
	bool bindSocket();
	void waitForARequest(ofstream& omyfile, ifstream& imyfile, char* path);
	void closeTFTP();
};

#endif