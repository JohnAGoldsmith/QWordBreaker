#include "chart.h"
#include "lexicon.h"

Chart::Chart()
{
    m_entries = new QMap<QString, Entry*>;
    m_words = new QMap<QString, Word*>;

}

void Chart::add_entry(Entry* entry){
    if (m_entries->contains( entry->get_key() ) ){
        return;
    }
    m_entries->insert(entry->get_key(), entry);
    QLineSeries *series = new QLineSeries();
    for (int n1 = 0; n1 < entry->get_history()->count(); n1++){
        for (int n2 = entry->get_history()->at(n1)->m_first_iteration; n2 <= entry->get_history()->at(n1)->m_final_iteration; n2++){
            series->append(n2, entry->get_history()->at(n1)->m_count );
        }
    }
    addSeries(series);
    createDefaultAxes();
}
void Chart::add_word(Word* word){
    if (m_words->contains( word->get_key() ) ){
        return;
    }
}
void Chart::clear(){
    removeAllSeries();
}
