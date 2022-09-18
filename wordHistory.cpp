#include "wordHistory.h"

WordHistory::WordHistory(QString word)
{
    m_word = word;
    m_history = new QList< parseTimeWindows *  >;
    m_parseMap = new QMap<QString, parseTimeWindows * >;
}

bool WordHistory::test_for_contains_parse(QStringList parse){
    if (m_parseMap->contains(parse.join(" "))){
        return true;
    }
    else {return false;}
}

void WordHistory::respond_to_parse_used_on_this_iteration(QStringList parse, int iteration){
    QString parse_string = parse.join(" ");
    if ( ! m_parseMap->contains(parse_string) ){
        timeWindow * time_window = new timeWindow(iteration, 0);
        parseTimeWindows * parse_time_windows = new parseTimeWindows(parse, time_window);
        m_history->append(parse_time_windows);
        m_parseMap->insert(parse_string, parse_time_windows);
    } else {
        // this parse has already been seen, but now we check whether it was present on the last iteration:
        parseTimeWindows * parse_time_windows = m_parseMap->value(parse_string);
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
    foreach (parseTimeWindows * pTW, * m_history){
        temp =  pTW->m_parse;
        foreach (timeWindow * tW, pTW->m_timeWindows){
            temp +=   "   from " + QString::number(tW->m_start) + " to " + QString::number(tW->m_end) + "; ";
         }
        output << "  >>  "  + temp;
    }
    return output;
}

void WordHistory::addTimeWindow(QStringList parse, timeWindow * TimeWindow){

}
