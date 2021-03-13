#ifndef TFTPPROTO
#define TFTPPROTO

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <string>

#define TFTP_BUFLEN 1024
#define TFTP_DATALEN 512

#define TFTPMODE_NETASCII 0
#define TFTPMODE_OCTET 1

#define OPPCODE_RRQ 1
#define OPPCODE_WRQ 2
#define OPPCODE_DATA 3
#define OPPCODE_ACK 4
#define OPPCODE_ERROR 5

using namespace std;

class TFTP_Proto {
private:
	char* rrq; // RRQ - Read Request
	char* wrq;// WRQ - Write Request
	char* data_packet;
	char* ack_packet;
	char* error_packet;

	string errmsg;
	char mode[6] = "octet";
	char zero;

	unsigned short int data_block_num; // use only for makeDataPacket
	unsigned short int error_count; // use only for makeErrorPacket
	unsigned short int headersize;
	unsigned short int blocksize;

public:
	short ack_data_block_num; // use only for makeACKPacket
	TFTP_Proto();
	~TFTP_Proto();
	char* makeRRQ(char* fileName, int& datalen, int& filenamelen);
	char* makeWRQ(char* fileName, int& datalen, int& filenamelen);
	char* makeDataPacket(char* data_block, int& datalen);
	char* makeACKPacket(int& datalen);
	char* makeErrorPacket();
	void closeTFTPPROTO();
	
};

#endif