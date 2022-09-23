#include <QPair>
#include "lexicon.h"
#include "wordbreaker.h"


void Lexicon::write_lexicon_to_json(QJsonArray & json_array){

    /*
    QJsonObject stats;
    stats["number of iterations"] = 40;
    json_array.append(stats);
    */
    QJsonObject json_entries;

    foreach(Entry * entry, * m_EntryDict){
        entry->write_entry_to_json(json_entries);
    }
    json_array.append(json_entries);
}

void Entry::write_entry_to_json(QJsonObject & json_entries){
    QJsonObject json_entry;
    json_entry["count"] = m_count;
    QPair<int,int> * pair;
    QJsonObject counts;
    for (int n = 0; n < m_count_register.size(); n++){
        QJsonValue this_count = m_count_register[n]->second;
        counts[QString::number(n)] = this_count;
    }
    json_entry["counts"] = counts;
    json_entries[m_key] = json_entry;
}

void Wordbreaker::write_wordbreaker_to_json(QJsonArray & json_array){
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh:mm:ss");
    qDebug() << 19 << formattedTime;

    QJsonObject json_settings;
    //json_settings["filename"] = m_root_folder;
    json_settings["number_of_iterations"] = m_numberofcycles;
    json_settings["number_of_new_candidates_per_iteration"] = m_how_many_candidates_per_iteration;
    json_array.append( json_settings ) ;

    m_lexicon->write_lexicon_to_json(json_array);

    QString fullfilename("wordbreaker.json");
    QFile saveFile(fullfilename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open save file.");
         return;
    }
    qDebug() << 610 << fullfilename;
    QJsonDocument doc(json_array);
    saveFile.write(doc.toJson());

    // put info into json_object before returning


    return;
}

