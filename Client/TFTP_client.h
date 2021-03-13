#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef TFTPCLIENT
#define TFTPCLIENT

#include <iostream>
#include "TFTP_protocol.h"

class TFTPclient {

private:
	SOCKET clientSocket;
	int status, bytes, addr_size;
	struct sockaddr_in server_info;
	string tbuf;
	char buffer[TFTP_BUFLEN];
	char databuf[TFTP_DATALEN];
	TFTP_Proto tftp;

public:
	TFTPclient();
	~TFTPclient();
	bool createSocket();
	void sendData(ifstream& myfile, char* path, char* name_of_the_file_to_upload);
	void recvData(ofstream& myfile, char* path, char* name_of_the_file_to_download);
	void closeTFTP();
};

#endif