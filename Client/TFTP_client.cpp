#define _CRT_SECURE_NO_WARNINGS
#include "TFTP_client.h"

TFTPclient::TFTPclient() {

	//WSA initialize
	WSADATA wsadata;
	WORD version = MAKEWORD(2, 2);
	status = WSAStartup(version, &wsadata);
	if (status != 0) {
		cout << "Error at Windows Socket Architecture initialization. Error code: " << WSAGetLastError() << endl;
		cout << "Quitting" << endl;
		system("pasuse");
		exit(0);
	}
	cout << "Windows Socket Architecture initialized." << endl;
}

bool TFTPclient::createSocket() {
	status = clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if (status == SOCKET_ERROR) {
		cout << "Error at socket creating. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(69);
	server_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ZeroMemory(server_info.sin_zero, sizeof(server_info.sin_zero));

	addr_size = sizeof(server_info);

	return true;
}

struct rrq {
	short opcode;
	char* data;
};

void TFTPclient::sendData(ifstream& myfile, char* path, char* name_of_the_file_to_upload) {

	char* file_name = new char[strlen(path) + strlen(name_of_the_file_to_upload) + 1];

	memcpy(file_name, path, strlen(path));
	memcpy(file_name + strlen(path), name_of_the_file_to_upload, strlen(name_of_the_file_to_upload) + 1);

	myfile.open(file_name, ios::binary);
	
	if (!myfile.is_open()) {
		cout << "Directory not found" << endl;
		WSACleanup();
		myfile.close();
		return;
 	}
	
	struct sockaddr_in peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int len = sizeof(peeraddr);

	//send WRQ//
	int datalen, filenamelen;
	char* a = tftp.makeWRQ(name_of_the_file_to_upload, datalen, filenamelen);
	sendto(clientSocket, a, datalen, 0, (struct sockaddr*) & server_info, sizeof(struct sockaddr_in));
	//send WRQ//

	delete[] a;

	//first ACK
	bytes = recvfrom(clientSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & peeraddr, &len);
	cout << buffer << endl;
	int n;
	if (buffer[1] == 4) {
		//cout << "ACK accepted. ACK packet number: " << &buffer[2] << endl;
	}
	else {
		cout << "Ackknowledge of data is missing. Something went wrong between sever and client. Quitting." << endl;
		sendto(clientSocket, NULL, 50, 0, (struct sockaddr*) & peeraddr, sizeof(struct sockaddr_in)); //tell the server the transmission is done.
		return;
	}
	char* b;
	while (!myfile.eof()) {
		ZeroMemory(databuf, sizeof(databuf));
		myfile.read(databuf, TFTP_DATALEN);
		n = myfile.gcount();
		b = tftp.makeDataPacket(databuf, datalen);
		bytes = sendto(clientSocket, b, n + 4, 0, (struct sockaddr*) & peeraddr, sizeof(struct sockaddr_in));
		delete[] b;
		ZeroMemory(buffer, sizeof(buffer));
		bytes = recvfrom(clientSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & peeraddr, &len);
		if (buffer[1] == 4) {
			//cout << "ACK accepted." << endl;
		}
		else {
			cout << "Ackknowledge of data is missing. Something went wrong between server and client. Quitting." << endl;
			WSACleanup();
			return;
		}
	}
	
	myfile.close();
}

void TFTPclient::recvData(ofstream& myfile, char* path, char* name_of_the_file_to_download) {

	char* file_name = new char[strlen(path) + strlen(name_of_the_file_to_download) + 1];

	memcpy(file_name, path, strlen(path));
	memcpy(file_name + strlen(path), name_of_the_file_to_download, strlen(name_of_the_file_to_download) + 1);

	myfile.open(file_name, ios::binary);

	if (!myfile.is_open()) {
		cout << "Directory not found" << endl;
		WSACleanup();
		myfile.close();
		return;
	}

	//send RRQ////////////////////////////////////////////////////////
	int datalen, filenamelen;
	char* a = tftp.makeRRQ(name_of_the_file_to_download, datalen, filenamelen);
	sendto(clientSocket, a, datalen, 0, (struct sockaddr*) & server_info, addr_size);
	//send RRQ//////////////////////////////////////////////////////////
	delete[] a;
	sockaddr_in server;
	int len = sizeof(server);

	//Get first data block/////////////////////////////////////////////////////////
	ZeroMemory(buffer, sizeof(buffer));
	bytes = recvfrom(clientSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & server, &len);
	if (buffer[1] == 3) cout << "Data packet accepted." << endl;
	else {
		cout << "Data packet is not accepted. Quitting." << endl;
		WSACleanup();
		myfile.close();
		return;
	}
	myfile.write(&buffer[4], bytes - 4);
	//Get first data block/////////////////////////////////////////////////////////
	char* b = tftp.makeACKPacket(datalen);

	sendto(clientSocket, b, datalen, 0, (struct sockaddr*) & server, len);
	delete[] b;
	while (bytes >= 512) {
		ZeroMemory(buffer, sizeof(buffer));
		bytes = recvfrom(clientSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & server, &len);
		myfile.write(&buffer[4], bytes - 4);
		//cout << "Received to server: " << buffer << endl;
		b = tftp.makeACKPacket(datalen);
		sendto(clientSocket, b, datalen, 0, (struct sockaddr*) & server, len);
		delete[] b;
	}
	myfile.close();

	delete[] file_name;
}

void TFTPclient::closeTFTP() {
	WSACleanup();
	cout << "Windows Socket Architecture is closed." << endl;
}

TFTPclient::~TFTPclient() {
	cout << "TFTP protocol is closed." << endl;
}

