#include "mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QSpinBox>

#include "wordbreaker.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    m_layout = new QVBoxLayout(this);

    QWidget * widget = new QWidget();
    widget->setLayout(m_layout);
    setCentralWidget(widget);



    m_splitter_top = new QSplitter(Qt::Vertical, this);
    m_layout->addWidget(m_splitter_top);

    m_iteration_spinbox  = new QSpinBox(this);
    m_splitter_top->addWidget(m_iteration_spinbox);


    m_progress_bar_1 = new QProgressBar;
    m_splitter_top->addWidget(m_progress_bar_1);

    m_progress_bar_2 = new QProgressBar;
    m_splitter_top->addWidget(m_progress_bar_1);
    m_splitter_top->addWidget(m_progress_bar_2);

    m_splitter_1 = new QSplitter (Qt::Horizontal,this);
    m_splitter_top->addWidget(m_splitter_1);
    m_listview_1 = new QListView(this);
    m_splitter_1->addWidget(m_listview_1);
    m_listview_2 = new QListView(this);
    m_splitter_1->addWidget(m_listview_2);

    //m_wordlist_tableview = new QTableView(this);
    //m_splitter_1->addWidget(m_wordlist_tableview);

    m_wordlist_tablewidget = new QTableWidget(15,2,this);
    m_splitter_1->addWidget(m_wordlist_tablewidget);
    m_wordlist_tablewidget->setSortingEnabled(false);

    m_wordbreaker= new Wordbreaker(this);
}



MainWindow::~MainWindow()
{
    delete ui;
}

