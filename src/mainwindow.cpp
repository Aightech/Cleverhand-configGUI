#include "mainwindow.h"
#include "../forms/ui_mainwindow.h"
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget_main->setColumnCount(2);
    ui->treeWidget_main->setHeaderLabels(QStringList() << tr("Boards")<< tr("Settings"));
    ui->treeWidget_main->setColumnWidth(0,150);
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_main);
    item->setText(0, QString::fromStdString("Board 1"));
    item->setCheckState(0, Qt::Checked);


    std::vector<std::pair<QString, QStringList>> sItemsNames;
    sItemsNames.push_back(std::make_pair("Mode",QStringList() << "Shut down" << "Standby" << "Start"));
    sItemsNames.push_back(std::make_pair("Frequency",QStringList() << "102400 Hz" << "204800 Hz"));
    sItemsNames.push_back(std::make_pair("Resolution",QStringList() << "Low" << "High"));
    sItemsNames.push_back(std::make_pair("R1",QStringList() << "2" << "4"));
    sItemsNames.push_back(std::make_pair("R2",QStringList() << "4" << "5"<< "6"<< "8"));
    sItemsNames.push_back(std::make_pair("R3",QStringList() << "4" << "6" << "8"<< "12"<< "16" << "32"<< "64"<< "128"));


    std::vector<QTreeWidgetItem *> widgetItems;
    for(int i=0; i<sItemsNames.size(); i++)
    {
        widgetItems.push_back(new QTreeWidgetItem(item));
        widgetItems[i]->setText(0, sItemsNames[i].first);
        QComboBox *comboBox = new QComboBox(this);
        comboBox->addItems(sItemsNames[i].second);
        ui->treeWidget_main->setItemWidget(widgetItems[i], 1, comboBox);

    }

    widgetItems.push_back(new QTreeWidgetItem(item));
    widgetItems[widgetItems.size()-1]->setText(0, "Band width");
    widgetItems[widgetItems.size()-1]->setText(1, "1000Hz | 300Hz");


    widgetItems.push_back(new QTreeWidgetItem(item));
    widgetItems[widgetItems.size()-1]->setText(0, "ADCs");
    std::vector<QTreeWidgetItem *> adcs;
    for(int i =0; i<3; i++)
    {
        adcs.push_back(new QTreeWidgetItem(widgetItems[widgetItems.size()-1]));
        adcs[i]->setText(0, "ADC"+QString::number(i));
        adcs[i]->setCheckState(1, Qt::Checked);


        QString names[2] = {"Neg. pad","Pos. pad"};
        for(int j=0;j<2;j++)
        {
            QTreeWidgetItem *adcpad = new QTreeWidgetItem(adcs[i]);
            adcpad->setText(0, names[j]);
            QComboBox *comboBox = new QComboBox(this);
            comboBox->addItems(QStringList() << "NC" << "1" << "2" << "3"<< "4" << "5" << "Common ref.");
            ui->treeWidget_main->setItemWidget(adcpad, 1, comboBox);
        }


    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

