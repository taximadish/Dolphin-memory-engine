#include "Client.h"

#include <WinSock2.h>
#include <Ws2tcpip.h>

Client::Client()
{
  m_storedData = "";
  m_running = false;
  m_memManager = new MemManager(false);
}

void Client::update()
{
  if (!m_running)
    return;

	fd_set rfds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(m_socket, &rfds);
	int recVal = select(m_socket + 1, &rfds, NULL, NULL, &timeout);
	if (recVal > 0) // -1 = Error, 0 = Would Block
	{
		const int RECV_SIZE = 1024;
		char recvData[RECV_SIZE + 1] = {'\0'};
		int bytesReceived = recv(m_socket, recvData, RECV_SIZE, 0);
		if (bytesReceived <= 0)
		{
		stop();
		}

		m_storedData.append(std::string(recvData));

		size_t delimPos;
		while ((delimPos = m_storedData.find("/")) != std::string::npos) // we have a whole value
		{
		std::string entry = m_storedData.substr(0, delimPos);
		m_storedData = m_storedData.substr(delimPos + 1);

		std::vector<std::string> parts = customSplit(entry, ";");
		std::string name = parts[0];
		int8_t ack = atoi(parts[1].c_str());
		std::string value = parts[2];

		if (m_updateAcks[name] != ack)
			continue;

		std::string pastHostVal;
		if (!m_pastValues.count(name)) // not in map yet
		{
			m_pastValues[name] = value;
		}
		pastHostVal = m_pastValues[name];
		std::string localChanges = m_memManager->getUpdate(name, pastHostVal);
		if (localChanges.length() > 0)
		{
			m_updateAcks[name]++;
			std::string sendData = name + ";";
            sendData.append(std::to_string(m_updateAcks[name]) + ";");
			sendData.append(localChanges);
			sendData.append("/");
			send(m_socket, sendData.c_str(), sendData.length(), 0);
		}

		std::string setValue = m_memManager->setEntryValue(name, value);
        if (setValue != "__NOT_SET__")
			m_pastValues[name] = setValue;
		}
	}
  
}

bool Client::start(std::string address, int port)
{
  const int iReqWinsockVer = 2; // Minimum winsock version required

  WSADATA wsaData;

  if (WSAStartup(MAKEWORD(iReqWinsockVer, 0), &wsaData) == 0)
  {
    // Check if major version is at least iReqWinsockVer
    if (LOBYTE(wsaData.wVersion) >= iReqWinsockVer)
      return createConnection(address, port);
    else
      return false;
  }
  else
  {
    return false;
  }
}

bool Client::createConnection(std::string address, int portno)
{
  if (address == "localhost")
    address = "127.0.0.1";
  if (portno == 0)
	  portno = 14321;

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0)
    return false;

  // Binding info
  sockaddr_in sockAddr;
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(portno);
  inet_pton(AF_INET, address.c_str(), &sockAddr.sin_addr.S_un.S_addr);

	if (::connect(m_socket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		return false;
	}

	m_running = true;
	return true;
}

bool Client::stop()
{
  closesocket(m_socket);
  m_running = false;
  return WSACleanup();
}


std::vector<std::string> Client::customSplit(std::string s, std::string delim)
{
  std::vector<std::string> parts = {};
  std::string delimiter = delim;

  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    parts.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  if (s.length() > 0)
  {
    parts.push_back(s);
  }
  return parts;
}
