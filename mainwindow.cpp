#include "mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QSpinBox>
#include <QtCharts/QLineSeries>
#include <QXYSeries>
#include <QColor>
#include <QtGlobal>

#include "mainwindow.h"
#include "wordbreaker.h"
#include "wordHistory.h"
#include "lexicon.h"
#include "chart.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_screen_state = 1;
    m_number_of_screen_states = 2;
    m_layout = new QVBoxLayout(this);

    QWidget * widget = new QWidget();
    widget->setLayout(m_layout);
    setCentralWidget(widget);

    m_splitter_top = new QSplitter(Qt::Vertical, this);
    m_layout->addWidget(m_splitter_top);


        /*
        if (false) {
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
        }
        */
    m_new_chart = new Chart();
    m_new_chartview = new QChartView(m_new_chart);
    m_splitter_top->addWidget(m_new_chartview);

    m_iteration_spinbox  = new QSpinBox(this);
    m_iteration_spinbox->setMaximumHeight(30);
    m_splitter_top->addWidget(m_iteration_spinbox);

    m_progress_bar_1 = new QProgressBar;
    m_progress_bar_2 = new QProgressBar;
    m_splitter_top->addWidget(m_progress_bar_1);
    m_splitter_top->addWidget(m_progress_bar_2);
    m_iteration_spinbox->setMaximumHeight(30);
    m_progress_bar_1->setMaximumHeight(30);
    m_progress_bar_2->setMaximumHeight(30);


    // bottom half of screen
    // Corpus:
    m_splitter_1 = new QSplitter (Qt::Horizontal,this);
    m_splitter_top->addWidget(m_splitter_1);
    m_listview_1 = new QListView(this);
    m_splitter_1->addWidget(m_listview_1);
    m_listview_2 = new QListView(this);
    m_splitter_1->addWidget(m_listview_2);

    m_entry_list_tableview = new QTableView(this);
    m_splitter_1->addWidget(m_entry_list_tableview);

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
    read_in_colors();


    // can I remove ALL of this? on axes :
    m_axis_X = new QValueAxis();
    m_axis_X->setRange(0, m_wordbreaker->get_number_of_iterations());
    /*
    if (false){
        m_axis_X->setTitleText("Iteration");
        m_axis_X->setTickCount(10);
        m_axis_X->setLabelFormat("%.2f");
        m_axis_X->setMinorGridLineVisible(true);
        m_axis_X->setMinorTickCount(10);
        m_chart->addAxis(m_axis_X, Qt::AlignBottom);
        //m_chart->setAxisX(m_axis_X, series);
    }
    */

    m_axis_Y = new QLogValueAxis();

    /*
    if (false){
        m_axis_Y->setTitleText("Counts");
        m_axis_Y->setBase(2);
        m_axis_Y->setRange(0,40);
        //m_chart->setAxisY(m_axis_Y, series);
    }

    m_chart->addAxis(m_axis_Y, Qt::AlignLeft);
    */


    connect(m_true_word_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::place_word_history_in_tablewidget );
    connect(m_entry_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::show_selected_entries_on_graph );
    connect(m_true_word_list_tablewidget, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::show_selected_word_parse_history_on_chart );
    /*
    QObject::connect(m_entry_list_tablewidget, &QTableWidget::itemChanged,
                     this, [](QListWidgetItem * item) {
            if (item->checkState() == Qt::Checked){
            }
            else{
            }

        });
    */
}
MainWindow::~MainWindow()
{
    //delete ui;
}
void MainWindow::read_in_colors(){
    //m_colors.append(Qt::red);
    m_colors.append( Qt::blue);
    m_colors.append( Qt::green);
    m_colors.append( Qt::green);
    m_colors.append( Qt::cyan);
    m_colors.append( Qt::magenta);

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
       item->setCheckState(Qt::Unchecked);
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
    qDebug() << word_history->display();
    QList<QStringList> report = word_history->display_as_table();
    m_tablewidget_3->setColumnCount(report.size());
    int number_of_rows = 0;
    foreach(QStringList row, report){
        if (row.size() > number_of_rows){
            number_of_rows = row.size();
        }
    }
    m_tablewidget_3->setRowCount(number_of_rows);
    for (int col = 0; col < report.size(); col++){
        QStringList this_column = report.at(col);
        for (int row = 0; row < this_column.size(); row++){
            m_tablewidget_3->setItem(row, col,new QTableWidgetItem(this_column.at(row) ) );
        }
    }
}
void MainWindow::show_selected_word_parse_history_on_chart(){
    //QStringList entry_list;
    m_new_chart->clear();
    int temp = 0;
    int color_index = 0;
    QTableWidgetItem * item = m_true_word_list_tablewidget->selectedItems().first();
    QString word_text = item->text();
    Word * word = m_wordbreaker->get_lexicon() ->get_TrueDictionary()->value(word_text);
    WordHistory* word_history = word->get_history();
    foreach (history_of_ParseCounts * history_of_PC ,* word_history->get_parse_list() ){
        QLineSeries *series = new QLineSeries();
        //QXYSeries * series2 = new QXYSeries();
        QString parse = history_of_PC->m_parse;
        series->setName(parse);
        series->setPointsVisible(true);
        QPen pen = series->pen();
        pen.setWidth(6);
        if (parse == word_text){
            color_index = Qt::red;
        } else {
            if (color_index == m_colors.size() - 1){
                color_index = 1;
            }else{ color_index++;}
        }
        pen.setColor(m_colors[color_index]);
        series->setPen(pen);
        QList<iteration_based_count*> * IBC_list = & history_of_PC->m_historical_parse_counts;
        for (int n = 0; n < IBC_list->size(); n++){
            series->append(IBC_list->at(n)->m_iteration , IBC_list->at(n)->m_count );
            //qDebug() << 112 << history_of_PC->m_parse  << n << "iteration" << IBC_list->at(n)->m_iteration << "count " << IBC_list->at(n)->m_count;
        }
        m_new_chart->addSeries(series);
        temp++;

    }
    m_new_chart->createDefaultAxes();

    qDebug () << "this many parse histories" << temp;
}

