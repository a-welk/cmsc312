#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

 
#define PORT 2081
 
int main()
{
	int sock1,sock2, clength;
	sock1 =  socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv,cli;
 
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind(sock1,(struct sockaddr *)&serv, sizeof(serv));
	listen(sock1,5);
	clength = sizeof(cli);
	int i=0;
	char buf[50];
    while(1){
		sock2 = accept(sock1,(struct sockaddr *)&cli, (socklen_t *)&clength);
        cout << "\n CLIENT CONNECTED" << endl;

        read(sock2, buf, 50);
		stringstream stream(buf);
		vector<int> nums;

		int num;
		while (stream >> num) {
			nums.push_back(num);
			if(stream.peek() == ',') {
				stream.ignore();
			}
		}
		sort(nums.begin(), nums.end());

		stringstream sortedStream;

		for(int i : nums) {
			sortedStream << i << " ";
		}

		string sortedString = sortedStream.str();
		cout << "Sorted numbers: " << sortedString << endl;

	close(sock2);
	}

	return 0;

}
 
