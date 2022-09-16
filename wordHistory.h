#ifndef WORDHISTORY_H
#define WORDHISTORY_H

#include <QStringList>
#include <QPair>
#include <QMap>


struct timeWindow{
    // a pair of integers indicating a sequence of continguous iterations in which the parse was found at least once.
    // a timeWindow in which the m_end is zero is one which is still in the running: the parse was present on the previous iteration.
    int m_start;
    int m_end;
    timeWindow (int t1, int t2) {m_start = t1; m_end=t2;}
    bool test_open() {if (m_end == 0) return true; return false;}
};

struct parseTimeWindows{
    // this is a struct of the parse and a list of the timeWindows it occurred in
    QString m_parse;  // this is a stringified version of the parse, using space to mark divisions
    QList< timeWindow* > m_timeWindows;

    parseTimeWindows(QStringList parse, timeWindow * this_timeWindow){
        m_parse = parse.join(" ");
        m_timeWindows.append(this_timeWindow);
    }

};
class wordHistory{
    // a class of a word and all of the parses it has been given during the iterations.
    QString                              m_word;
    QList< parseTimeWindows *  >       * m_history;
    QMap<QString, parseTimeWindows * > * m_parseMap; // map from a stringified version of the parse to the parsetimeWindow
public:
                wordHistory(QString word) {m_word = word;}
    void        respond_to_parse_used_on_this_iteration( QStringList, int );
    bool        test_for_contains_parse( QStringList );
    void        addTimeWindow(QStringList, timeWindow * );
    QStringList display();
};







#endif // WORDHISTORY_H
