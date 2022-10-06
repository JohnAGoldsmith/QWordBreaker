#include "word.h"
#include "wordHistory.h"

Word::Word(QString word, int count)
{
    m_key = word;
    m_count = count;
    m_history = new Parses(word);
}
