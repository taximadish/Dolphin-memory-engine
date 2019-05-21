#include "MainWindow.h"

#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QString>
#include <QVBoxLayout>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <sstream>
#include <string>

#include "../DolphinProcess/DolphinAccessor.h"
#include "../MemoryWatch/MemWatchEntry.h"
#include "Settings/DlgSettings.h"
#include "Settings/SConfig.h"

MainWindow::MainWindow()
{
  initialiseWidgets();
  makeLayouts();
  DolphinComm::DolphinAccessor::init();
  updateDolphinHookingStatus();

  m_updateTimer = new QTimer(this);
  m_updateTimer->setInterval(16); // 60 per second
  m_updateTimer->start();
  connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer);
  m_server = new Server();
  m_client = new Client();
  updateConnectStatus();
  updateServerStatus();
}

MainWindow::~MainWindow()
{
  DolphinComm::DolphinAccessor::free();
}

void MainWindow::initialiseWidgets()
{
  m_btnAttempHook = new QPushButton(tr("Hook"));
  m_btnUnhook = new QPushButton(tr("Unhook"));
  connect(m_btnAttempHook, &QPushButton::clicked, this, &MainWindow::onHookAttempt);
  connect(m_btnUnhook, &QPushButton::clicked, this, &MainWindow::onUnhook);
  m_txtProcessNum = new QSpinBox();
  m_txtProcessNum->setValue(1);
  m_txtProcessNum->setRange(1, 9);
  m_txtProcessNum->setMaximumWidth(30);

  m_btnStartServer = new QPushButton(tr("Start Server"));
  connect(m_btnStartServer, &QPushButton::clicked, this, &MainWindow::onStartServerAttempt);

  m_txtAddress = new QLineEdit();
  m_txtAddress->setPlaceholderText("IP Address (Default localhost)");
  m_txtAddress->setText("localhost");
  m_txtPort = new QLineEdit();
  m_txtPort->setPlaceholderText("Port (Default 14321)");
  m_txtPort->setText("14321");
  m_btnConnect = new QPushButton(tr("Connect"));
  connect(m_btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectAttempt);

  m_lblDolphinStatus = new QLabel("");
  m_lblDolphinStatus->setAlignment(Qt::AlignHCenter);

  m_lblConnectStatus = new QLabel("");
  m_lblConnectStatus->setAlignment(Qt::AlignRight);

  m_lblServerStatus = new QLabel("");
  m_lblServerStatus->setAlignment(Qt::AlignLeft);
}

void MainWindow::makeLayouts()
{
  QHBoxLayout* dolphinHookButtons_layout = new QHBoxLayout();
  dolphinHookButtons_layout->addWidget(m_btnAttempHook);
  dolphinHookButtons_layout->addWidget(m_btnUnhook);
  dolphinHookButtons_layout->addWidget(m_txtProcessNum);

  QHBoxLayout* socketSettings_layout = new QHBoxLayout();
  socketSettings_layout->addWidget(m_btnStartServer);
  socketSettings_layout->addWidget(m_txtAddress);
  socketSettings_layout->addWidget(m_txtPort);
  socketSettings_layout->addWidget(m_btnConnect);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_lblDolphinStatus);
  mainLayout->addLayout(dolphinHookButtons_layout);
  mainLayout->addSpacing(5);
  mainLayout->addWidget(m_lblConnectStatus);
  mainLayout->addLayout(socketSettings_layout);
  mainLayout->addWidget(m_lblServerStatus);

  QWidget* mainWidget = new QWidget();
  mainWidget->setLayout(mainLayout);
  setCentralWidget(mainWidget);
}

void MainWindow::onUpdateTimer()
{
  updateConnectStatus();
  updateServerStatus();

  static bool serverSwitch = true;
  if (serverSwitch)
  {
    m_server->update(); // Update server every second call (reduces latency)
  }
  serverSwitch = !serverSwitch;
  if (DolphinComm::DolphinAccessor::getStatus() ==
      DolphinComm::DolphinAccessor::DolphinStatus::hooked)
  {
    m_client->update();
  }
}

void MainWindow::onConnectAttempt()
{
  if (m_client->m_running)
    m_client->stop();
  else
    m_client->start(m_txtAddress->text().toStdString(),
                    atoi(m_txtPort->text().toStdString().c_str()));
}

void MainWindow::onStartServerAttempt()
{
  if (m_server->m_running)
    m_server->stop();
  else
    m_server->start(atoi(qPrintable(m_txtPort->text())));
}

void MainWindow::updateConnectStatus()
{
  const char* text;
  switch (m_client->m_running)
  {
  case true:
    text = "Connected!";
    break;
  default:
    text = "Not Connected";
    break;
  }
  m_lblConnectStatus->setText(tr(text));
}

void MainWindow::updateServerStatus()
{
  std::string text;
  switch (m_server->m_running)
  {
  case true:
    text = "Server running on Port ";
    text.append(std::to_string(m_server->m_port));
    break;
  default:
    text = "Server not Running";
    break;
  }
  m_lblServerStatus->setText(tr(text.c_str()));
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
    m_btnAttempHook->hide();
    m_btnUnhook->show();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::notRunning:
  {
    m_lblDolphinStatus->setText(tr("Cannot hook to Dolphin, the process is not running"));
    m_btnAttempHook->show();
    m_btnUnhook->hide();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::noEmu:
  {
    m_lblDolphinStatus->setText(
        tr("Cannot hook to Dolphin, the process is running, but no emulation has been started"));
    m_btnAttempHook->show();
    m_btnUnhook->hide();
    break;
  }
  case DolphinComm::DolphinAccessor::DolphinStatus::unHooked:
  {
    m_lblDolphinStatus->setText(tr("Unhooked, press \"Hook\" to hook to Dolphin again"));
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
}

void MainWindow::onUnhook()
{
  DolphinComm::DolphinAccessor::unHook();
  updateDolphinHookingStatus();
}