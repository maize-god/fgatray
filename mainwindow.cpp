#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "defaults.h"

#include <QCloseEvent>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _Create_TrayIcon();
    _LoadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO CHANGE THIS BEHAVIOUR
    delete m_trayIcon;
    e->accept();
}

void MainWindow::_Create_TrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);

    QIcon icon(":/i/main.png");
    m_trayIcon->setIcon(icon);

    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(tr("&Get Advice Now (Left Click)"), this,
                          SLOT(onGetRandomAdvice()));
    m_trayMenu->addAction(tr("Get &Audio Advice Now (Middle Click)"), this,
                          SLOT(onGetRandomAdviceWithSound()));
    m_trayMenuPollServerToggleAction = m_trayMenu->addAction(
                tr("Get Advices A&utomatically"));
    connect(m_trayMenuPollServerToggleAction, SIGNAL(toggled(bool)),
            SLOT(onTogglePollServer(bool)));
    m_trayMenuPollServerToggleAction->setCheckable(true);
    m_trayMenu->addAction(tr("Change &Settings (Double Left Click)"), this,
                          SLOT(show()));
    m_trayMenu->addAction(tr("E&xit"), this, SLOT(close()));
    m_trayIcon->setContextMenu(m_trayMenu);

    m_trayIcon->show();
}

void MainWindow::_SaveSettings()
{
}

void MainWindow::_LoadSettings()
{
}

void MainWindow::onGetRandomAdvice()
{
}

void MainWindow::onGetRandomAdviceWithSound()
{
}

void MainWindow::onTogglePollServer(bool poll)
{
}
