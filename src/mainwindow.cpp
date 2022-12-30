#include "mainwindow.h"
#include "../forms/ui_mainwindow.h"
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_master(1)
{
    m_settings = new QSettings(".clvHd", "conf");
    ui->setupUi(this);
    ui->treeWidget_main->setColumnCount(2);
    ui->treeWidget_main->setHeaderLabels(QStringList()
                                         << tr("Boards") << tr("Settings"));
    ui->treeWidget_main->setColumnWidth(0, 150);

    ui->comboBox_freq->addItems(QStringList() << "102400 Hz"
                                << "204800 Hz");
    ui->comboBox_freq->setCurrentIndex(1);
    ui->comboBox_res->addItems(QStringList() << "Low"
                               << "High");
    ui->comboBox_res->setCurrentIndex(1);
    ui->comboBox_R1->addItems(QStringList() << "2"
                              << "4");
    ui->comboBox_R2->addItems(QStringList() << "4"
                              << "5"
                              << "6"
                              << "8");
    ui->comboBox_R3->addItems(QStringList() << "4"
                              << "6"
                              << "8"
                              << "12"
                              << "16"
                              << "32"
                              << "64"
                              << "128");

    connect(ui->pushButton_connect, SIGNAL(released()), this,
            SLOT(connection()));
    connect(ui->pushButton_upload, SIGNAL(released()), this, SLOT(upload()));
    connect(ui->pushButton_startStream, SIGNAL(released()), this,
            SLOT(start_streaming()));
    connect(ui->pushButton_stopStream, SIGNAL(released()), this,
            SLOT(stop_streaming()));

    connect(ui->comboBox_freq, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_res, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_R1, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_R2, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_R3, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->checkBox_adc1, SIGNAL(stateChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->checkBox_adc2, SIGNAL(stateChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->checkBox_adc3, SIGNAL(stateChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_n1, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_p1, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_n2, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_p2, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_n3, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));
    connect(ui->comboBox_p3, SIGNAL(currentIndexChanged(int)), this,
            SLOT(update_indiv_param()));

    connect(ui->actionRead_write_reg, SIGNAL(triggered(bool)), this,
            SLOT(debug()));

    connect(ui->pushButton_read, SIGNAL(released()), this, SLOT(read_reg()));
    connect(ui->pushButton_write, SIGNAL(released()), this, SLOT(write_reg()));

    ui->pushButton_stopStream->setDisabled(true);
    ui->pushButton_startStream->setDisabled(true);
    ui->pushButton_upload->setDisabled(true);
    ui->comboBox_freq->setDisabled(true);
    ui->comboBox_res->setDisabled(true);
    ui->comboBox_R1->setDisabled(true);
    ui->comboBox_R2->setDisabled(true);
    ui->comboBox_R3->setDisabled(true);
    ui->checkBox_adc1->setDisabled(true);
    ui->checkBox_adc2->setDisabled(true);
    ui->checkBox_adc3->setDisabled(true);
    ui->comboBox_n1->setDisabled(true);
    ui->comboBox_p1->setDisabled(true);
    ui->comboBox_n2->setDisabled(true);
    ui->comboBox_p2->setDisabled(true);
    ui->comboBox_n3->setDisabled(true);
    ui->comboBox_p3->setDisabled(true);
    ui->spinBox_bytes->setDisabled(true);

    ui->label_debug->setVisible(false);
    ui->label_board_id->setVisible(false);
    ui->spinBox_board_id->setVisible(false);
    ui->label_regadd->setVisible(false);
    ui->lineEdit_read_reg->setVisible(false);
    ui->pushButton_read->setVisible(false);
    ui->label_value->setVisible(false);
    ui->lineEdit_value_reg->setVisible(false);
    ui->pushButton_write->setVisible(false);

    m_start_time = clk::now();
    m_time = clk::now();
    pthread_create(&m_thread, 0, &MainWindow::streaming_loop, this);

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&m_plotTimer, SIGNAL(timeout()), this, SLOT(data_ploting()));



}

MainWindow::~MainWindow()
{
    m_active = false;
    pthread_join(m_thread, 0);
    delete ui;
}

void
MainWindow::connection()
{
    ui->pushButton_connect->setDisabled(true);
    ui->lineEdit_pathdev->setDisabled(true);
    m_master.open_connection(Communication::Client::SERIAL,ui->lineEdit_pathdev->text().toStdString().c_str());//"192.168.127.253",5000);
               // ui->lineEdit_pathdev->text().toStdString().c_str());

    m_nb_board = m_master.setup();
    for(int i = 0; i < m_nb_board; i++) { this->addBoard(i + 1); }
    if(m_nb_board)
    {

        ui->pushButton_upload->setDisabled(false);
        ui->comboBox_freq->setDisabled(false);
        ui->comboBox_res->setDisabled(false);
        ui->comboBox_R1->setDisabled(false);
        ui->comboBox_R2->setDisabled(false);
        ui->comboBox_R3->setDisabled(false);
        ui->checkBox_adc1->setDisabled(false);
        ui->checkBox_adc2->setDisabled(false);
        ui->checkBox_adc3->setDisabled(false);
        ui->comboBox_n1->setDisabled(false);
        ui->comboBox_p1->setDisabled(false);
        ui->comboBox_n2->setDisabled(false);
        ui->comboBox_p2->setDisabled(false);
        ui->comboBox_n3->setDisabled(false);
        ui->comboBox_p3->setDisabled(false);
        ui->spinBox_bytes->setDisabled(false);
        ui->spinBox_board_id->setMinimum(1);
        m_nb_ch = m_nb_board * (3 + 3);
	
        std::cout << "[INFOS] " << m_nb_ch << " active channels" << std::endl;
        lsl::stream_info info_sample(
                    ui->lineEdit_lsl_name->text().toStdString(),
                    ui->lineEdit_lsl_type->text().toStdString(), m_nb_ch, 0,
                    lsl::cf_float32);
        m_lsl_outlet = new lsl::stream_outlet(info_sample);
        m_lsl_sample.resize(m_nb_ch);
        m_mean_sample.resize(m_nb_ch);
        ui->lineEdit_lsl_name->setDisabled(true);
        ui->lineEdit_lsl_type->setDisabled(true);
    }
    ui->spinBox_board_id->setMaximum(m_nb_board);
}

void
MainWindow::debug()
{
    if(m_debug_mode == false)
    {
        ui->label_debug->setVisible(true);
        ui->label_board_id->setVisible(true);
        ui->spinBox_board_id->setVisible(true);
        ui->label_regadd->setVisible(true);
        ui->lineEdit_read_reg->setVisible(true);
        ui->pushButton_read->setVisible(true);
        ui->label_value->setVisible(true);
        ui->lineEdit_value_reg->setVisible(true);
        ui->pushButton_write->setVisible(true);
        m_debug_mode = true;
    }
    else
    {
        ui->label_debug->setVisible(false);
        ui->label_board_id->setVisible(false);
        ui->spinBox_board_id->setVisible(false);
        ui->label_regadd->setVisible(false);
        ui->lineEdit_read_reg->setVisible(false);
        ui->pushButton_read->setVisible(false);
        ui->label_value->setVisible(false);
        ui->lineEdit_value_reg->setVisible(false);
        ui->pushButton_write->setVisible(false);
        m_debug_mode = false;
    }
}

void
MainWindow::read_reg()
{
    uint8_t val;
    int v = m_master.readReg(ui->spinBox_board_id->value() - 1,
                             ui->lineEdit_read_reg->text().toInt(nullptr, 16),
                             1, &val);
    ui->lineEdit_value_reg->setText(QString::number(val, 16));
}

void
MainWindow::write_reg()
{
    m_master.writeReg(ui->spinBox_board_id->value() - 1,
                      ui->lineEdit_read_reg->text().toInt(nullptr, 16),
                      ui->lineEdit_value_reg->text().toInt(nullptr, 16));
}

void
MainWindow::upload()
{
    for(int i = 0; i < m_boardItem.size(); i++)
    {
        int route_table[3][2] = {
            {qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(0)->child(0), 1))
             ->currentIndex(),
             qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(0)->child(1), 1))
             ->currentIndex()},
            {qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(1)->child(0), 1))
             ->currentIndex(),
             qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(1)->child(1), 1))
             ->currentIndex()},
            {qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(2)->child(0), 1))
             ->currentIndex(),
             qobject_cast<QComboBox *>(
             ui->treeWidget_main->itemWidget(
             m_boardItem[i]->child(7)->child(2)->child(1), 1))
             ->currentIndex()},
        };
        //enable ch1 and disaable ch2 and ch3 (0x36)
        bool chx_enable[3] = {
            (bool)m_boardItem[i]->child(7)->child(0)->checkState(1),
            (bool)m_boardItem[i]->child(7)->child(1)->checkState(1),
            (bool)m_boardItem[i]->child(7)->child(2)->checkState(1)};
        //no high resolution & Clock frequency for Channel 1 :204800 (0x08)
        bool chx_high_res[3] = {
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(2), 1))
            ->currentIndex(),
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(2), 1))
            ->currentIndex(),
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(2), 1))
            ->currentIndex()};
        bool chx_high_freq[3] = {
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(1), 1))
            ->currentIndex(),
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(1), 1))
            ->currentIndex(),
            (bool)qobject_cast<QComboBox *>(
            ui->treeWidget_main->itemWidget(m_boardItem[i]->child(1), 1))
            ->currentIndex()};
        int R1[3] = {qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(3), 1))
                     ->currentText()
                     .toInt(),
                     qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(3), 1))
                     ->currentText()
                     .toInt(),
                     qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(3), 1))
                     ->currentText()
                     .toInt()}; //R1 ch1:2 ch2:4 ch3:4 (0x1)
        int R2 = qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                                               m_boardItem[i]->child(4), 1))
                ->currentText()
                .toInt(); //R2 = 4 (0x01)
        int R3[3] = {qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(5), 1))
                     ->currentText()
                     .toInt(),
                     qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(5), 1))
                     ->currentText()
                     .toInt(),
                     qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                     m_boardItem[i]->child(5), 1))
                     ->currentText()
                     .toInt()}; //R3_ch1 = 4 (0x01)
        m_master.setupEMG(i, route_table, chx_enable, chx_high_res,
                          chx_high_freq, R1, R2, R3);
        for(int j = 0; j < 3; j++)
        {
            m_settings->setValue("Board" + QString::number(i) + "/ADC" +
                                 QString::number(j) + "_neg_in",
                                 route_table[j][0]);
            m_settings->setValue("Board" + QString::number(i) + "/ADC" +
                                 QString::number(j) + "_pos_in",
                                 route_table[j][1]);
            m_settings->setValue("Board" + QString::number(i) + "/ADC" +
                                 QString::number(j) + "_enable",
                                 chx_enable[j]);
            m_settings->setValue("Board" + QString::number(i) + "/ADC" +
                                 QString::number(j) + "_high_res",
                                 (int)chx_high_res[j]);
            m_settings->setValue("Board" + QString::number(i) + "/ADC" +
                                 QString::number(j) + "_high_freq",
                                 (int)chx_high_freq[j]);
            m_settings->setValue(
                        "Board" + QString::number(i) + "/ADC" + QString::number(j) +
                        "_R1",
                        qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                                                      m_boardItem[i]->child(3), 1))
                        ->currentIndex());
            m_settings->setValue(
                        "Board" + QString::number(i) + "/ADC" + QString::number(j) +
                        "_R3",
                        qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                                                      m_boardItem[i]->child(5), 1))
                        ->currentIndex());
        }
        m_settings->setValue(
                    "Board" + QString::number(i) + "/R2",
                    qobject_cast<QComboBox *>(
                        ui->treeWidget_main->itemWidget(m_boardItem[i]->child(4), 1))
                    ->currentIndex());
    }
    ui->pushButton_startStream->setDisabled(false);

    if(m_plot != nullptr)
    {
        ui->verticalLayout_graph->removeWidget(m_plot);
        delete m_plot;
    }

    m_plot = new QCustomPlot();
    int code[6][3]={{1,2,0},
                    {2,1,0},
                    {0,1,2},
                    {0,2,1},
                    {2,0,1},
                    {1,0,2}};
    for(int i = 0; i<3*m_boardItem.size();i++)
    {
        m_plot->addGraph();
        double val[3]={0.25, 0.5, 0.25*(2- abs((int)(2*i/m_boardItem.size())%2-1))};
        m_plot->graph(i)->setPen(QPen(QColor(255*val[code[i*2/m_boardItem.size()][0]], 255*val[code[i*2/m_boardItem.size()][1]], 255*val[code[i*2/m_boardItem.size()][2]])));
    }
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    m_plot->xAxis->setTicker(timeTicker);
    m_plot->axisRect()->setupFullAxesBox();

    //m_plot->xAxis->setTickLabels(false);
    m_plot->yAxis->setTickLabels(false);
    //m_plot->xAxis->grid()->setVisible(false);
    //m_plot->yAxis->grid()->setVisible(false);
    //m_plot->xAxis->setTicks(false);
    //m_plot->yAxis->setTicks(false);

    m_plot->xAxis->setRange(0, 10);
    m_plot->yAxis->setRange(0, 1000);

    m_values.push_back(new std::vector<float>(1000));
    m_plot->replot();
    ui->verticalLayout_graph->addWidget(m_plot);
    m_plot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

