#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListView>
#include <QProgressBar>
#include <QTableView>
#include <QTableWidget>
#include <QAbstractTableModel>
#include <QtCharts>
#include "listmodel.h"
#include "chart.h"

class Lexicon;
class Wordbreaker;
class QSpinBox;
class Entry;

typedef QMap<QString, double> Map;
//typedef QPair<QString, int> StringCount;
typedef QPair<QString, double> StringDouble;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
  friend Wordbreaker;
  friend Lexicon;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //void        show_entry_on_graph(Entry* entry);
    void        show_entries_on_graph(QList<Entry*> * entry_list);
    void        show_selected_entries_on_graph();
    void        show_selected_word_parse_history_on_chart();
private:
    Ui::MainWindow *ui;
    Wordbreaker * m_wordbreaker;
    QVBoxLayout * m_layout;

    Chart       * m_new_chart;
    QChartView  * m_new_chartview;

    //QChart      * m_chart;
    //QChartView  * m_chart_view;
    QGraphicsScene * m_chart_scene;
    QGraphicsView * m_graphics_view;
    QLogValueAxis * m_axis_Y;
    QValueAxis    * m_axis_X;

    QSplitter   * m_splitter_top;
    QSplitter   * m_splitter_1;
    QListView   * m_listview_1;
    QListView   * m_listview_2;
    QTableView  * m_entry_list_tableview;
    QTableWidget * m_entry_list_tablewidget;
    QTableWidget * m_true_word_list_tablewidget;
    QTableWidget * m_tablewidget_3;
    QSpinBox     * m_iteration_spinbox;
    QProgressBar * m_progress_bar_1;
    QProgressBar * m_progress_bar_2;

    void place_word_history_in_tablewidget(); // this is triggered by user selecting a new item in the true word widget (widget 2)
    void place_entrydict_on_table_widget(QMap<QString, Entry*> * );
    void            initialize_progress_bar_1();
};
#endif // MAINWINDOW_H
