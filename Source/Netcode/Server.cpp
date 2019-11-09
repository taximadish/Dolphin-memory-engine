#include "Server.h"

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <map>

Server::Server()
{
  m_running = false;
  m_storedData = "";
  
  m_memManager = new MemManager(true);
}

void Server::update()
{
  if (!m_running)
    return;

  tryAcceptConnection();

    // Handle Update info
    int maxSock = -1;
    fd_set readSockSet;
    FD_ZERO(&readSockSet);

    for (int i = 0; i < m_remoteSockets.size(); i++)
    {
      FD_SET(m_remoteSockets[i], &readSockSet);
      if (m_remoteSockets[i] > maxSock)
        maxSock = m_remoteSockets[i];
    }

    timeval timeout = {0, 0};
    int retval = select(maxSock, &readSockSet, NULL, NULL, &timeout);
    if (retval >= 0)
    {
      // iterate backwards in case we need to remove a disconnected client socket
      for (int32_t i = m_remoteSockets.size() - 1; i >= 0; i--)
      {
        if (FD_ISSET(m_remoteSockets[i], &readSockSet))
        {
          bool stillConnected = hostHandleUpdate(m_remoteSockets[i], &m_storedData,
                                                 &m_updateAcks[m_remoteSockets[i]]);
          if (!stillConnected)
          {
            closesocket(m_remoteSockets[i]);
            m_remoteSockets.erase(m_remoteSockets.begin() +
                                  i); // remove closed socket at position i
          }
        }
      }
    }

    // Send current gamestate

    for (int clientNum = 0; clientNum < m_remoteSockets.size(); clientNum++)
    {
		std::string data = "";
		std::vector<std::string> sharedThings = m_memManager->Keys();
		for (int i = 0; i < sharedThings.size(); i++)
		{
			std::string name = sharedThings[i];
			std::string newValue = m_memManager->hostGetEntryValue(name);

			size_t index = 0;
			while (true)
			{
				/* Locate the substring to replace. */
				std::string playerString = "$USER" + std::to_string(clientNum);
				index = newValue.find(playerString, index);
				if (index == std::string::npos)
					break;

				/* Make the replacement. */
				newValue.replace(index, playerString.length(), "$YOU_ARE" + std::to_string(clientNum));

				/* Advance index forward so the next iteration doesn't pick it up as well. */
				index += 3;
			}

			if (newValue == "")
				continue;

			data.append(name + ";");
			data.append(std::to_string(m_updateAcks[m_remoteSockets[clientNum]][name]) + ";");
			data.append(newValue + "/");
		}

		send(m_remoteSockets[clientNum], data.c_str(), data.length(), 0);
    }
}

bool Server::start(int portno)
{
  const int iReqWinsockVer = 2; // Minimum winsock version required

  WSADATA wsaData;

  if (WSAStartup(MAKEWORD(iReqWinsockVer, 0), &wsaData) == 0)
  {
    // Check if major version is at least iReqWinsockVer
    if (LOBYTE(wsaData.wVersion) >= iReqWinsockVer)
      return createConnection(portno);
    else
      return false;
  }
  else
  {
    return false;
  }
}

bool Server::createConnection(int portno)
{
  if (portno == 0)
	  portno = 14321;

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0)
    return false;

  // Binding info
  sockaddr_in sockAddr;
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(portno);
  sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(m_socket, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
      return false;

    if (listen(m_socket, SOMAXCONN) != 0)
      return false;

	m_running = true;
    m_port = portno;
    return true;
}


bool Server::hostHandleUpdate(int m_remoteSocket, std::string* storedData,
                                  std::map<std::string, int8_t>* updateAcks)
{
  const int RECV_SIZE = 1024;
  char recvData[RECV_SIZE + 1] = {'\0'};
  int bytesReceived = recv(m_remoteSocket, recvData, RECV_SIZE, 0);
  if (bytesReceived <= 0)
    return false;

  storedData->append(std::string(recvData));

  size_t delimPos;
  while ((delimPos = storedData->find("/")) != std::string::npos) // we have a whole entry
  {
    std::string entry = storedData->substr(0, delimPos);
    *storedData = storedData->substr(delimPos + 1);

    std::vector<std::string> parts = customSplit(entry, ";");
    std::string name = parts[0];
    int8_t ack = atoi(parts[1].c_str());
    std::string value = parts[2];

    (*updateAcks)[name] = ack;

	std::vector<int>::iterator it = std::find(m_remoteSockets.begin(), m_remoteSockets.end(), m_remoteSocket);
    int32_t id = std::distance(m_remoteSockets.begin(), it);

    m_memManager->hostHandleUpdate(name, id, value);
  }

  return true;
}

void Server::tryAcceptConnection()
{
  int maxSock = -1;
  fd_set inboundSockSet;
  FD_ZERO(&inboundSockSet);
  FD_SET(m_socket, &inboundSockSet); // listenSock will be ready-for-ready when it's time to accept()

  timeval timeout = {0, 0};
  int retval = select(m_socket + 1, &inboundSockSet, NULL, NULL, &timeout);
  if (retval > 0)
  {
    if (FD_ISSET(m_socket, &inboundSockSet))
    {
      sockaddr_in remoteAddr;
      int iRemoteAddrLen;

      iRemoteAddrLen = sizeof(remoteAddr);
      int remoteSocket = accept(m_socket, (sockaddr*)&remoteAddr, &iRemoteAddrLen);

      if (remoteSocket == INVALID_SOCKET)
        return;

      m_remoteSockets.push_back(remoteSocket);
      m_updateAcks.erase(remoteSocket);
    }
  }
}

bool Server::stop()
{
  for (int i = 0; i < m_remoteSockets.size(); i++)
  {
    closesocket(m_remoteSockets[i]);
  }
  m_remoteSockets.clear();
  closesocket(m_socket);
  m_running = false;
  return WSACleanup();
}


std::vector<std::string> Server::customSplit(std::string s, std::string delim)
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
