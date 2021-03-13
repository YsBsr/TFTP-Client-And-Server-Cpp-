#define _CRT_SECURE_NO_WARNINGS
#include "TFTP_protocol.h"

TFTP_Proto::TFTP_Proto() {
	data_block_num = 1;
	ack_data_block_num = 0;
	error_count = 1;
	errmsg = "Someting went wrong between server and client.";
	cout << "TFTP Protocol initialized." << endl;
}

struct TFTPheader {
	short opcode;
};

struct TFTPdatablock {
	short datablock;
};


char* TFTP_Proto::makeRRQ(char* fileName, int& datalen, int& filenamelen) {
	struct TFTPheader header;
	header.opcode = htons(OPPCODE_RRQ);
	filenamelen = strlen(fileName) + 1;
	headersize = sizeof(header);
	datalen = headersize + filenamelen + 5 + 1;
	rrq = new char[datalen];
	memcpy(rrq, &header, headersize);
	memcpy(rrq + headersize, fileName, filenamelen);
	memcpy(rrq + headersize + filenamelen, mode, 6);
	return rrq;
	return rrq;
}



char* TFTP_Proto::makeWRQ(char* fileName, int& datalen, int& filenamelen) {
	struct TFTPheader header;
	header.opcode = htons(OPPCODE_WRQ);
	filenamelen = strlen(fileName) + 1;
	headersize = sizeof(header);
	datalen = headersize + filenamelen + 5 + 1;
	wrq = new char[datalen];
	memcpy(wrq, &header, headersize);
	memcpy(wrq + headersize, fileName, filenamelen);
	memcpy(wrq + headersize + filenamelen, mode, 6);
	return wrq;
}

char* TFTP_Proto::makeDataPacket(char* data_block, int& datalen) {
	struct TFTPheader header;
	header.opcode = htons(OPPCODE_DATA);
	struct TFTPdatablock block;
	block.datablock = htons(data_block_num++);
	headersize = sizeof(header);
	blocksize = sizeof(block);
	datalen = headersize + blocksize + TFTP_DATALEN;
	data_packet = new char[datalen];
	memcpy(data_packet, &header, sizeof(header));
	memcpy(data_packet + headersize, &block, blocksize);
	memcpy(data_packet + headersize + blocksize, data_block, TFTP_DATALEN);
	return data_packet;
}

char* TFTP_Proto::makeACKPacket(int& datalen) {
	struct TFTPheader header;
	header.opcode = htons(OPPCODE_ACK);
	struct TFTPdatablock block;
	block.datablock = htons(ack_data_block_num++);
	headersize = sizeof(header);
	blocksize = sizeof(block);
	datalen = headersize + blocksize + 1;
	ack_packet = new char[datalen];
	memcpy(ack_packet, &header, headersize);
	memcpy(ack_packet + headersize, &block, blocksize + 1);
	return ack_packet;
}

char* TFTP_Proto::makeErrorPacket() {
	struct TFTPheader header;
	header.opcode = htons(OPPCODE_ACK);
	struct TFTPdatablock block;
	block.datablock = htons(ack_data_block_num++);
	error_packet = new char[sizeof(header) + sizeof(block)];
	memcpy(error_packet, &header, sizeof(header));
	memcpy(error_packet + sizeof(header), &block, sizeof(block));
	memcpy(error_packet + sizeof(header) + sizeof(block), errmsg.c_str(), errmsg.length() + 2);
	return error_packet;
}

void TFTP_Proto::closeTFTPPROTO() {
	TFTP_Proto::~TFTP_Proto();
}

TFTP_Proto::~TFTP_Proto() {
	std::cout << "TFTP_Protocol is closed" << std::endl;
}
