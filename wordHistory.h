#ifndef WORDHISTORY_H
#define WORDHISTORY_H

#include <QStringList>
#include <QPair>
#include <QMap>


struct timeWindow{
    int m_start;
    int m_end;
    timeWindow (int t1, int t2) {m_start = t1; m_end=t2;}
};

struct parseTimeWindows{
    QString m_parse;  // this is a stringified version of the parse, using space to mark divisions
    QList< timeWindow* > m_timeWindows;

    parseTimeWindows(QStringList parse, timeWindow * this_timeWindow){
        m_parse = parse.join(" ");
        m_timeWindows.append(this_timeWindow);
    }

};


class wordHistory{

    QString                            m_word;
    QList< parseTimeWindows  >       * m_history;
    QMap<QString, parseTimeWindows>  * m_parseMap; // map from a stringified version of the parse to the parsetimeWindow
public:
    wordHistory(QString word) {m_word = word;}
    bool addParse( QStringList, int );
    bool containsParse( QStringList, int iteration );
    void addParseTimeWindow( parseTimeWindows * );
    QStringList display();
};







#endif // WORDHISTORY_H
