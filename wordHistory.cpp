#include "wordHistory.h"
#include <QDebug>


Parses::Parses(QString word)
{   m_word = word;
    m_parse_list = new QList< parse_with_count_history * >;
    m_parse_map = new QMap<QString, parse_with_count_history * > ;
}
Parses::Parses()
{
    m_parse_list = new QList< parse_with_count_history * >;
    m_parse_map = new QMap<QString, parse_with_count_history * > ;
}
void Parses::respond_to_parse_used_on_this_iteration(QStringList parse, int iteration){
    QString parse_string = parse.join(" ");
    parse_with_count_history * this_parse_with_count_history;
    if ( ! m_parse_map->contains(parse_string)){
        this_parse_with_count_history = new parse_with_count_history(parse_string);
        this_parse_with_count_history->m_count_on_current_iteration = 1;
        add_parse_with_count_history(this_parse_with_count_history);
    } else{
        this_parse_with_count_history = m_parse_map->value(parse_string);
        this_parse_with_count_history->m_count_on_current_iteration++;
    }
    return;
}

QStringList Parses::display () const{
    QStringList output;
    qDebug();
    output << m_word;
    int previous_count (0);
    qDebug() << 92  << "number of parses" << m_parse_list->size();
    int temp = 0;
    foreach(parse_with_count_history * history, * m_parse_list){
        qDebug() << 93 << "index" <<  temp++;
        output.append(history->m_parse);
        qDebug() << 95 << "parse" << history->m_parse;
        qDebug() << 96 << "number of count records" << history->m_history_of_counts.size();
        foreach (iteration_based_count * count,  history->m_history_of_counts ){
            qDebug() << 97 << "first iteration"<< count->m_first_iteration;
            qDebug() << 98 << "count" <<count->m_count;
            qDebug() << 99 << "final iteration" << count->m_final_iteration;
            output.append(QString::number(count->m_first_iteration) + ":" + QString::number(count->m_final_iteration) + "  " + QString::number(count->m_count));
        }
    }
    return output;
}
QList<QStringList> Parses::display_as_table() const {
    /* the output is a list of columns, each column is the history of a parse*/
    QList<QStringList> columns;
    foreach (parse_with_count_history * history, * m_parse_list){
        QStringList this_column;
        this_column << history->m_parse;
        foreach (iteration_based_count * IBC, history->m_history_of_counts){
            for (int n = IBC->m_first_iteration; n <= IBC->m_final_iteration; n++){
                this_column.append(QString::number(n) + " " + QString::number(IBC->m_count) );
            }
        }
        columns.append(this_column);
    }
    return columns;
}
void Parses::add_parse_with_count_history(parse_with_count_history* PwCH){
    QString parse = PwCH->m_parse;
    if (! m_parse_map->contains(parse)){
        m_parse_list->append(PwCH);
        m_parse_map->insert(parse, PwCH);
    }
    return;
}
void Parses::update_count_history(int current_iteration){
    // after the iteration is complete, the information for the current iteration is shifted to the history;
    //qDebug() << 76 << ">>> " << m_word;
    QListIterator<parse_with_count_history*> iter(* m_parse_list);
    while(iter.hasNext()){
        parse_with_count_history * this_parse = iter.next();
        //qDebug() << 80 << "this parse: " << this_parse->m_parse << "history slices" << this_parse->m_history_of_counts.size();
        if (this_parse->m_count_on_current_iteration == 0){
            //qDebug() << 80 <<  "Parse with zero count:" << this_parse->m_parse;
            continue;
        }
        if (this_parse->m_history_of_counts.size() >= 1){
            int previous_iteration = this_parse->m_history_of_counts.last()->m_final_iteration;
            int previous_count = this_parse->m_history_of_counts.last()->m_count;
            if ( previous_iteration == current_iteration - 1 &&
                 previous_count     == this_parse->m_count_on_current_iteration ){
                this_parse->m_history_of_counts.last()->m_final_iteration +=1;
                //qDebug() << "Merge this iteration with previous iteration";
            } else{ // change in parse count on this iteration
                iteration_based_count * IBC = new iteration_based_count(current_iteration, this_parse->m_count_on_current_iteration);
                this_parse->m_history_of_counts.append(IBC);
                //qDebug() << "New and different count on this iteration";
            }
        } else{
            // no history to this parse.
            //parse_with_count_history * new_parse_with_history = new parse_with_count_history (this_parse->m_parse);
            this_parse->add_an_iteration_and_count_to_parse(current_iteration, this_parse->m_count_on_current_iteration);
            add_parse_with_count_history(this_parse);
            //qDebug() << 101 << "No history for this parse yet.";
        }
        this_parse->m_count_on_current_iteration = 0;
    }
}


