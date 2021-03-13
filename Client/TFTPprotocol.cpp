#define _CRT_SECURE_NO_WARNINGS
#include "TFTP_protocol.h"

TFTP_Proto::TFTP_Proto() {
	data_block_num = 0;
	ack_data_block_num = 0;
	error_count = 0;
	std::cout << "TFTP Protocol initialized." << std::endl;
}

string TFTP_Proto::makeRRQ(char* fileName, int mode) {
	rrq = OPPCODE_WRQ;
	rrq = rrq + string(fileName);
	return rrq;
}

string TFTP_Proto::makeWRQ(char* fileName, int mode) {
	wrq = OPPCODE_WRQ;
	wrq = wrq + string(fileName);
	return wrq;
}

string TFTP_Proto::makeDataPacket(char* data_block) {
	data_packet = OPPCODE_DATA;
	data_packet = data_packet + string(data_block);
	return data_packet;
}

string TFTP_Proto::makeACKPacket() {
	ack_packet = OPPCODE_ACK;
	ack_packet = ack_packet + to_string(ack_data_block_num++);
	return ack_packet;
}

string TFTP_Proto::makeErrorPacket() {
	error_packet = OPPCODE_ERROR;
	error_packet = to_string(error_count++);
	return error_packet;
}

void TFTP_Proto::closeTFTPPROTO() {
	TFTP_Proto::~TFTP_Proto();
}

TFTP_Proto::~TFTP_Proto() {
	std::cout << "TFTP_Protocol is closed" << std::endl;
}
