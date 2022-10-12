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
void Entry::update_count_history(int current_iteration){
    if (m_count == 0){
        return;
    }
    if ( m_history.size() >= 1){
        int previous_iteration =  m_history.last()->m_final_iteration;
        int previous_count =      m_history.last()->m_count;
        if ( previous_iteration == current_iteration - 1 &&
             previous_count     == m_count ){
             m_history.last()->m_final_iteration +=1;
        } else{ // change in parse count on this iteration
            iteration_based_count * IBC = new iteration_based_count(current_iteration, m_count);
            m_history.append(IBC);
        }
    } else{
        // no history to this entry
        iteration_based_count * IBC = new iteration_based_count (current_iteration, current_iteration, m_count);
        m_history.append(IBC);
    }
    m_count = 0;
}


void Entry::display(int iteration_number, QFile outfile){
    iteration_based_count * pair;
     foreach ( pair, m_history) {
         qDebug()   <<   iteration_number << m_count;
     }
}
