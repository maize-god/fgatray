#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "defaults.h"

#include <QCloseEvent>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_fgAdvice = new FGAdvice(this);
    connect(m_fgAdvice, SIGNAL(got(int)), SLOT(onAdviceReceived(int)));

    _Create_TrayIcon();
    _LoadSettings();

    m_canClose = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    _SaveSettings();

    if(m_canClose) {
        delete m_trayIcon;
        e->accept();
    } else {
        e->ignore();
        hide();
    }
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
                          SLOT(onShow()));
    m_trayMenu->addAction(tr("E&xit"), this, SLOT(onExit()));
    m_trayIcon->setContextMenu(m_trayMenu);

    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(onTrayActivate(QSystemTrayIcon::ActivationReason)));

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
    m_fgAdvice->get();
}

void MainWindow::onGetRandomAdviceWithSound()
{
    m_fgAdvice->get(true);
}

void MainWindow::onTogglePollServer(bool poll)
{
}

void MainWindow::onTrayActivate(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Trigger:
        onGetRandomAdvice();
        break;
    case QSystemTrayIcon::MiddleClick:
        onGetRandomAdviceWithSound();
        break;
    case QSystemTrayIcon::DoubleClick:
        onShow();
        break;
    default:
        break;
    }
}

void MainWindow::onExit()
{
    m_canClose = true;
    close();
}

void MainWindow::onShow()
{
    if(isVisible()) {
        hide();
    } else {
        show();
    }
}

void MainWindow::onAdviceReceived(int state)
{
    switch(state) {
    case FGAdvice::ReplySound:
        if(m_mediaPlayer.state() == QMediaPlayer::StoppedState) {
            m_mediaPlayer.setMedia(
                        QMediaContent(
                            QUrl::fromLocalFile(
                                m_fgAdvice->soundFilePath())));
            m_mediaPlayer.play();
        }
        // fall through
    case FGAdvice::ReplyText:
        m_trayIcon->showMessage(
                    tr("Fuckin' Great Advice"), m_fgAdvice->text(),
                    QSystemTrayIcon::Information);
        break;
    case FGAdvice::ReplyError:
        m_trayIcon->showMessage(
                    tr("ERROR"), m_fgAdvice->errorText(),
                    QSystemTrayIcon::Critical);
    }
}
