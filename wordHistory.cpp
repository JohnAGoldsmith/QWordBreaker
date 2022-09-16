#include "wordHistory.h"

bool wordHistory::test_for_contains_parse(QStringList parse){
    if (m_parseMap->contains(parse.join(" "))){
        return true;
    }
    else {return false;}
}

void wordHistory::respond_to_parse_used_on_this_iteration(QStringList parse, int iteration){
    QString parse_string = parse.join(" ");
    if (m_parseMap->contains(parse_string)) {
        // this parse has already been seen, but now we check whether it was present on the last iteration:
        parseTimeWindows * pTW = m_parseMap->value(parse_string);
        timeWindow * TW = pTW->m_timeWindows.last();
        if ( TW->test_open() ){
            // it was present on the last iteration
            return;
        }
        timeWindow * time_window = new timeWindow (iteration, 0);
        pTW->m_timeWindows.append(time_window);
        return;
    }
    timeWindow * time_window = new timeWindow(iteration, 0);
    parseTimeWindows *  this_parseTimeWindows =   new parseTimeWindows(parse, time_window);
    m_history->append(this_parseTimeWindows);
    return;
}

QStringList wordHistory::display(){
    QStringList output;
    output << m_word;
    foreach (parseTimeWindows * pTW, * m_history){
        output << pTW->m_parse;
        foreach (timeWindow * tW, pTW->m_timeWindows){
            output << "from " + QString::number(tW->m_start) + " to " + QString::number(tW->m_end);
         }
    }


}

void wordHistory::addTimeWindow(QStringList parse, timeWindow * TimeWindow){

}
