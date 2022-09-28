#ifndef CHART_H
#define CHART_H
#include <QtCharts>
#include <QMap>

class Word;
class Entry;

class Chart : public QChart
{
    QMap<QString, Entry*> * m_entries;
    QMap<QString, Word*>  * m_words;

public:
    Chart();
    void    add_entry(Entry*);
    void    add_word(Word*);
};

#endif // CHART_H
