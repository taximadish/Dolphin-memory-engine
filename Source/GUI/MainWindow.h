#pragma once

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <qspinbox.h>

#include "../Common/CommonTypes.h"
#include "../Common/MemoryCommon.h"
#include "MemScanner/MemScanWidget.h"
#include "MemViewer/MemViewerWidget.h"
#include "MemWatcher/MemWatchWidget.h"
#include "../MemManager/MemManager.h"
#include "../Netcode/Server.h"
#include "../Netcode/Client.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

  enum ConnectState
  {
	  NOT_CONNECTED = 0,
	  CONNECTED,
	  CLOSED,
	  FAILED
  };
  void updateDolphinHookingStatus();
  void onHookAttempt();
  void onUnhook();
  void onMuteChanged();

  void updateServerStatus();
  void onStartServerAttempt();
  void updateConnectStatus();
  void onConnectAttempt();
  void onUpdateTimer();

private:
  void initialiseWidgets();
  void makeLayouts();

  QLabel* m_lblDolphinStatus;
  QPushButton* m_btnAttempHook;
  QSpinBox* m_txtProcessNum;
  QPushButton* m_btnUnhook;
  QPushButton* m_btnStartServer;
  Server* m_server;
  Client* m_client;

  bool m_muteInitialised;
  QCheckBox* m_chkMute;
  MemWatchEntry* m_unmutedWatch;
  std::string m_unMutedVal;

  QTimer* m_updateTimer;
  QLabel* m_lblConnectStatus;
  QLabel* m_lblServerStatus;
  QLineEdit* m_txtAddress;
  QLineEdit* m_txtPort;
  QPushButton* m_btnConnect;
};
