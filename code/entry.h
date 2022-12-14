#ifndef ENTRY_H
#define ENTRY_H
#include "wordbreaker.h"
#include <QString>
#include <QList>
#include <QFile>
#include <QPair>
#include <QDebug>


class Entry
{
  QString       m_key;
  int           m_count;
  double        m_frequency;
  QList<QPair<int,int>* > m_count_register;

public:
    Entry(QString key = "", int count =0);
    Entry (StringCount string_count);
    void        display(int iteration_number, QFile outfile);
    int         get_count() {return m_count;}
    QString     get_key() {return m_key;}
    double      get_frequency() {return m_frequency;}
    void        increment_count (int n = 1) {m_count += n;}
    void        reset_counts(int current_iteraiton);
    void        set_frequency(double freq) {m_frequency = freq;}
    void        set_key (QString key) {m_key = key;}
    void        set_count(int n) {m_count = n;}

};

#endif // ENTRY_H
