#ifndef WORDHISTORY_H
#define WORDHISTORY_H

#include <QStringList>
#include <QPair>



struct timeWindow{
    int m_start;
    int m_end;
};

struct parseTimeWindow{
    QStringList m_parse;
    QList< timeWindow > m_timeWindows;
};


class wordHistory{

    QString                     m_word;
    QList< parseTimeWindow  > * m_history;
    parseTimeWindow           * m_parseMap; // map from a stringified version of the parse to the parsetimeWindow
public:
    wordHistory(QString word) {m_word = word;}
    bool addParse( QStringList, int );
    bool containsParse( QStringList, int iteration );
    void addParseTimeWindow( parseTimeWindow * );
};







#endif // WORDHISTORY_H
