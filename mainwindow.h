#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSystemTrayIcon;
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

    void _Create_TrayIcon();
    void _SaveSettings();
    void _LoadSettings();

private slots:
    void onGetRandomAdvice();
    void onGetRandomAdviceWithSound();
    void onTogglePollServer(bool poll);
};

#endif // MAINWINDOW_H
