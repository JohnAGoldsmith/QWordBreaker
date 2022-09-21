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
struct iteration_based_count{
    int m_iteration;
    int m_count;
    iteration_based_count(int iteration, int count){
        m_iteration = iteration;
        m_count = count;
    }
    void increment_count(int n){
        m_count += n;
    }
};

struct history_of_ParseCounts{
    QString                           m_parse;             // stringified version of the parse, using space to mark division
    QList< iteration_based_count * >  m_historical_parse_counts; // each pair: iteration number, and count

    history_of_ParseCounts(QString parse, iteration_based_count * this_iteration_count) {
        m_parse = parse;
        m_historical_parse_counts.append(this_iteration_count);
    }
    void add_an_iteration_and_count_to_parse(int iteration, int count){
        iteration_based_count * this_iteration_count = new iteration_based_count (iteration, count);
        m_historical_parse_counts.append(this_iteration_count);
    }
};

class WordHistory{
    // a class of a word and all of the parses it has been given during the iterations.
    QString                                    m_word;
    QList< history_of_ParseCounts * >        * m_history;
    QMap<QString, history_of_ParseCounts * > * m_parseMap;     // map from a stringified version of the parse to the historicalParseCounts

    QList< parseTimeWindows *  >             * m_history_old;
    QMap<QString, parseTimeWindows * >       * m_parseMap_old; // map from a stringified version of the parse to the parseTimeWindows
public:
                WordHistory(QString word);
    void        respond_to_parse_used_on_this_iteration( QStringList, int );
    bool        test_for_contains_parse( QStringList );

    //remove this:
    void        addTimeWindow(QStringList, timeWindow * );

    QStringList                         display();
    QString                              get_word() {return m_word;}
    QList< parseTimeWindows *>       *   get_history_old() {return m_history_old;}
    //QList< history_of_ParseCounts* >  *   get_history() {return m_history;}
};







#endif // WORDHISTORY_H
