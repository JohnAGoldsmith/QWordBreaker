#ifndef WORDHISTORY_H
#define WORDHISTORY_H

#include <QStringList>
#include <QPair>
#include <QMap>



struct iteration_based_count{
    int m_iteration;
    int m_final_iteration;
    int m_count;
    iteration_based_count(int iteration, int count){
        m_iteration = iteration;
        m_count = count;
        m_final_iteration = iteration;
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
    history_of_ParseCounts(QString parse){
        m_parse = parse;
    }

    void add_an_iteration_and_count_to_parse(int iteration, int count){
        iteration_based_count * this_iteration_count = new iteration_based_count (iteration, count);
        m_historical_parse_counts.append(this_iteration_count);
    }
};

class WordHistory{
    // a class of a word and all of the parses it has been given during the iterations.
    QString                                    m_word;
    QList< history_of_ParseCounts * >        * m_parse_list;
    QMap<QString, history_of_ParseCounts * > * m_parse_map;     // map from a stringified version of the parse to the historicalParseCounts

public:
                WordHistory(QString word);
                WordHistory();
    void        respond_to_parse_used_on_this_iteration( QStringList, int );
    bool        test_for_contains_parse( QStringList );
    QStringList                          display() const ;
    QString                              get_word() const {return m_word;}
    QList<history_of_ParseCounts*>   *   get_parse_list() const {return m_parse_list;}
    void                                add_history_of_parse_counts(history_of_ParseCounts*);
    QList<QStringList>                  display_as_table() const;
};


class EntryHistory{
    // A class to keep track of a hypothetical word's counts on each iteration
    QString                             m_entry;
    QList< iteration_based_count  >     m_history;
   // QList< history_of_ParseCounts * >        * m_parse_list;
    QMap<QString, history_of_ParseCounts * > * m_parse_map;     // map from a stringified version of the parse to the historicalParseCounts

public:
          EntryHistory(QString, int, int);

};




#endif // WORDHISTORY_H
