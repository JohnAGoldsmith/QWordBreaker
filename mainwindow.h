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
#include "listmodel.h"

class Lexicon;
class Wordbreaker;
class QSpinBox;

typedef QMap<QString, double> Map;
typedef QPair<QString, int> StringCount;
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

   // QTableView*    get_wordlist_tableview(){return m_wordlist_tableview;}

private:
    Ui::MainWindow *ui;
    Wordbreaker * m_wordbreaker;
    QVBoxLayout * m_layout;
    QSplitter * m_splitter_top;
    QSplitter   * m_splitter_1;
    QListView  * m_listview_1;
    QListView  * m_listview_2;
    //QTableView * m_wordlist_tableview;
    QTableWidget * m_wordlist_tablewidget;
    QTableWidget * m_true_word_list_tablewidget;
    QTableWidget * m_list_tablewidget_3;
    QSpinBox     * m_iteration_spinbox;
    QProgressBar * m_progress_bar_1;
    QProgressBar * m_progress_bar_2;


};
#endif // MAINWINDOW_H