void
MainWindow::update_indiv_param()
{
    for(auto i : m_boardItem)
    {
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(1), 1))
                ->setCurrentIndex(ui->comboBox_freq->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(2), 1))
                ->setCurrentIndex(ui->comboBox_res->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(3), 1))
                ->setCurrentIndex(ui->comboBox_R1->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(4), 1))
                ->setCurrentIndex(ui->comboBox_R2->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(5), 1))
                ->setCurrentIndex(ui->comboBox_R3->currentIndex());

        i->child(6)->setText(1, ui->label_bandwidth->text());

        i->child(7)->child(0)->setCheckState(1,
                                             ui->checkBox_adc1->checkState());
        i->child(7)->child(1)->setCheckState(1,
                                             ui->checkBox_adc2->checkState());
        i->child(7)->child(2)->setCheckState(1,
                                             ui->checkBox_adc3->checkState());

        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(0)->child(0), 1))
                ->setCurrentIndex(ui->comboBox_n1->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(0)->child(1), 1))
                ->setCurrentIndex(ui->comboBox_p1->currentIndex());

        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(1)->child(0), 1))
                ->setCurrentIndex(ui->comboBox_n2->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(1)->child(1), 1))
                ->setCurrentIndex(ui->comboBox_p2->currentIndex());

        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(2)->child(0), 1))
                ->setCurrentIndex(ui->comboBox_n3->currentIndex());
        qobject_cast<QComboBox *>(
                    ui->treeWidget_main->itemWidget(i->child(7)->child(2)->child(1), 1))
                ->setCurrentIndex(ui->comboBox_p3->currentIndex());
    }
}

