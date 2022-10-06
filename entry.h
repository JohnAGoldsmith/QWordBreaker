#ifndef ENTRY_H
#define ENTRY_H
#include "wordbreaker.h"
#include <QString>
#include <QList>
#include <QFile>
#include <QPair>
#include <QDebug>
#include "wordHistory.h"

class EntryHistory;

class Entry
{
  QString       m_key;
  int           m_count;
  double        m_frequency;
  //QList<QPair<int,int>* > m_history; // this is a pair (iteration-number, count);
                                            // in this register, the iteration number is the first
                                            // iteration at which this count was found.
                                            // If the count goes up or goes down on a later iteration,
                                            // there will be another pair in the register with that new count at that interation.
  QList<iteration_based_count* > m_history; // this is a triple (iteration-number-first, iteration-number-last, count);
                                            // in this register, the first iteration number is the first
                                            // iteration at which this count was found.
                                            // If the count goes up or goes down on a later iteration,
                                            // there will be another pair in the register with that new count at that interation.

public:
    Entry(QString key = "", int count =0);
    Entry (string_count string_count);
    ~Entry();
    void        compress_histories();
    void        display(int iteration_number, QFile outfile);
    int         get_count() {return m_count;}
    QString     get_key() {return m_key;}
    double      get_frequency() {return m_frequency;}
    void        increment_count (int n = 1) {m_count += n;}
    void        reset_counts(int current_iteration);
    void            read_json(QJsonObject);
    void            set_frequency(double freq) {m_frequency = freq;}
    void            set_key (QString key) {m_key = key;}
    void            set_count(int n) {m_count = n;}
    void            write_entry_to_json(QJsonObject & );
    void            read_entry_from_json(QJsonObject &);
    QList<iteration_based_count * > *  get_history() {return & m_history;}
    //void            place_count_in_history(int iteration);

};

#endif // ENTRY_H
