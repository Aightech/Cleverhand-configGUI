#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/**
 * @file      mainwindow.h
 * @brief     Header of
 * @date      Thu Jul  7 09:48:51 2022
 * @author    aightech
 * @copyright BSD-3-Clause
 * 
 * This module
 */

#include "qboxlayout.h"
#include <QMainWindow>
#include <clvHdMaster.hpp>
#include <QTreeWidgetItem>
#include <QSettings>
#include "qcustomplot.h"



#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip> // std::setprecision
#include <pthread.h>
#include <lsl_cpp.h>
#include <utility>
 #include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


using clk = std::chrono::system_clock;
using sec = std::chrono::duration<double>;

/**
 * @brief The MainWindow class
 * 
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief addBoard Add a new board to the list of boards.
     * 
     * @param id 
     */
    void addBoard(int id);

    /**
     * @brief streaming_loop Loop that continuously reads data from the board and send it to the LSL stream.
     * 
     * @param obj The MainWindow object.
     * @return void* 
     */
    static void *streaming_loop(void *obj);

    /**
     * @brief push_lsl Push data to the LSL stream.
     * 
     */
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
    void data_ploting();

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
    std::vector<float> m_mean_sample;
    double m_t=0;

    QHBoxLayout* m_debug_bar;
    bool m_debug_mode=false;

    int m_nb_bytes=2;

    QSettings* m_settings;

  std::chrono::time_point<std::chrono::system_clock> m_time;
  std::chrono::time_point<std::chrono::system_clock> m_start_time;
  sec m_duration;

  QCustomPlot* m_plot=nullptr;
  std::vector<std::vector<float>*> m_values;

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  QTimer m_plotTimer;


};
#endif // MAINWINDOW_H
