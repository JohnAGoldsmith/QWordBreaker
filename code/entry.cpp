#include "entry.h"

Entry::Entry(QString key, int count)
{
    m_key = key;
    m_count = count;
    m_frequency = 0.0;


}
Entry::Entry(StringCount string_count){
    m_key = string_count.first;
    m_count = string_count.second;
}
void  Entry::reset_counts(int current_iteration){
    QPair<int,int> * pair;
    if (m_count_register.length() > 0 ){
        int last_count = (*m_count_register.last()).second;
        if (m_count != last_count){
            pair = new QPair<int,int>(current_iteration-1, m_count);
            m_count_register.append(pair);
        }
    }
    else{
        pair = new QPair<int,int>(current_iteration , m_count);
        m_count_register.append( pair);
    }
    m_count = 0;
}


void Entry::display(int iteration_number, QFile outfile){
    QPair<int,int> * pair;
     foreach ( pair, m_count_register) {
         qDebug()   <<   iteration_number << m_count;
     }
}
