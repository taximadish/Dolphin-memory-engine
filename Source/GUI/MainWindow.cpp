#include "MainWindow.h"

#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QString>
#include <QVBoxLayout>
#include <string>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <sstream>

#include "../DolphinProcess/DolphinAccessor.h"
#include "../MemoryWatch/MemWatchEntry.h"
#include "Settings/DlgSettings.h"
#include "Settings/SConfig.h"

MainWindow::MainWindow()
{
  initialiseWidgets();
  makeLayouts();
  makeMenus();
  DolphinComm::DolphinAccessor::init();
  updateDolphinHookingStatus();

  m_memManager = new MemManager();
  m_updateTimer = new QTimer(this);
  m_updateTimer->setInterval(100);
  m_updateTimer->start();
  connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer);
  m_connectState = ConnectState::NOT_CONNECTED;
  m_isHost = false;
  updateConnectStatus();
}

MainWindow::~MainWindow()
{
  delete m_watcher;
  DolphinComm::DolphinAccessor::free();
}

void MainWindow::makeMenus()
{
  m_actOpenWatchList = new QAction(tr("&Open..."), this);
  m_actSaveWatchList = new QAction(tr("&Save"), this);
  m_actSaveAsWatchList = new QAction(tr("&Save as..."), this);
  m_actClearWatchList = new QAction(tr("&Clear the watch list"), this);
  m_actImportFromCT = new QAction(tr("&Import from Cheat Engine's CT file..."), this);
  m_actExportAsCSV = new QAction(tr("&Export as CSV..."), this);

  m_actOpenWatchList->setShortcut(Qt::Modifier::CTRL + Qt::Key::Key_O);
  m_actSaveWatchList->setShortcut(Qt::Modifier::CTRL + Qt::Key::Key_S);
  m_actSaveAsWatchList->setShortcut(Qt::Modifier::CTRL + Qt::Modifier::SHIFT + Qt::Key::Key_S);
  m_actImportFromCT->setShortcut(Qt::Modifier::CTRL + Qt::Key::Key_I);

  m_actSettings = new QAction(tr("&Settings"), this);

  m_actViewScanner = new QAction(tr("&Scanner"), this);
  m_actViewScanner->setCheckable(true);
  m_actViewScanner->setChecked(true);

  m_actQuit = new QAction(tr("&Quit"), this);
  m_actAbout = new QAction(tr("&About"), this);
  connect(m_actOpenWatchList, &QAction::triggered, this, &MainWindow::onOpenWatchFile);
  connect(m_actSaveWatchList, &QAction::triggered, this, &MainWindow::onSaveWatchFile);
  connect(m_actSaveAsWatchList, &QAction::triggered, this, &MainWindow::onSaveAsWatchFile);
  connect(m_actClearWatchList, &QAction::triggered, this, &MainWindow::onClearWatchList);
  connect(m_actImportFromCT, &QAction::triggered, this, &MainWindow::onImportFromCT);
  connect(m_actExportAsCSV, &QAction::triggered, this, &MainWindow::onExportAsCSV);

  connect(m_actSettings, &QAction::triggered, this, &MainWindow::onOpenSettings);

  connect(m_actQuit, &QAction::triggered, this, &MainWindow::onQuit);
  connect(m_actAbout, &QAction::triggered, this, &MainWindow::onAbout);

  m_menuFile = menuBar()->addMenu(tr("&File"));
  m_menuFile->addAction(m_actOpenWatchList);
  m_menuFile->addAction(m_actSaveWatchList);
  m_menuFile->addAction(m_actSaveAsWatchList);
  m_menuFile->addAction(m_actClearWatchList);
  m_menuFile->addAction(m_actImportFromCT);
  m_menuFile->addAction(m_actExportAsCSV);
  m_menuFile->addAction(m_actQuit);

  m_menuEdit = menuBar()->addMenu(tr("&Edit"));
  m_menuEdit->addAction(m_actSettings);

  m_menuView = menuBar()->addMenu(tr("&View"));
  m_menuView->addAction(m_actViewScanner);

  m_menuHelp = menuBar()->addMenu(tr("&Help"));
  m_menuHelp->addAction(m_actAbout);
}

void MainWindow::initialiseWidgets()
{

  m_watcher = new MemWatchWidget(this);

  m_btnAttempHook = new QPushButton(tr("Hook"));
  m_btnUnhook = new QPushButton(tr("Unhook"));
  connect(m_btnAttempHook, &QPushButton::clicked, this, &MainWindow::onHookAttempt);
  connect(m_btnUnhook, &QPushButton::clicked, this, &MainWindow::onUnhook);
  m_txtProcessNum = new QLineEdit();
  m_txtProcessNum->setPlaceholderText("Process Number (Default 1)");
  m_txtProcessNum->setText("1");

  m_chkHost = new QCheckBox();
  m_chkHost->setText("Host");
  connect(m_chkHost, &QCheckBox::stateChanged, this, &MainWindow::onChkHostChanged);
  m_txtAddress = new QLineEdit();
  m_txtAddress->setPlaceholderText("IP Address (Default localhost)");
  m_txtAddress->setText("localhost");
  m_txtPort = new QLineEdit();
  m_txtPort->setPlaceholderText("Port (Default 1400)");
  m_btnConnect = new QPushButton(tr("Connect"));
  connect(m_btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectAttempt);

  m_lblDolphinStatus = new QLabel("");
  m_lblDolphinStatus->setAlignment(Qt::AlignHCenter);

  m_lblMem2Status = new QLabel("");
  m_lblMem2Status->setAlignment(Qt::AlignHCenter);

  m_lblConnectStatus = new QLabel("");
  m_lblConnectStatus->setAlignment(Qt::AlignRight);
}

