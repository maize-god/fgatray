#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "defaults.h"
#include "settings.h"

#include <QCloseEvent>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QAbstractButton>

struct ENUM_PAIR {
    int id;
    QString desc;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_fgAdvice = new FGAdvice(this);
    connect(m_fgAdvice, SIGNAL(got(int)), SLOT(onAdviceReceived(int)));

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, SIGNAL(timeout()), SLOT(onGetRandomAdvice()));

    _Create_TrayIcon();
    _InitControls();
    _LoadSettings();
    _ApplySettings(true);

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

void MainWindow::_InitControls()
{
    const ENUM_PAIR _intervals[] = {
        { 60, tr("Every minute") },
        { 120, tr("Every two minutes") },
        { 300, tr("Every five minutes") },
        { 600, tr("Every ten minutes") },
        { 900, tr("Every fifteen minutes") },
        { 1200, tr("Every twenty minutes") },
        { 1800, tr("Twice in a hour") },
        { 3600, tr("Every hour") },
        { 7200, tr("Every two hours") },
        { 0, QString() }
    };
    const ENUM_PAIR* ep = _intervals;
    while(ep->id != 0) {
        ui->cmbAutoUpdateInterval->addItem(ep->desc, QVariant(ep->id));
        ep++;
    }
}

void MainWindow::_SaveSettings()
{
    Settings& s = Settings::instance();

    s.context(); // General
    s.set("AutoUpdate", ui->cbAutoUpdate->isChecked());
    {
        int idx = ui->cmbAutoUpdateInterval->currentIndex();
        s.set("AutoUpdateInterval",
              (idx >= 0 ?
                   ui->cmbAutoUpdateInterval->itemData(idx).toInt() :
                   DEF_UPDATE_INTERVAL));
    }

    s.context("Proxy");
    s.set("Use", ui->cbUseProxy->isChecked());
    s.set("Host", ui->leProxyHost->text());
    s.set("Port", ui->leProxyPort->text());
    s.set("User", ui->leProxyUser->text());
    s.set("Password", ui->leProxyPassword->text());

    s.context();
}

void MainWindow::_LoadSettings()
{
    Settings& s = Settings::instance();

    {
        s.context(); // General
        bool autoUpdate = s.get("AutoUpdate", false);
        ui->cbAutoUpdate->setChecked(autoUpdate);
        m_trayMenuPollServerToggleAction->setChecked(autoUpdate);
        on_cbAutoUpdate_toggled(autoUpdate);
        int auint = s.get("AutoUpdateInterval", DEF_UPDATE_INTERVAL);
        int idx = 0;
        for(int i = 0; i < ui->cmbAutoUpdateInterval->count(); i++) {
            if(ui->cmbAutoUpdateInterval->itemData(i).toInt() == auint) {
                idx = i;
                break;
            }
        }
        ui->cmbAutoUpdateInterval->setCurrentIndex(idx);
    }

    {
        s.context("Proxy");
        bool useProxy = s.get("Use", false);
        ui->cbUseProxy->setChecked(useProxy);
        on_cbUseProxy_toggled(useProxy);
        ui->leProxyHost->setText(s.getString("Host"));
        ui->leProxyPort->setText(s.getString("Port"));
        ui->leProxyUser->setText(s.getString("User"));
        ui->leProxyPassword->setText(s.getString("Password"));
    }

    s.context();
}

void MainWindow::_ApplySettings(bool startup)
{
    if(ui->cbUseProxy->isChecked()) {
        m_fgAdvice->setProxy(
                    ui->leProxyHost->text(),
                    ui->leProxyPort->text().toInt(),
                    ui->leProxyUser->text(),
                    ui->leProxyPassword->text());
    } else {
        m_fgAdvice->clearProxy();
    }

    if(ui->cbAutoUpdate->isChecked()) {
        if(startup) {
            onGetRandomAdvice();
        }

        int interval = DEF_UPDATE_INTERVAL;
        if(ui->cmbAutoUpdateInterval->currentIndex() >= 0)
            interval = ui->cmbAutoUpdateInterval->itemData(
                        ui->cmbAutoUpdateInterval->currentIndex()).toInt();
        interval = 1000 * interval;
        if(m_updateTimer->interval() != interval) {
            m_updateTimer->stop();
            m_updateTimer->setInterval(interval);
        }

        if(!m_updateTimer->isActive())
            m_updateTimer->start();
    } else {
        if(m_updateTimer->isActive())
            m_updateTimer->stop();
    }
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
    ui->cbAutoUpdate->setChecked(poll);
    on_cbAutoUpdate_toggled(poll);
    _SaveSettings();
    _ApplySettings();
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

void MainWindow::on_cbAutoUpdate_toggled(bool checked)
{
    ui->cmbAutoUpdateInterval->setEnabled(checked);
}

void MainWindow::on_cbUseProxy_toggled(bool checked)
{
    ui->leProxyHost->setEnabled(checked);
    ui->leProxyPort->setEnabled(checked);
    ui->leProxyUser->setEnabled(checked);
    ui->leProxyPassword->setEnabled(checked);
}

void MainWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton btnId =
            ui->buttonBox->standardButton(button);

    switch(btnId) {
    case QDialogButtonBox::Close:
        m_canClose = true;
        close();
        break;
    case QDialogButtonBox::Save:
        _SaveSettings();
        _ApplySettings();
        break;
    case QDialogButtonBox::Reset:
        _LoadSettings();
        break;
    default:
        // shut the compiler up
        break;
    }
}
