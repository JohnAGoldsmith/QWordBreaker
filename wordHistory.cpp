#include "wordHistory.h"

bool wordHistory::addParse(QStringList parse, int iteration){
    if (m_parseMap->contains parse.join(" ")) {
        return false;
    }
    timeWindow * this_timeWindow = new timeWindow(iteration, 0);
    parseTimeWindow *  this_parseTimeWindow =   new parseTimeWindow(parse, this_timeWindow);
}
