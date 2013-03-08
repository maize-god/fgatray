#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

class QMenu;
class QAction;

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

    bool m_autoUpdate;
    int m_updateIntervalMillisec;
    bool m_canClose;

    void _Create_TrayIcon();
    void _SaveSettings();
    void _LoadSettings();

private slots:
    void onGetRandomAdvice();
    void onGetRandomAdviceWithSound();
    void onTogglePollServer(bool poll);
    void onTrayActivate(QSystemTrayIcon::ActivationReason reason);
    void onExit();
    void onShow();
};

#endif // MAINWINDOW_H