void MainWindow::makeLayouts()
{
  QHBoxLayout* dolphinHookButtons_layout = new QHBoxLayout();
  dolphinHookButtons_layout->addWidget(m_txtProcessNum);
  dolphinHookButtons_layout->addWidget(m_btnAttempHook);
  dolphinHookButtons_layout->addWidget(m_btnUnhook);

  QHBoxLayout* socketSettings_layout = new QHBoxLayout();
  socketSettings_layout->addWidget(m_chkHost);
  socketSettings_layout->addWidget(m_txtAddress);
  socketSettings_layout->addWidget(m_txtPort);
  socketSettings_layout->addWidget(m_btnConnect);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_lblDolphinStatus);
  mainLayout->addLayout(dolphinHookButtons_layout);
  mainLayout->addWidget(m_lblMem2Status);
  mainLayout->addSpacing(20);
  mainLayout->addWidget(m_lblConnectStatus);
  mainLayout->addLayout(socketSettings_layout);
  mainLayout->addSpacing(20);
  mainLayout->addWidget(m_watcher);

  QWidget* mainWidget = new QWidget();
  mainWidget->setLayout(mainLayout);
  setCentralWidget(mainWidget);
}

void MainWindow::addWatchRequested(u32 address, Common::MemType type, size_t length,
                                   bool isUnsigned, Common::MemBase base)
{
  MemWatchEntry* newEntry =
      new MemWatchEntry(tr("No label"), address, type, base, isUnsigned, length);
  m_watcher->addWatchEntry(newEntry);
}

void MainWindow::updateMem2Status()
{
  if (DolphinComm::DolphinAccessor::isMEM2Present())
    m_lblMem2Status->setText(tr("The extended Wii-only memory is present"));
  else
    m_lblMem2Status->setText(tr("The extended Wii-only memory is absent"));
}



MainWindow::ConnectState MainWindow::initConnection()
{
  const int iReqWinsockVer = 2; // Minimum winsock version required

  WSADATA wsaData;

  if (WSAStartup(MAKEWORD(iReqWinsockVer, 0), &wsaData) == 0)
  {
    // Check if major version is at least iReqWinsockVer
    if (LOBYTE(wsaData.wVersion) >= iReqWinsockVer)
      return createConnection();
    else
      return ConnectState::FAILED;
  }
  else
  {
    return ConnectState::FAILED;
  }
}

bool MainWindow::teardownConnection()
{
  closesocket(m_socket);
  return WSACleanup();
}

