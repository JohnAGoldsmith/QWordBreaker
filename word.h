#ifndef WORD_H
#define WORD_H
#include "lexicon.h"

class Word
{
public:
                            Word(QString, int count = 1);
    WordHistory*            get_history() {return m_history;}
    int                     get_count() {return m_count;}
    void                    increment_count (int n = 1) {m_count += n;}
    void                    write_word_to_json(QJsonObject &);

private:
    QString                 m_key;
    int                     m_count;
    WordHistory *           m_history;

};

#endif // WORD_H
