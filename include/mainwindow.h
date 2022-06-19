#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qboxlayout.h"
#include <QMainWindow>
#include <clvHdMaster.hpp>
#include <QTreeWidgetItem>
#include <QSettings>



#include <pthread.h>
#include <lsl_cpp.h>
#include <utility>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addBoard(int id);

    static void *streaming_loop(void *obj);
    void push_lsl();

private slots:
    void connection();
    void update_indiv_param();
    void upload();
    void start_streaming();
    void stop_streaming();
    void debug();
    void write_reg();
    void read_reg();

private:
    Ui::MainWindow *ui;
    ClvHd::Master m_master;
    std::vector<QTreeWidgetItem*> m_boardItem;
    int m_nb_ch = 0;
    int m_nb_board;
    std::vector<std::pair<int,uint8_t>> m_active_channel;
    std::vector<std::pair<int,int>> m_active_channel_ind;
    bool m_is_streaming=false;
     bool m_active=true;
    pthread_t m_thread;
    lsl::stream_outlet *m_lsl_outlet;
    std::vector<float> m_lsl_sample;

    QHBoxLayout* m_debug_bar;
    bool m_debug_mode=false;

    int m_nb_bytes=2;

    QSettings* m_settings;




};
#endif // MAINWINDOW_H
