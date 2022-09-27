#include "word.h"

Word::Word(QString word, int count)
{
    m_key = word;
    m_count = count;
    m_history = new WordHistory();
}
