#ifndef WORDHISTORY_H
#define WORDHISTORY_H

#include <QStringList>
#include <QPair>
#include <QMap>



struct iteration_based_count{
    int m_first_iteration;
    int m_final_iteration;
    int m_count;
    iteration_based_count(int iteration, int count){
        m_first_iteration = iteration;
        m_count = count;
        m_final_iteration = iteration;
    }
    void increment_count(int n){
        m_count += n;
    }
};


struct parse_with_count_history{
    QString                           m_parse;             // stringified version of the parse, using space to mark division
    int                               m_count_on_current_iteration;
    QList< iteration_based_count * >  m_history_of_counts; // each pair: iteration number, and count

    parse_with_count_history(QString parse, iteration_based_count * this_iteration_count) {
        m_parse = parse;
        m_history_of_counts.append(this_iteration_count);
        m_count_on_current_iteration = 0;
    }
    parse_with_count_history(QString parse){
        m_parse = parse;
        m_count_on_current_iteration = 0;
    }
    void add_an_iteration_and_count_to_parse(int iteration, int count){
        iteration_based_count * this_iteration_count = new iteration_based_count (iteration, count);
        m_history_of_counts.append(this_iteration_count);
    }
};

class Parses{
    // a class of a word and all of the parses it has been given during the iterations.
    QString                                      m_word;
    QList< parse_with_count_history * >        * m_parse_list;
    QMap<QString, parse_with_count_history * > * m_parse_map;     // map from a stringified version of the parse to the historicalParseCounts
public:
                                        Parses(QString word);
                                        Parses();
    void                                respond_to_parse_used_on_this_iteration( QStringList, int );
    bool                                test_for_contains_parse( QStringList );
    QStringList                         display() const ;
    QString                             get_word() const {return m_word;}
    QList<parse_with_count_history*>   *  get_parse_list() const {return m_parse_list;}
    void                                add_parse_with_count_history(parse_with_count_history*);
    QList<QStringList>                  display_as_table() const;
    void                                update_count_history(int current_iteration);
};





#endif // WORDHISTORY_H
