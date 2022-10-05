#include "entry.h"
#include "wordbreaker.h"
#include "wordHistory.h"

Entry::Entry(QString key, int count)
{
    m_key = key;
    m_count = count;
    m_frequency = 0.0;
}
Entry::Entry(string_count this_string_count){
    m_key = this_string_count.m_string;
    m_count = this_string_count.m_count;
}
Entry::~Entry(){
    for (int n = 0; n < m_history.size(); n++){
        delete m_history[n];
    }
}
void Entry::compress_histories(){


}
void Entry::place_count_in_history(int iteration){
    if (iteration > 1){
        if (m_history.size() >= 1){
            if (m_history.last()->m_count == m_count){
                return;
            }
        }
    }
    iteration_based_count * this_count = new iteration_based_count (iteration, m_count);
    m_history.append(this_count);
}
void  Entry::reset_counts(int current_iteration){
    iteration_based_count * pair;
    if (m_history.length() > 0 ){
        int last_count = (*m_history.last()).m_count;
        if (m_count != last_count){
            pair = new iteration_based_count(current_iteration-1, m_count);
            m_history.append(pair);
        }
    }
    else{
        pair = new  iteration_based_count(current_iteration , m_count);
        m_history.append( pair);
    }
    m_count = 0;
}


void Entry::display(int iteration_number, QFile outfile){
    iteration_based_count * pair;
     foreach ( pair, m_history) {
         qDebug()   <<   iteration_number << m_count;
     }
}
