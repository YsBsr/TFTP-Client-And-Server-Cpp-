#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "TFTP_client.h"
#include <thread>


using namespace std;

char path[] = "C:\\Root\\Directory\\Of\\Client\\";  // This directory is used for both download and 

int main() { 
	this_thread::sleep_for(chrono::seconds(1));

	ifstream iFile;
	ofstream oFile;
	TFTPclient client;
	bool status;
	status = client.createSocket();
	if (!status) {
		client.closeTFTP();
		exit(0);
	}
	client.sendData(iFile, path, (char *)"file_name_to_upload.extension"); // TO make a WRQ (Making Write Request to upload the file to the server machine)
	client.recvData(oFile, path, (char *)"file_name_to_download.extension"); // TO make a RRQ (Making Read Request to download the file from the server machine)
	client.closeTFTP();
	system("pause");
	return 0;
}