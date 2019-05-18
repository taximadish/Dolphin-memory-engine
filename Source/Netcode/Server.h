#pragma once

#include <vector>

#include "../MemManager/MemManager.h"

class Server
{
public:
  Server();

  bool start(int portno = 0);
  void update();
  bool stop();
  bool m_running;
  int m_port;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::string m_storedData;
  std::map <int, std::map<std::string, int8_t>> m_updateAcks;
  bool hostHandleUpdate(int m_remoteSocket, std::string* storedData,
                        std::map<std::string, int8_t>* updateAcks);
  void tryAcceptConnection();
  bool createConnection(int portno);
  MemManager* m_memManager;
  int m_socket;
  std::vector<int> m_remoteSockets;
};
