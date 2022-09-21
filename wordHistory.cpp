#include "wordHistory.h"



WordHistory::WordHistory(QString word)
{
    m_word = word;
    m_history = new QList< history_of_ParseCounts * >;
    m_history_old = new QList< parseTimeWindows *  >;
    m_parseMap = new QMap<QString, history_of_ParseCounts * > ;
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
    if ( ! m_parseMap->contains(parse_string)){
        int count = 1;
        this_iteration_parse_count = new iteration_based_count(iteration, count);
        history_of_parse_counts = new history_of_ParseCounts(parse_string, this_iteration_parse_count);
        m_parseMap->insert(parse_string, history_of_parse_counts);
        m_history->append(history_of_parse_counts);
    } else{
        history_of_parse_counts = m_parseMap->value(parse_string);
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
    foreach(history_of_ParseCounts * history, * m_history){
        output.append(history->m_parse);
        foreach (iteration_based_count * count,  history->m_historical_parse_counts ){
            output.append(QString::number(count->m_iteration) + ":" + QString::number(count->m_count));
        }
    }



    return output;
}

void WordHistory::addTimeWindow(QStringList parse, timeWindow * TimeWindow){

}
