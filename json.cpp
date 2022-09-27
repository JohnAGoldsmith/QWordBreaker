#include <QPair>
#include <QFileDialog>
#include "lexicon.h"
#include "wordbreaker.h"
#include "wordHistory.h"



void Lexicon::write_lexicon_to_json(QJsonObject & json_object){

//    QJsonObject json_object;

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

void Wordbreaker::write_wordbreaker_to_json_ask_for_filename(){
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    QString outfilename= dialog.getSaveFileName(m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.txt)");
    write_wordbreaker_to_json(outfilename);
}


void Wordbreaker::write_wordbreaker_to_json(QString outfilename ){
    QJsonObject toplevel_object;
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh:mm:ss");
    qDebug() << 19 << formattedTime;

    QJsonObject json_settings;
    //json_settings["filename"] = m_root_folder;
    json_settings["number_of_iterations"] = m_numberofcycles;
    json_settings["number_of_new_candidates_per_iteration"] = m_how_many_candidates_per_iteration;
    toplevel_object["1 settings"] = json_settings;

    m_lexicon->write_lexicon_to_json(toplevel_object);

    //QString fullfilename("wordbreaker.json");
    QFile saveFile(outfilename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open save file.");
         return;
    }
    qDebug() << 610 << outfilename;
    QJsonDocument doc(toplevel_object);
    saveFile.write(doc.toJson());

    // put info into json_object before returning


    return;
}

void Wordbreaker::read_json(  ){
    QString infilename= QFileDialog::getOpenFileName(m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.json)");
    QFile fileIn(infilename);
    if (!fileIn.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QByteArray thisData = fileIn.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(thisData));
    QJsonObject rootItem = loadDoc.object(); // this should be the root item;
    if (rootItem.contains("2-entries") && rootItem["2-entries"].isObject() ){
        QJsonObject EntriesObject = rootItem["2-entries"].toObject();
        m_lexicon->read_entries_from_json(EntriesObject);
    }
    m_main_window->place_entrydict_on_table_widget( m_lexicon->get_entry_dict() );

    if (rootItem.contains("3-words") && rootItem["3-words"].isObject() ){
        QJsonObject words_json = rootItem["3-words"].toObject();
        m_lexicon->read_words_from_json(words_json);
    }
    m_lexicon->put_wordlist_on_tablewidget( m_main_window->m_true_word_list_tablewidget );


}
void Lexicon::read_words_from_json(QJsonObject & words){
    int count;
    QJsonObject this_json_word;
    QJsonObject::const_iterator iter = words.constBegin();
    for (iter = words.begin(); iter != words.end(); iter++){
        if (iter.value().isObject()){
            this_json_word = iter.value().toObject();
            count = this_json_word["count"].toInt();
            Word* word = new Word(iter.key(), count);
            //qDebug() << 114 << count;
            word->read_word_from_json(this_json_word);
            add_word(word);
        }
    }
}
void Word::read_word_from_json(QJsonObject & json_word){

    if (json_word.contains("history") && json_word["history"].isArray()) {
        if (m_history) { delete m_history;}
        m_history = new WordHistory(m_key);
        QJsonArray parse_histories = json_word["history"].toArray();
        //qDebug() << 119 <<  m_key;
        for (int parse_no = 0; parse_no < parse_histories.size(); parse_no++) {
                /*     one parse    */
                if (parse_histories[parse_no].isObject() && parse_histories[parse_no].toObject().contains("parse") ) {
                QJsonObject this_json_parse_history = parse_histories[parse_no].toObject();
                if (this_json_parse_history.contains("parse")) {
                        QString this_parse = this_json_parse_history["parse"].toString();
                        //qDebug() << 126 << this_parse;
                        if (this_json_parse_history.contains("history") && this_json_parse_history["history"].isArray() ) {
                            QJsonArray this_history_array = this_json_parse_history["history"].toArray();
                            history_of_ParseCounts * these_parsecounts  = new history_of_ParseCounts(this_parse);
                            qDebug() << 131 << this_parse;

                            for (int slice_no = 0; slice_no < this_history_array.size(); slice_no++){
                                /*       one slice of the parse's history      */
                                QJsonObject json_slice = this_history_array[slice_no].toObject();
                                these_parsecounts->add_an_iteration_and_count_to_parse(json_slice["iteration"].toInt(), json_slice["count"].toInt() );
                                //qDebug() << 135 << json_slice["iteration"].toInt() <<  json_slice["count"].toInt() ;
                                qDebug() << 138 << json_slice["iteration"].toInt() <<
                                            139 << json_slice["count"].toInt();
                            }
                            m_history->add_history_of_parse_counts(these_parsecounts);

                            qDebug() << 142 << m_history->get_word() << m_history->get_parse_list()->size();
                        }
                }
                qDebug() << 145;
            }
            qDebug() << 147;
        } // end of one parse

        qDebug() <<149 <<  m_history->display();

    }
}

void Word::write_word_to_json(QJsonObject & words){
    QJsonObject this_word;
    this_word["count"]=  m_count;
    qDebug() << 157 << m_key;
    WordHistory * history = get_history();
    QJsonArray json_parse_histories_1;
    QList<history_of_ParseCounts*> * parse_list = history->get_parse_list();
    for (int n = 0; n < parse_list->count(); n++){
        QJsonObject json_parse_history_2;

        QString parse = parse_list->at(n)->m_parse;
        json_parse_history_2["parse"] = parse;

        QList< iteration_based_count * > * list = & parse_list->at(n)->m_historical_parse_counts;
        QJsonArray json_iter_count_history;
        for (int n2 = 0; n2 < list->size(); n2++ ){
            int iteration = list->at(n2)->m_iteration;
            int count = list->at(n2)->m_count;
            QJsonObject json_slice;
            json_slice["iteration"] = iteration;
            json_slice["count"] = count;
            json_iter_count_history.append(json_slice);
            qDebug() << 176 << "iteration" <<iteration  << "count" << count;
        }
        json_parse_history_2["history"] = json_iter_count_history;
        qDebug() << 181 << "This many parses so far" << n+1;
        qDebug() << 182 << "This many items in the last parse: " << list->size();
        json_parse_histories_1.append(json_parse_history_2);
    }
    this_word["history"] = json_parse_histories_1;
    words[m_key] = this_word;
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
    this_entry["count"] = m_count;
    this_entry["counts"] = count_list;
    json_entries[m_key] = this_entry;
 }
