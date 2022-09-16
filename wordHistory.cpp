#include "wordHistory.h"

bool wordHistory::addParse(QStringList parse, int iteration){
    if (m_parseMap->contains(parse.join(" "))) {
        return false;
    }
    timeWindow * this_timeWindow = new timeWindow(iteration, 0);
    parseTimeWindows *  this_parseTimeWindows =   new parseTimeWindows(parse, this_timeWindow);
}

QStringList wordHistory::display(){



}
