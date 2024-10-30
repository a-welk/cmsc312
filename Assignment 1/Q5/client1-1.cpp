#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <cstdlib>
 
using namespace std;

#define PORT 2081
 
int main () {

	int sock1;
	string nums;
	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv;
 
	serv.sin_port = htons(PORT);
        cout << "PORT" << PORT << "   " << htons(PORT) << endl;
	//printf("%x %x\n",PORT,htons(PORT));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "CLIENT CONNECTING" << endl;
	//printf("client connecting\n");
	connect(sock1, (struct sockaddr *)&serv, sizeof(serv));
	cout << "Enter numbers to be sorted: ";
	getline(cin, nums);
	write(sock1, nums.c_str(), nums.size() + 1);

	close(sock1);
	return 0;
}
