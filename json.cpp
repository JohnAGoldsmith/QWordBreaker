#include <QPair>
#include "lexicon.h"
#include "wordbreaker.h"



void Lexicon::write_lexicon_to_json(QJsonObject & json_object){


    QJsonObject stats;
    stats["number of iterations"] = 40;
    json_object["1 stats"]=  stats;

    QJsonObject  entries;

    foreach(Entry * entry, * m_EntryDict){
        entry->write_entry_to_json(entries);
    }
    json_object["2-entries"] = entries;


    QJsonObject words;
    QMapIterator<QString, Word*> iter(*m_TrueDictionary);




    while (iter.hasNext()){
        iter.next();
        iter.value()->write_word_to_json(words);
    }
    json_object["3-words"] = words;
}

void Word::write_word_to_json(QJsonObject & words){
    QJsonObject this_word;
    this_word["count"]=    QString::number(m_count);
    words[m_key] = this_word;

}


void Wordbreaker::write_wordbreaker_to_json(QJsonObject & toplevel_object){
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh:mm:ss");
    qDebug() << 19 << formattedTime;

    QJsonObject json_settings;
    //json_settings["filename"] = m_root_folder;
    json_settings["number_of_iterations"] = m_numberofcycles;
    json_settings["number_of_new_candidates_per_iteration"] = m_how_many_candidates_per_iteration;
    toplevel_object["1 settings"] = json_settings;

    m_lexicon->write_lexicon_to_json(toplevel_object);

    QString fullfilename("wordbreaker.json");
    QFile saveFile(fullfilename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open save file.");
         return;
    }
    qDebug() << 610 << fullfilename;
    QJsonDocument doc(toplevel_object);
    saveFile.write(doc.toJson());

    // put info into json_object before returning


    return;
}

void Wordbreaker::read_json(QString filename ){
    QFile file (filename);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QByteArray thisData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(thisData));
    QJsonObject rootItem = loadDoc.object(); // this should be the root item;
    if (rootItem.contains("2-entries") && rootItem["2-entries"].isObject() ){
        QJsonObject EntriesObject = rootItem["2-entries"].toObject();
        m_lexicon->read_entries_from_json(EntriesObject);
    }
    if (rootItem.contains("3-words") && rootItem["3-words"].isObject() ){
        QJsonObject EntriesObject = rootItem["3-words"].toObject();
        m_lexicon->read_entries_from_json(EntriesObject);
    }



}
void Lexicon::read_entries_from_json(QJsonObject & entries){
    QJsonObject::const_iterator iter = entries.constBegin();
    for (iter = entries.begin(); iter != entries.end(); iter++) {
            Entry* entry = new Entry(iter.key());
            QJsonObject entry_json = iter.value().toObject();
            entry->read_entry_from_json(entry_json);
            add_entry(entry);
        }
}
void Entry::read_entry_from_json(QJsonObject & json_entry){
    if (json_entry.contains("count")){
        m_count = json_entry["count"].toInt();
    }
    if (json_entry.contains("counts") && json_entry["counts"].isArray()){
        for (int n = 0; n < json_entry["counts"].toArray().size(); n++){
            if (json_entry["counts"].toArray().at(n).isObject()){
                QJsonObject iter_obj = json_entry["counts"].toArray().at(n).toObject();
                iteration_based_count * it_count = new iteration_based_count(iter_obj["iteration"].toInt(), iter_obj["count"].toInt());
                m_history.append(it_count);
            }
        }
    }
}

void Entry::write_entry_to_json(QJsonObject & json_entries){
    QJsonObject this_entry;
    QJsonArray count_list;
    for (int n = 0; n < m_history.size(); n++){
        QJsonObject iter_count;
        iter_count["iteration"] = m_history[n]->m_iteration;
        iter_count["count"] = m_history[n]->m_count;
        count_list.append(iter_count);
        //QJsonValue this_count = m_history[n]->m_count;
        //qDebug() << this_count;
        //this_entry[QString::number(n)] = this_count;
    }
    this_entry["counts"] = count_list;
    json_entries[m_key] = this_entry;
 }
