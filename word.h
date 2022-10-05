#ifndef WORD_H
#define WORD_H
#include "lexicon.h"

class Word
{
public:
                            Word(QString, int count = 1);
    void                    compress_histories();
    WordHistory*            get_history() {return m_history;}
    int                     get_count() {return m_count;}
    QString                 get_key() {return m_key;}
    void                    increment_count (int n = 1) {m_count += n;}
    void                    write_word_to_json(QJsonObject &);
    void                    read_word_from_json(QJsonObject &);

private:
    QString                 m_key;
    int                     m_count;
    WordHistory *           m_history;

};

#endif // WORD_H
