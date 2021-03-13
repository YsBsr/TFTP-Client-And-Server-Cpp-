
#include "TFTP_server.h"

TFTPserver::TFTPserver() {
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

bool TFTPserver::createSocket() {
	status = serverSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if (status == SOCKET_ERROR) {
		cout << "Error at socket creating. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	return true;
}

bool TFTPserver::bindSocket() {
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(69);
	server_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ZeroMemory(server_info.sin_zero, sizeof(server_info.sin_zero));


	status = bind(serverSocket, (struct sockaddr*) & server_info, sizeof(server_info));
	if (status == SOCKET_ERROR) {
		cout << "Error at binding. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	addr_size = sizeof(serverStorage);

	return true;
}

void TFTPserver::waitForARequest(ofstream& omyfile, ifstream& imyfile, char* path) {
	

	//Wait for Request//
	ZeroMemory(buffer, sizeof(buffer));
	bytes = recvfrom(serverSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & serverStorage, &addr_size);
	
	if (buffer[1] == OPPCODE_WRQ) {//Get WRQ//////////////////////////////////////////////////////////
		char* only_filename = &buffer[2];
		memcpy(path + strlen(path), only_filename, strlen(only_filename) + 1);
		omyfile.open(path, ios::binary);
		if (omyfile.is_open()) cout << "File is open" << endl;
		else {
			cout << "File not found. Quitting" << endl;
			WSACleanup();
			omyfile.close();
			return;
		}
		int datalen;
		char* a = tftp.makeACKPacket(datalen);
		sendto(serverSocket, a, datalen, 0, (struct sockaddr*) & serverStorage, addr_size);
		delete[] a;

		do {
			ZeroMemory(buffer, sizeof(buffer));
			bytes = recvfrom(serverSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & serverStorage, &addr_size);
			omyfile.write(&buffer[4], bytes - 4);
			a = tftp.makeACKPacket(datalen);
			sendto(serverSocket, a, datalen, 0, (struct sockaddr*) & serverStorage, addr_size);
		} while (bytes >= 512);

		omyfile.close();
		return;
	}
	else if (buffer[1] == OPPCODE_RRQ) {//Get RRQ//////////////////////////////////////////////////////////
		tftp.ack_data_block_num = 1;
		//Create the file in the directory given/////////////////////////////////
		memcpy(path + strlen(path), &buffer[2], strlen(&buffer[2]) + 1);
		imyfile.open(path, ios::binary);
		if (imyfile.is_open()) cout << "File is open" << endl;
		else {
			cout << "File not found. Quitting" << endl;
			WSACleanup();
			imyfile.close();
			return;
		}
		ZeroMemory(buffer, sizeof(buffer));
		//Create the file in the directory given/////////////////////////////////

		int datalen, n;
		char* a;

		while (!imyfile.eof()) {
			ZeroMemory(databuf, sizeof(databuf));
			imyfile.read(databuf, TFTP_DATALEN);
			n = imyfile.gcount();
			a = tftp.makeDataPacket(databuf, datalen);
			bytes = sendto(serverSocket, a, n + 4, 0, (struct sockaddr*) & serverStorage, addr_size);
			delete[] a;
			ZeroMemory(databuf, sizeof(databuf));
			bytes = recvfrom(serverSocket, buffer, TFTP_BUFLEN, 0, (struct sockaddr*) & serverStorage, &addr_size);
			if (buffer[1] == 4) {
				//cout << "ACK accepted." << endl;
			}
			else {
				cout << "Ackknowledge of data is missing. Something went wrong between server and client. Quitting." << endl;
				WSACleanup();
				return;
			}
		}
		imyfile.close();
		return;
	}
	else {
		cout << "There is no RRQ or WRQ accepted. Quitting." << endl;
		WSACleanup();
		imyfile.close();
		omyfile.close();
		return;
	}
	return;
}

void TFTPserver::closeTFTP() {
	WSACleanup();
	cout << "Windows Socket Architecture is closed." << endl;
}

TFTPserver::~TFTPserver() {
	cout << "TFTP protocol is closed." << endl;
}