//void MainWindow::show_entries_on_graph(QList<Entry*> * entry_list){

//}
/*
void MainWindow::show_entry_on_graph(Entry* entry){
    if (!entry) { return;}
    QString entry_text = entry->get_key();
    QLineSeries *series = new QLineSeries();
    for (int n = 0; n < entry->get_history()->count(); n++){
        series->append(entry->get_history()->at(n)->m_iteration , entry->get_history()->at(n)->m_count );
    }
    m_chart->addSeries(series);
    m_chart->createDefaultAxes();
}
*/
void MainWindow::show_selected_entries_on_graph(){
    foreach(QTableWidgetItem * item, m_entry_list_tablewidget->selectedItems()){
        QString entry_text = item->text();
        Entry* entry = m_wordbreaker->get_lexicon()->get_entry_dict()->value(entry_text);
        if (!entry) { continue;}

        //show_entry_on_graph(entry);
        m_new_chart->add_entry(entry);

        /*
        if (false) {
            QLineSeries *series = new QLineSeries();
            for (int n = 0; n < entry->get_history()->count(); n++){
                series->append(entry->get_history()->at(n)->m_iteration , entry->get_history()->at(n)->m_count );
            }
            m_chart->addSeries(series);
        }
        */
    }
    //m_chart->createDefaultAxes();
}
void MainWindow::initialize_progress_bar_1(int value){
    m_progress_bar_1->setMinimum(0);
    if (value == -1){  // default value, none given.
        m_progress_bar_1->setMaximum(m_wordbreaker->get_corpus_with_spaces()->length());
    } else{
        m_progress_bar_1->setMaximum(value);
    }
}
void MainWindow::toggle_screens_backwards(){

    if (m_screen_state == 1){
        m_screen_state = m_number_of_screen_states;
    } else{
        m_screen_state -= 1;
    }
    set_screen_state();
}
void MainWindow::toggle_screens(){

    if (m_screen_state == m_number_of_screen_states){
        m_screen_state = 1;
    } else{
        m_screen_state += 1;
    }
    set_screen_state();
}
void MainWindow::set_screen_state(){
    switch ( m_screen_state ) {
        case 1:{
            m_listview_1->setVisible(true);
            m_listview_2->setVisible(true);
            m_iteration_spinbox->setVisible(true);
            m_progress_bar_2->setVisible(true);
            m_progress_bar_1->setVisible(true);
            break;
        }
        case 2:{
            m_progress_bar_2->hide();
            m_progress_bar_1->hide();
            m_iteration_spinbox->hide();
            m_listview_1->hide();
            m_listview_2->hide();
            //int largeHeight = QGuiApplication::primaryScreen ()->virtualSize ().height ();
            //m_splitter_top->setSizes(QList<int>({100 , 100}));

            break;
        }
        default:
            {   m_screen_state = 1;
                m_listview_1->setVisible(true);
                m_listview_2->setVisible(true);
                break;
            }
    }
}







