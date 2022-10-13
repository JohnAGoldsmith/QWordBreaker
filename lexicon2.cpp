#include "lexicon.h"
#include "wordbreaker.h"
#include <QFileDialog>
#include <QtMath>

#include "tablemodel.h"
void Lexicon::commence2(){
    ingest_broken_corpus(m_wordbreaker->m_corpus_filename, m_wordbreaker->m_numberoflines  );
    for (m_current_iteration = 1; m_current_iteration <= m_wordbreaker-> m_numberofcycles; m_current_iteration++) {
       set_progress_bar_2(m_current_iteration);
       generate_candidates2 (m_wordbreaker-> m_how_many_candidates_per_iteration);
       parse_corpus (m_current_iteration);
       //temporarily remove:
       //RecallPrecision(m_current_iteration );
     }
    copy_entries_to_entrylist(); // for qmodel of entries.
}

/*
struct double_string_count{
    string_count m_string_count_1;
    string_count m_string_count_2;
    double_string_count (string_count, string_count);
};
*/
struct Nominee {
    QString m_string;
    int     m_count;
    string_count m_string_count_1;
    string_count m_string_count_2;
    double       m_weightedMI;
    Nominee(QString nominee, int count, string_count & sc1, string_count& sc2)
    {   m_string = nominee,
        m_count = count;
        m_string_count_1.m_string = sc1.m_string;
        m_string_count_1.m_count = sc1.m_count;
        m_string_count_2.m_string = sc2.m_string;
        m_string_count_2.m_count = sc2.m_count;

         m_weightedMI = 0.0;
    }
    Nominee(const Nominee& nominee) {
        m_string = nominee.m_string;
        m_count = nominee.m_count;
        m_string_count_1.m_string = nominee.m_string_count_1.m_string;
        m_string_count_1.m_count = nominee.m_string_count_1.m_count;
        m_string_count_2.m_string = nominee.m_string_count_2.m_string;
        m_string_count_2.m_count = nominee.m_string_count_2.m_count;
        m_weightedMI = nominee.m_weightedMI;
    }
    void operator=(const  Nominee& nominee) {
        m_string = nominee.m_string;
        m_count = nominee.m_count;
        m_string_count_1.m_string = nominee.m_string_count_1.m_string;
        m_string_count_1.m_count = nominee.m_string_count_1.m_count;
        m_string_count_2.m_string = nominee.m_string_count_2.m_string;
        m_string_count_2.m_count = nominee.m_string_count_2.m_count;
        m_weightedMI = nominee.m_weightedMI;
    }
    void increment_count(int n) {m_count += n;}
};
int Lexicon::compute_number_of_candidates_per_iteration(){
    return 10;
}
bool myLessThan_2(const Nominee* s1, const Nominee* s2){
    return s1->m_weightedMI >  s2->m_weightedMI;
}
void create_nominee_map(QMap<QString, Entry*> * EntryDict, QMap<QString, Nominee*>& NomineeMap, QStringList line ){
    const int COUNT_THRESHOLD (5);
    for(int wordno = 0; wordno < line.length()-1; wordno++){
        Entry * entry1 = EntryDict->value( line.at(wordno) );
        if (entry1->get_count() < COUNT_THRESHOLD) {continue;}
        Entry * entry2 = EntryDict->value( line.at(wordno + 1) );
        if (entry2->get_count() < COUNT_THRESHOLD) {continue;}
        QString candidate = entry1->get_key() +  entry2->get_key();
        if  ( EntryDict->contains(candidate) )
        {   continue; }
        if (candidate == " " || candidate == "  "){
            int i = 1;
        }
        if (NomineeMap.contains(candidate))
        {  NomineeMap[candidate]->increment_count(1);
        }
        else
        {   string_count string_count_1 (entry1->get_key(), entry1->get_count() );
            string_count string_count_2 (entry2->get_key(), entry2->get_count() );
            Nominee * the_nominee = new Nominee(candidate, 1, string_count_1, string_count_2);
            NomineeMap[candidate] = the_nominee;
            //qDebug() << 89 << candidate;
        }
    }
    return;
}
void Lexicon::generate_candidates2(int how_many){
    QMap<QString, Nominee*> NomineeMap;
    QList<Nominee*> NomineeList;
    m_mainwindow->initialize_progress_bar_1();
    foreach(QStringList parsed_line,  m_ParsedCorpus){
        create_nominee_map(m_EntryDict, NomineeMap, parsed_line);
    }
    QMapIterator<QString, Nominee*> iter(NomineeMap);
    while(iter.hasNext()){
        Nominee * nominee (iter.next().value());
        double temp = ( double(nominee->m_count) / double(m_entries_token_count) )  /
                ( ( nominee->m_string_count_1.m_count/double(m_entries_token_count) ) *
                  (nominee->m_string_count_2.m_count / double(m_entries_token_count) )
                );
        nominee->m_weightedMI =  nominee->m_count * qLn(temp) ;
        NomineeList.append(nominee);
    }
    qDebug() << 108;
    std::sort (NomineeList.begin(), NomineeList.end(), myLessThan_2);
    m_CurrentCandidates.clear();
    foreach (Nominee * nominee, NomineeList){
        // currently nothing goes into m_DeletionDict !
        if (m_DeletionDict.contains(nominee->m_string)) {
            delete nominee;
            continue;
        }
        m_CurrentCandidates.append(nominee);
        if (m_CurrentCandidates.count() == how_many){
            break;
        }
    }
    qDebug() << 123;
    foreach(Nominee * nominee, m_CurrentCandidates ){
        //qDebug() << 120 << nominee->m_string << nominee->m_count <<  nominee->m_weightedMI;
        add_entry(string_count(nominee->m_string, nominee->m_count));
        qDebug() << 133 << nominee->m_string << nominee->m_count;
        delete nominee;
    }
    compute_dict_frequencies();
    return;
}