void MainWindow::onUpdateTimer()
{
  if (m_connectState != CONNECTED)
    return;

  if (DolphinComm::DolphinAccessor::getStatus() !=
      DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  char data[40];
  if (m_isHost)
  {
    const char* format = "%s;%s/%s;%s/";
    sprintf_s(data, format, "Coins", m_memManager->readEntryValue("Coins").c_str(), "Star Points",
              m_memManager->readEntryValue("Star Points").c_str());
    send(m_remoteSocket, data, 40, 0);
  }
  else // Client
  {
    int bytesReceived = recv(m_socket, data, 40, 0);

    if (bytesReceived > 0)
    {
      m_lblConnectStatus->setText(data);

	  std::vector<std::string> entries = customSplit(data, "/");

	  for (int i = 0; i < entries.size(); i++)
	  {
            std::string entry = entries[i];
            std::vector<std::string> parts = customSplit(entry, ";");

            m_memManager->setEntryValue(parts[0], parts[1]);
	  }
    }
	else // connection closed
	{
          teardownConnection();
          m_connectState = CLOSED;
	}
  }
}

std::vector<std::string> MainWindow::customSplit(std::string s, std::string delim)
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

void MainWindow::onConnectAttempt()
{
  teardownConnection();
  m_connectState = initConnection();
  updateConnectStatus();
}

void MainWindow::onChkHostChanged()
{
  if (m_connectState != CONNECTED)
  {
    m_isHost = m_chkHost->isChecked();
    m_lblDolphinStatus->setText(m_isHost ? "Host" : "Client");
  }
}

MainWindow::ConnectState MainWindow::createConnection()
{
  const char* address = "127.0.0.1";
  int portno = 1400;

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0)
    return ConnectState::FAILED;
  
  // Binding info
  sockaddr_in sockAddr;
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(portno);
  inet_pton(AF_INET, address, &sockAddr.sin_addr.S_un.S_addr);

  if (m_isHost)
  {
    if (::bind(m_socket, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0)
      return ConnectState::FAILED;

	if (listen(m_socket, SOMAXCONN) != 0)
      return ConnectState::FAILED;
    
	// Remote = Client
	sockaddr_in remoteAddr;
    int iRemoteAddrLen;

    iRemoteAddrLen = sizeof(remoteAddr);
    m_remoteSocket = accept(m_socket, (sockaddr*)&remoteAddr, &iRemoteAddrLen);
    if (m_remoteSocket == INVALID_SOCKET)
      return ConnectState::FAILED;

	return CONNECTED;
  }
  else // Client
  {
    if (::connect(m_socket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
      return ConnectState::FAILED;

	return CONNECTED;
  }
}

void MainWindow::updateConnectStatus()
{
  const char* text;
  switch (m_connectState)
  {
  case NOT_CONNECTED:
    text = "Not Connected";
    break;
  case CONNECTED:
    text = "Connected!";
    break;
  case FAILED:
    text = "Connection Failed";
    break;
  default:
    text = "Unknown connection state?!";
    break;
  }
  m_lblConnectStatus->setText(tr(text));
}



void MainWindow::updateDolphinHookingStatus()
{
  switch (DolphinComm::DolphinAccessor::getStatus())
  {
  case DolphinComm::DolphinAccessor::DolphinStatus::hooked:
  {
    m_lblDolphinStatus->setText(
        tr("Hooked successfully to Dolphin, current start address: ") +
        QString::number(DolphinComm::DolphinAccessor::getEmuRAMAddressStart(), 16).toUpper());
    m_watcher->setEnabled(true);
    m_btnAttempHook->hide();
    m_btnUnhook->show();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::notRunning:
  {
    m_lblDolphinStatus->setText(tr("Cannot hook to Dolphin, the process is not running"));
    m_watcher->setDisabled(true);
    m_btnAttempHook->show();
    m_btnUnhook->hide();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::noEmu:
  {
    m_lblDolphinStatus->setText(
        tr("Cannot hook to Dolphin, the process is running, but no emulation has been started"));
    m_watcher->setDisabled(true);
    m_btnAttempHook->show();
    m_btnUnhook->hide();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::unHooked:
  {
    m_lblDolphinStatus->setText(tr("Unhooked, press \"Hook\" to hook to Dolphin again"));
    m_watcher->setDisabled(true);
    m_btnAttempHook->show();
    m_btnUnhook->hide();
    break;
  }
  }
}

void MainWindow::onHookAttempt()
{
  u16 num = m_txtProcessNum->text().toInt();
  DolphinComm::DolphinAccessor::hook(num);
  updateDolphinHookingStatus();
  if (DolphinComm::DolphinAccessor::getStatus() ==
      DolphinComm::DolphinAccessor::DolphinStatus::hooked)
  {
    m_watcher->getUpdateTimer()->start(SConfig::getInstance().getWatcherUpdateTimerMs());
    m_watcher->getFreezeTimer()->start(SConfig::getInstance().getFreezeTimerMs());
    updateMem2Status();
  }
}

void MainWindow::onUnhook()
{
  m_watcher->getUpdateTimer()->stop();
  m_watcher->getFreezeTimer()->stop();
  m_lblMem2Status->setText(QString(""));
  DolphinComm::DolphinAccessor::unHook();
  updateDolphinHookingStatus();
}

void MainWindow::onOpenWatchFile()
{
  if (m_watcher->warnIfUnsavedChanges())
    m_watcher->openWatchFile();
}

void MainWindow::onSaveWatchFile()
{
  m_watcher->saveWatchFile();
}

void MainWindow::onSaveAsWatchFile()
{
  m_watcher->saveAsWatchFile();
}

void MainWindow::onClearWatchList()
{
  m_watcher->clearWatchList();
}

void MainWindow::onImportFromCT()
{
  m_watcher->importFromCTFile();
}

void MainWindow::onExportAsCSV()
{
  m_watcher->exportWatchListAsCSV();
}

void MainWindow::onOpenSettings()
{
  DlgSettings* dlg = new DlgSettings(this);
  int dlgResult = dlg->exec();
  delete dlg;
  if (dlgResult == QDialog::Accepted)
  {
    m_watcher->getUpdateTimer()->stop();
    m_watcher->getFreezeTimer()->stop();
    if (DolphinComm::DolphinAccessor::getStatus() ==
        DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    {
      m_watcher->getUpdateTimer()->start(SConfig::getInstance().getWatcherUpdateTimerMs());
      m_watcher->getFreezeTimer()->start(SConfig::getInstance().getFreezeTimerMs());
    }
  }
}

void MainWindow::onAbout()
{
  QString title = tr("About Dolphin memory engine");
  QString text =
      "Beta version 0.5\n\n" +
      tr("A RAM search made to facilitate research and reverse engineering of GameCube and Wii "
         "games using the Dolphin emulator.\n\nThis program is licensed under the MIT license. You "
         "should have received a copy of the MIT license along with this program");
  QMessageBox::about(this, title, text);
}

void MainWindow::onQuit()
{
  close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (m_watcher->warnIfUnsavedChanges())
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}
