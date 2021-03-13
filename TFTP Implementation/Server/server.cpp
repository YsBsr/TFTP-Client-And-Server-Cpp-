#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "TFTP_server.h"

using namespace std;

char path[] = "C:\\Root\\Directory\\Of\\Server\\";

int main() {
	TFTPserver server;
	bool status;
	status = server.createSocket();
	if (!status) {
		server.closeTFTP();
		exit(0);
	}

	status = server.bindSocket();
	if (!status) {
		server.closeTFTP();
		exit(0);
	}

	ofstream oFile;
	ifstream iFile;
	server.waitForARequest(oFile, iFile, path);
	server.closeTFTP();
	system("pause");
	return 0;
}