#include "mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QSpinBox>
#include <QtCharts/QLineSeries>


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

    m_chart = new QChart();
    if (true){
        m_chart_view = new QChartView(m_chart);
        m_splitter_top->addWidget(m_chart_view);
    }
    if(false){
        m_chart_scene = new QGraphicsScene();
        m_graphics_view = new QGraphicsView(m_chart_scene);
        m_graphics_view->setRenderHint(QPainter::Antialiasing);// Set view antialiasing
        m_graphics_view->setSceneRect(0, 0, 2000, 280);// Set view size
        m_splitter_top->addWidget(m_graphics_view);
        m_graphics_view->show();
    }

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

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Iteration");
    //axisX->setRange(00, 100);
    axisX->setMinorGridLineVisible(true);
    axisX->setMinorTickCount(10);
    m_chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    //axisY->setRange(0, 150);
    axisY->setTitleText("Counts");
    m_chart->addAxis(axisY, Qt::AlignLeft);

    m_wordbreaker= new Wordbreaker(this);


    connect(m_true_word_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::place_word_history_in_tablewidget );
    connect(m_entry_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::show_selected_entry_on_graph );



}



MainWindow::~MainWindow()
{
    //delete ui;
}
void MainWindow::place_entrydict_on_table_widget( QMap<QString, Entry*> * entry_dict){
    QTableWidget * widget = m_entry_list_tablewidget;
    widget->clear();
    widget->setColumnCount(2);
    widget->setRowCount(entry_dict->count()    );

    int row = 0;
    QMapIterator<QString, Entry*> iter(* entry_dict);
    while(iter.hasNext()){
        iter.next();
       QTableWidgetItem* item = new QTableWidgetItem(iter.key());
       widget->setItem(row,0,item);
       QString count = QString::number(iter.value()->get_count());
       QTableWidgetItem* item2 = new QTableWidgetItem( count);
       widget->setItem(row,1,item2);
       row += 1;
   }

}
void MainWindow::place_word_history_in_tablewidget( ){

    m_tablewidget_3->clear();
    QTableWidgetItem * item = m_true_word_list_tablewidget->selectedItems().first();
    QString word_text = item->text();

    Word * word = m_wordbreaker->get_lexicon() ->get_TrueDictionary()->value(word_text);
    WordHistory* word_history = word->get_history();
    int row_count = word_history->display().length();
    m_tablewidget_3->setRowCount(row_count);
    for (auto rowno =0; rowno < row_count; rowno++) {
        m_tablewidget_3->setItem( rowno, 0, new QTableWidgetItem(word_history->display().at(rowno)) );
    }


}
void MainWindow::show_selected_entry_on_graph(){



    foreach(QTableWidgetItem * item, m_entry_list_tablewidget->selectedItems()){
        QString entry_text = item->text();
        Entry* entry = m_wordbreaker->get_lexicon()->get_entry_dict()->value(entry_text);
        if (!entry) { return;}
        QLineSeries *series = new QLineSeries();
        for (int n = 0; n < entry->get_history()->count(); n++){
            series->append(entry->get_history()->at(n)->m_iteration , entry->get_history()->at(n)->m_count );
            qDebug() << 112 << n << entry->get_history()->at(n)->m_count;
        }
        m_chart->addSeries(series);

    }
    if (false){
        QTableWidgetItem * item = m_entry_list_tablewidget->selectedItems().first();
        QString entry_text = item->text();

        Entry* entry = m_wordbreaker->get_lexicon()->get_entry_dict()->value(entry_text);
        QLineSeries *series = new QLineSeries();
        for (int n = 0; n < entry->get_history()->count(); n++){
            series->append( n, entry->get_history()->at(n)->m_count );
            qDebug() << 112 << n << entry->get_history()->at(n)->m_count;
        }
        m_chart->addSeries(series);

    }



    if (false){
        m_chart_scene->addItem(m_chart);
        m_graphics_view->show();
    }
    m_chart->createDefaultAxes();
}
