#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMediaPlayer>

#include "fgadvice.h"

class QMenu;
class QAction;
class QTimer;
class QAbstractButton;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);
    
private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *m_trayIcon;
    QMenu* m_trayMenu;
    QAction* m_trayMenuPollServerToggleAction;

    QMediaPlayer m_mediaPlayer;

    FGAdvice* m_fgAdvice;

    QTimer* m_updateTimer;

    bool m_autoUpdate;
    int m_updateIntervalMillisec;
    bool m_canClose;

    void _Create_TrayIcon();
    void _InitControls();
    void _SaveSettings();
    void _LoadSettings();
    void _ApplySettings(bool startup = false);

private slots:
    void onGetRandomAdvice();
    void onGetRandomAdviceWithSound();
    void onTogglePollServer(bool poll);
    void onTrayActivate(QSystemTrayIcon::ActivationReason reason);
    void onExit();
    void onShow();
    void onAdviceReceived(int state);
    void on_cbAutoUpdate_toggled(bool checked);
    void on_cbUseProxy_toggled(bool checked);
    void on_buttonBox_clicked(QAbstractButton *button);
};

#endif // MAINWINDOW_H
