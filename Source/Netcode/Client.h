#pragma once

#include <vector>

#include "../MemManager/MemManager.h"

class Client
{
public:
  Client();

  bool start(std::string address, int portno = 0);
  void update();
  bool stop();
  bool m_running;

private:
  std::vector<std::string> customSplit(std::string s, std::string delim);
  std::map<std::string, std::string> m_pastValues;
  std::string m_storedData;
  std::map<std::string, int8_t> m_updateAcks;
  bool createConnection(std::string address, int portno);
  MemManager* m_memManager;
  int m_socket;
  std::vector<int> m_remoteSockets;
};
