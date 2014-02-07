#include "CSocket.h"

// Time to add the code for all the functions we defined in the class!

CSocket::CSocket()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData); // We'll use WinSock 2.2
}

CSocket::~CSocket()
{
	while (socket_ids.size() != 0)
	{
		SOCKET openSocket = socket_ids[socket_ids.size() - 1]; // Get the last socket from the array
		closesocket(openSocket);
		socket_ids.pop_back(); // Remove it from the array
	}

	WSACleanup(); // We're done using WinSock
}

SOCKET CSocket::OpenSocket(std::string address, unsigned short port)
{
	SOCKET sock;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Open the socket

	if (sock != INVALID_SOCKET) // We don't want to be saving an invalid socket!
	{
		socket_data sdata; // Save our data for later!
		sdata.ip = address;
		sdata.port = port;
		socket_list[sock] = sdata;
		socket_ids.push_back(sock);
	}

	return sock; // You'll have to check if the socket has opened yourself
}

bool CSocket::CloseSocket(SOCKET socketid)
{
	int retval = closesocket(socketid);

	if (retval == 0) // Is the socket closed?
	{
		socket_data *sdata = new socket_data; // Create an empty data struct
		socket_list[socketid] = *sdata; // Save it to the ID

		for (unsigned int i = 0; i < socket_ids.size(); i++)
		{
			if (socket_ids[i] == socketid)
				socket_ids.erase(socket_ids.begin()+i);
		}

		return true; // Socket closed successfully
	}

	return false; // Socket didn't close!
}

std::string CSocket::SendData(SOCKET socketid, std::string data)
{
	sockaddr_in *serveraddr;
	socket_data sockdata;

	sockdata = socket_list[socketid]; // Retrieve our socket info
	serveraddr = new sockaddr_in; // Empty the struct

	// Fill in connection info
	serveraddr->sin_family = AF_INET;
	serveraddr->sin_addr.s_addr = inet_addr(sockdata.ip.c_str());
	serveraddr->sin_port = htons(sockdata.port);

	int result = connect(socketid, (sockaddr*)serveraddr, sizeof(*serveraddr)); // Attempt connection
	if (result == SOCKET_ERROR)
	{
		return ""; // Return nothing if connection failed
	}

	result = send(socketid, data.c_str(), data.length(), 0); // Send the data
	if (result < 0)
		return ""; // Send failed, return empty string

	std::string response = "";
	int response_len;
	char buffer[1000000]; // A one million character string response buffer
	do
	{
		response_len = recv(socketid, buffer, 1000000, 0);
		if (response_len > 0)
			response += std::string(buffer).substr(0, response_len);
		// NOTE: I hope you have fast internet and good connection since download lag
		// isn't handled here
	} while (response_len > 0);

	return response; // Return the valid response
}

socket_data CSocket::GetSocketData(SOCKET socketid)
{
	return socket_list[socketid]; // Return the data from the map
}

std::vector<SOCKET> CSocket::GetSocketList()
{
	return socket_ids; // Return the entire socked ID array
}