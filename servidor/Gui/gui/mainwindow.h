#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define BYTE 1
#define KILOBYTE 1024 * BYTE
#define MEGABYTE 1024 * KILOBYTE
#define GIGABYTE 1024 * MEGABYTE
#define CONFIG_FILE_PATH "/tmp/cacique.conf"
#define PID_FILE_PATH "/tmp/cacique.pid"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool read_pid_server();
    bool load_config();
    bool write_config_file();
    bool check_param();

private slots:
    void on_bt_apply_clicked();

    void on_bt_exit_clicked();

    void on_bt_cancel_clicked();

private:
    Ui::MainWindow *ui;
    pid_t pid_server;
    long speed_rate;
};

#endif // MAINWINDOW_H
