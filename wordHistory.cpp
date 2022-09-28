#include "wordHistory.h"
#include <QDebug>


WordHistory::WordHistory(QString word)
{
    m_word = word;
    m_parse_list = new QList< history_of_ParseCounts * >;
    m_parse_map = new QMap<QString, history_of_ParseCounts * > ;

    // remove:
    m_history_old = new QList< parseTimeWindows *  >;
    m_parseMap_old = new QMap<QString, parseTimeWindows * >;

}
WordHistory::WordHistory()
{
//    m_word = word;
    m_parse_list = new QList< history_of_ParseCounts * >;
    m_parse_map = new QMap<QString, history_of_ParseCounts * > ;

    // remove:
    m_history_old = new QList< parseTimeWindows *  >;
    m_parseMap_old = new QMap<QString, parseTimeWindows * >;

}

bool WordHistory::test_for_contains_parse(QStringList parse){
    if (m_parseMap_old->contains(parse.join(" "))){
        return true;
    }
    else {return false;}
}

void WordHistory::respond_to_parse_used_on_this_iteration(QStringList parse, int iteration){
    QString parse_string = parse.join(" ");
    history_of_ParseCounts * history_of_parse_counts;
    iteration_based_count * this_iteration_parse_count;
    if ( ! m_parse_map->contains(parse_string)){
        int count = 1;
        this_iteration_parse_count = new iteration_based_count(iteration, count);
        history_of_parse_counts = new history_of_ParseCounts(parse_string, this_iteration_parse_count);
        m_parse_map->insert(parse_string, history_of_parse_counts);
        m_parse_list->append(history_of_parse_counts);
    } else{
        history_of_parse_counts = m_parse_map->value(parse_string);
        iteration_based_count * last_iteration_count = history_of_parse_counts->m_historical_parse_counts.last();
        if (last_iteration_count->m_iteration < iteration){
            iteration_based_count * new_iteration_count = new iteration_based_count(iteration, 1);
            history_of_parse_counts->m_historical_parse_counts.append(new_iteration_count);
        } else{
            history_of_parse_counts->m_historical_parse_counts.last()->increment_count(1);
        }
     }
    // remove all this:
    if ( ! m_parseMap_old->contains(parse_string) ){
        timeWindow * time_window = new timeWindow(iteration, 0);
        parseTimeWindows * parse_time_windows = new parseTimeWindows(parse, time_window);
        m_history_old->append(parse_time_windows);
        m_parseMap_old->insert(parse_string, parse_time_windows);
    } else {
        // this parse has already been seen, but now we check whether it was present on the last iteration:
        parseTimeWindows * parse_time_windows = m_parseMap_old->value(parse_string);
        timeWindow * time_window = parse_time_windows->m_timeWindows.last();
        if ( time_window->test_open() ){
            // it was present on the last iteration
        } else {
            // it was present earlier, but it stopped being used and so was closed earlier, and has now come back from the dead.
            time_window = new timeWindow(iteration,0);
            parse_time_windows->m_timeWindows.append(time_window);
        }
        return;
    }

    return;
}

QStringList WordHistory::display(){
    QStringList output;
    QString temp;
    output << m_word;
    foreach (parseTimeWindows * pTW, * m_history_old){
        temp =  pTW->m_parse;
        foreach (timeWindow * tW, pTW->m_timeWindows){
            temp +=   "   from " + QString::number(tW->m_start) + " to " + QString::number(tW->m_end) + "; ";
         }
        output << "  >>  "  + temp;
    }
    //output.clear();
    output << "second part: " +  m_word;
    int previous_count (0);
    //qDebug() << 92 << m_parse_list->size();
    foreach(history_of_ParseCounts * history, * m_parse_list){
        output.append(history->m_parse);
        //qDebug() << 95 << history->m_parse;
        //qDebug() << 96 << history->m_historical_parse_counts.size();
        foreach (iteration_based_count * count,  history->m_historical_parse_counts ){
            //qDebug() << 97 << count->m_iteration;
            //qDebug() << 98 << count->m_count;
            //qDebug() << 99;
            output.append(QString::number(count->m_iteration) + ":" + QString::number(count->m_count));
        }
    }

///dd

    return output;
}
void WordHistory::add_history_of_parse_counts(history_of_ParseCounts* HoPC){
    m_parse_list->append(HoPC);
    QString parse = HoPC->m_parse;
    m_parse_map->insert(parse, HoPC);
}

void WordHistory::addTimeWindow(QStringList parse, timeWindow * TimeWindow){

}