void
MainWindow::start_streaming()
{
    try
    {
        std::cout << "now streaming..." << std::endl;
        ui->spinBox_bytes->setDisabled(true);
        m_nb_bytes = ui->spinBox_bytes->value();
        m_master.start_acquisition();

        m_is_streaming = true;

        ui->pushButton_startStream->setDisabled(true);
        ui->pushButton_stopStream->setDisabled(false);
        m_plotTimer.start(30); // Interval 0 means to refresh as fast as possible

    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
}

void
MainWindow::stop_streaming()
{
    m_master.stop_acquisition();
    m_is_streaming = false;
    ui->pushButton_startStream->setDisabled(false);
    ui->pushButton_stopStream->setDisabled(true);
    ui->spinBox_bytes->setDisabled(false);
    m_plotTimer.stop();
}

void *
MainWindow::streaming_loop(void *obj)
{
    while(reinterpret_cast<MainWindow *>(obj)->m_active)
        reinterpret_cast<MainWindow *>(obj)->push_lsl();
    return nullptr;
}

void
MainWindow::push_lsl()
{
    try
    {
        //usleep(1000);
        if(m_is_streaming)
        {
            int n;
            m_time = clk::now();
            n = m_master.read_all_signal();
            std::cout <<  "v " << n << std::endl;
            //m_duration = clk::now() - m_time;
//            std::cout <<  "t  " << m_duration.count() << " " << m_t++ << std::endl;
            //m_time = clk::now();
            for(int i = 0; i < m_boardItem.size(); i++)
            {
                //m_master.get_error(i);

                //std::cout <<  ui->horizontalSlider_var->value()<< std::endl;
                //if(m_master.data_ready(i, -1))
                //std::cout << m_master.get_error(i, true) << std::endl;	//       m_boardItem[i]->setText(1, QString::fromStdString(m_master.get_error(i)));
                //else
                //  m_boardItem[i]->setText(1, " ");

                for(int j = 0; j < 3; j++)
                {

                    if(m_master.data_ready(i, j, m_nb_bytes - 2) )
                        m_lsl_sample[3 * i + j] =
                                (m_nb_bytes == 2) ? m_master.fast_EMG(i, j)
                                                  : m_master.precise_EMG(i, j);
//                    float a=0.0005;
//                    m_mean_sample[3 * i + j] = (1-a)*m_mean_sample[3 * i + j] + a*m_lsl_sample[3 * i + j];
//                    float val = (abs(m_lsl_sample[3 * i + j]-m_mean_sample[3 * i + j])>2)?m_mean_sample[3 * i + j]:m_lsl_sample[3 * i + j];
                    m_plot->graph(i*3+j)->addData(m_t,1000/m_boardItem.size()/3*(i*3+j+ (0.5 + m_lsl_sample[3 * i + j]/2500)));

                }

            }

            m_t = ((sec)(m_time-m_start_time)).count();
            std::cout <<  "t  " << ((sec)(m_time-m_start_time)).count() << " v: " << m_lsl_sample[0]<< " v: " << m_lsl_sample[1]<< std::endl;
            m_lsl_outlet->push_sample(m_lsl_sample);

        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
    catch(std::string str)
    {
        std::cerr << "[ERROR] Got an exception: " << str << std::endl;
    }
}


void
MainWindow::addBoard(int id)
{
    //    for(int j=0;j<3;j++)
    //    {
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_neg_in",route_table[j][0]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_pos_in",route_table[j][1]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_enable", chx_enable[j]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_high_res", chx_high_res[j]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_high_freq", chx_high_freq[j]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_R1", R1[j]);
    //        m_settings->setValue("Board" + QString::number(i) +"/ADC"+QString::number(j)+"_R3", R3[j]);
    //    }
    // m_settings->setValue("Board" + QString::number(i) +"/R2", R2);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_main);
    m_boardItem.push_back(item);
    item->setText(0, QString::fromStdString("Board ") + QString::number(id));
    item->setCheckState(0, Qt::Checked);

    item->setForeground(1, Qt::red);

    std::vector<std::pair<QString, QStringList>> sItemsNames;
    sItemsNames.push_back(std::make_pair("Mode", QStringList() << "Shut down"
                                         << "Standby"
                                         << "Start"));
    sItemsNames.push_back(std::make_pair("Frequency", QStringList()
                                         << "102400 Hz"
                                         << "204800 Hz"));
    sItemsNames.push_back(std::make_pair("Resolution", QStringList()
                                         << "Low"
                                         << "High"));
    sItemsNames.push_back(std::make_pair("R1", QStringList() << "2"
                                         << "4"));
    sItemsNames.push_back(std::make_pair("R2", QStringList() << "4"
                                         << "5"
                                         << "6"
                                         << "8"));
    sItemsNames.push_back(std::make_pair("R3", QStringList() << "4"
                                         << "6"
                                         << "8"
                                         << "12"
                                         << "16"
                                         << "32"
                                         << "64"
                                         << "128"));

    std::vector<QTreeWidgetItem *> widgetItems;
    for(int i = 0; i < sItemsNames.size(); i++)
    {
        widgetItems.push_back(new QTreeWidgetItem(item));
        widgetItems[i]->setText(0, sItemsNames[i].first);
        QComboBox *comboBox = new QComboBox(this);
        comboBox->addItems(sItemsNames[i].second);
        ui->treeWidget_main->setItemWidget(widgetItems[i], 1, comboBox);
    }

    widgetItems.push_back(new QTreeWidgetItem(item));
    widgetItems[widgetItems.size() - 1]->setText(0, "Band width");
    widgetItems[widgetItems.size() - 1]->setText(1, "1000Hz | 300Hz");

    widgetItems.push_back(new QTreeWidgetItem(item));
    widgetItems[widgetItems.size() - 1]->setText(0, "ADCs");
    std::vector<QTreeWidgetItem *> adcs;
    for(int i = 0; i < 3; i++)
    {
        adcs.push_back(
                    new QTreeWidgetItem(widgetItems[widgetItems.size() - 1]));
        adcs[i]->setText(0, "ADC" + QString::number(i + 1));
        adcs[i]->setCheckState(1, Qt::Checked);

        QString names[2] = {"Neg. pad", "Pos. pad"};
        for(int j = 0; j < 2; j++)
        {
            QTreeWidgetItem *adcpad = new QTreeWidgetItem(adcs[i]);
            adcpad->setText(0, names[j]);
            QComboBox *comboBox = new QComboBox(this);
            comboBox->addItems(QStringList() << "NC"
                               << "1"
                               << "2"
                               << "3"
                               << "4"
                               << "5"
                               << "Common ref.");
            ui->treeWidget_main->setItemWidget(adcpad, 1, comboBox);
        }
    }
    qobject_cast<QComboBox *>(
                ui->treeWidget_main->itemWidget(item->child(1), 1))
            ->setCurrentIndex(m_settings
                              ->value("Board" + QString::number(id - 1) +
                                      "/ADC" + QString::number(0) +
                                      "_high_freq")
                              .value<int>());
    qobject_cast<QComboBox *>(
                ui->treeWidget_main->itemWidget(item->child(2), 1))
            ->setCurrentIndex(m_settings
                              ->value("Board" + QString::number(id - 1) +
                                      "/ADC" + QString::number(0) + "_high_res")
                              .value<int>());
    qobject_cast<QComboBox *>(
                ui->treeWidget_main->itemWidget(item->child(3), 1))
            ->setCurrentIndex(m_settings
                              ->value("Board" + QString::number(id - 1) +
                                      "/ADC" + QString::number(0) + "_R1")
                              .value<int>());
    qobject_cast<QComboBox *>(
                ui->treeWidget_main->itemWidget(item->child(4), 1))
            ->setCurrentIndex(
                m_settings->value("Board" + QString::number(id - 1) + "/R2")
                .value<int>());
    qobject_cast<QComboBox *>(
                ui->treeWidget_main->itemWidget(item->child(5), 1))
            ->setCurrentIndex(m_settings
                              ->value("Board" + QString::number(id - 1) +
                                      "/ADC" + QString::number(0) + "_R3")
                              .value<int>());

    //    item->child(6)->setText(1,ui->label_bandwidth->text());

    item->child(7)->child(0)->setCheckState(
                1, (Qt::CheckState)(
                    (int)(m_settings
                          ->value("Board" + QString::number(id - 1) + "/ADC" +
                                  QString::number(0) + "_enable")
                          .value<bool>()) *
                    2));
    item->child(7)->child(1)->setCheckState(
                1, (Qt::CheckState)(
                    (int)(m_settings
                          ->value("Board" + QString::number(id - 1) + "/ADC" +
                                  QString::number(1) + "_enable")
                          .value<bool>()) *
                    2));
    item->child(7)->child(2)->setCheckState(
                1, (Qt::CheckState)(
                    (int)(m_settings
                          ->value("Board" + QString::number(id - 1) + "/ADC" +
                                  QString::number(2) + "_enable")
                          .value<bool>()) *
                    2));

    for(int j = 0; j < 3; j++)
    {
        qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                                      item->child(7)->child(j)->child(0), 1))
                ->setCurrentIndex(m_settings
                                  ->value("Board" + QString::number(id - 1) +
                                          "/ADC" + QString::number(j) +
                                          "_neg_in")
                                  .value<int>());
        qobject_cast<QComboBox *>(ui->treeWidget_main->itemWidget(
                                      item->child(7)->child(j)->child(1), 1))
                ->setCurrentIndex(m_settings
                                  ->value("Board" + QString::number(id - 1) +
                                          "/ADC" + QString::number(j) +
                                          "_pos_in")
                                  .value<int>());



    }
}

void MainWindow::data_ploting()
{
    m_plot->replot();
    if(m_t>10)
        m_plot->xAxis->setRange(m_t-10, m_t);
//    double key = QTime::currentTime().msec(); // time elapsed since start of demo, in seconds
//    static double lastPointKey = 0;
//    for(int i =0 ; i<m_plots.size();i++)
//    {
//        m_plots[i]->replot();
//        m_plots[i]->xAxis->setRange(((sec)(m_time-m_start_time)).count() , 1, Qt::AlignRight);
//        m_plots[i]->yAxis->setRange(m_mean_sample[i]-2, m_mean_sample[i]+2);
//    }
//((sec)(m_time-m_start_time)).count()
    //    m_plots[0]->graph();
    //ui->customPlot->replot();

    //    // calculate frames per second:
    //    static double lastFpsKey;
    //    static int frameCount;
    //    ++frameCount;
    //    if (key-lastFpsKey > 2) // average fps over 2 seconds
    //    {
    //      ui->statusBar->showMessage(
    //            QString("%1 FPS, Total Data points: %2")
    //            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
    //            .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
    //            , 0);
    //      lastFpsKey = key;
    //      frameCount = 0;
    //    }
}
