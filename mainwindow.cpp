#include "mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QSpinBox>


#include "wordbreaker.h"
#include "wordHistory.h"
#include "lexicon.h"


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

    m_entry_list_tablewidget = new QTableWidget(15,2,this);
    m_splitter_1->addWidget(m_entry_list_tablewidget);
    m_entry_list_tablewidget->setSortingEnabled(false);


    m_true_word_list_tablewidget = new QTableWidget(15,2,this);
    m_splitter_1->addWidget(m_true_word_list_tablewidget);
    m_true_word_list_tablewidget->setSortingEnabled(false);

    m_tablewidget_3 = new QTableWidget(15,2,this);
    m_splitter_1->addWidget(m_tablewidget_3);
    m_tablewidget_3->setSortingEnabled(false);



    m_wordbreaker= new Wordbreaker(this);

    connect(m_true_word_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::place_word_history_in_tablewidget );

}



MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::place_word_history_in_tablewidget(){
    QString word;
    m_tablewidget_3->clear();
    QTableWidgetItem * item = m_true_word_list_tablewidget->selectedItems().first();
    word = item->text();
    WordHistory* word_history = m_wordbreaker->get_lexicon()->get_WordHistories()->value(word);
    int row_count = word_history->display().length();
    m_tablewidget_3->setRowCount(row_count);
    for (auto rowno =0; rowno < row_count; rowno++) {
        m_tablewidget_3->setItem( rowno, 0, new QTableWidgetItem(word_history->display()[rowno]) );
    }
}

