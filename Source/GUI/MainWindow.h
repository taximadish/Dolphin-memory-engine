#pragma once

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>

#include "../Common/CommonTypes.h"
#include "../Common/MemoryCommon.h"
#include "MemScanner/MemScanWidget.h"
#include "MemViewer/MemViewerWidget.h"
#include "MemWatcher/MemWatchWidget.h"
#include "../MemManager/MemManager.h"

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

  void closeEvent(QCloseEvent* event) override;
  void addWatchRequested(u32 address, Common::MemType type, size_t length, bool isUnsigned,
                         Common::MemBase base);
  void updateDolphinHookingStatus();
  void onHookAttempt();
  void onUnhook();
  void updateMem2Status();

  void updateConnectStatus();
  void onConnectAttempt();
  ConnectState initConnection();
  bool teardownConnection();
  ConnectState createConnection();
  void onUpdateTimer();
  void onChkHostChanged();

  void onOpenWatchFile();
  void onSaveWatchFile();
  void onSaveAsWatchFile();
  void onClearWatchList();
  void onOpenSettings();
  void onImportFromCT();
  void onExportAsCSV();
  void onAbout();
  void onQuit();

private:
  void makeMenus();
  void initialiseWidgets();
  void makeLayouts();

  std::vector<std::string> customSplit(std::string input, std::string delim);

  ConnectState m_connectState;
  int m_socket;
  int m_remoteSocket;

  MemWatchWidget* m_watcher;

  QLabel* m_lblDolphinStatus;
  QPushButton* m_btnAttempHook;
  QLineEdit* m_txtProcessNum;
  QPushButton* m_btnUnhook;
  QLabel* m_lblMem2Status;



  MemManager* m_memManager;
  QTimer* m_updateTimer;
  QLabel* m_lblConnectStatus;
  bool m_isHost;
  QCheckBox* m_chkHost;
  QLineEdit* m_txtAddress;
  QLineEdit* m_txtPort;
  QPushButton* m_btnConnect;

  
  
  QMenu* m_menuFile;
  QMenu* m_menuEdit;
  QMenu* m_menuView;
  QMenu* m_menuHelp;
  QAction* m_actOpenWatchList;
  QAction* m_actSaveWatchList;
  QAction* m_actSaveAsWatchList;
  QAction* m_actClearWatchList;
  QAction* m_actImportFromCT;
  QAction* m_actExportAsCSV;
  QAction* m_actViewScanner;
  QAction* m_actSettings;
  QAction* m_actQuit;
  QAction* m_actAbout;
};
