#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <climits>
#include <signal.h>

extern "C"
{
  #include "../../src/server_lib.h"
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_config();
}

MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::read_pid_server()
{
    FILE *fp;
    int ret = 0;

    fp = fopen(PID_FILE_PATH,"r");

    if (fp == NULL)
        return false;

    ret = fscanf(fp,"%ld",(long *) &(this->pid_server));
    fclose(fp);

    if (ret == 0)
        return false;
    return true;
}
bool MainWindow::load_config()
{
    char root_dir[PATH_MAX];
    long port = 0;
    long speed_limmit = 0;

    read_config_file(root_dir, &port, &speed_limmit);

    ui->txt_root_dir->setText(root_dir);
    ui->txt_port->setText(QString::number(port));
    ui->txt_speed_limit->setText(QString::number(speed_limmit));
    ui->cb_speed_limit->setCurrentIndex(0);
    return true;
}

bool MainWindow::check_param()
{
    QString msg_error;

    if (ui->txt_root_dir->text().size() < 1
        || !QDir(ui->txt_root_dir->text()).exists())
    {
        msg_error = "Diretorio raiz invalido!";
        goto on_error;
    }

    if (ui->txt_port->text().size() < 1 ||
        ui->txt_port->text().toInt() < 0
        || ui->txt_port->text().toInt() > 65535)
    {
        msg_error = "Valor de porta invalido!";
        goto on_error;
    }
    else if (ui->txt_port->text().toInt() < 1024)
    {
        QMessageBox::information(this,"AVISO","Manipular uma porta com esse valor requer permissoes de super usuario, assim eh possivel que o deamon nao responda a esta configuracao!");
    }
    switch (ui->cb_speed_limit->currentIndex())
    {
    case 0:
        speed_rate = BYTE;
        break;
    case 1:
        speed_rate = KILOBYTE;
        break;
    case 2:
        speed_rate = MEGABYTE;
        break;
    case 3:
        speed_rate = GIGABYTE;
        break;
    default:
        speed_rate = BYTE;
        break;
    }

    if ((ui->txt_speed_limit->text().toLong() * speed_rate) <= 0
            || (ui->txt_speed_limit->text().toLong() * speed_rate) > LONG_MAX )
    {
        msg_error = "Valor de limite de velocidade invalido!";
        goto on_error;
    }

    return true;
on_error:
    QMessageBox::information(this,"ERRROR",msg_error);
    return false;

}

void MainWindow::on_bt_apply_clicked()
{
    QString msg_error;

    if (this->check_param() == false)
        return;

    if (this->read_pid_server() == false)
    {
        msg_error = "Erro ao ler pid do servidor!";
        goto on_error;
    }
    if (write_config_file(ui->txt_root_dir->text().toStdString().c_str(),
                         ui->txt_port->text().toLong(),
                         ui->txt_speed_limit->text().toLong()))
    {
        msg_error = "Erro ao criar arquivo de configuracao!";
        goto on_error;
    }

    if (kill(this->pid_server, SIGHUP) != 0)
    {
        msg_error = "Erro sinal nao pode ser enviado!";
        goto on_error;
    }

    QMessageBox::information(this,"Sucesso", "Configuracao aplicada com sucesso!");
    return;

on_error:
    QMessageBox::information(this,"ERRROR",msg_error);
    return;
}

void MainWindow::on_bt_exit_clicked()
{
    this->close();
}

void MainWindow::on_bt_cancel_clicked()
{
    ui->txt_root_dir->setText("");
    ui->txt_port->setText("");
    ui->txt_speed_limit->setText("");
}
