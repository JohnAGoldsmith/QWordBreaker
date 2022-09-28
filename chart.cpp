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
    for (int n = 0; n < entry->get_history()->count(); n++){
        series->append(entry->get_history()->at(n)->m_iteration , entry->get_history()->at(n)->m_count );
    }
    addSeries(series);
    createDefaultAxes();
}
void Chart::add_word(Word* word){
    if (m_words->contains( word->get_key() ) ){
        return;
    }
}
