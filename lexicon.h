#ifndef LEXICON_H
#define LEXICON_H
//#include "wordbreaker.h"
#include "entry.h"
#include "mainwindow.h"
#include "wordHistory.h"
#include "word.h"

class Word;
class Wordbreaker;

class Lexicon : public QObject
{

    Q_OBJECT

    Wordbreaker *                   m_wordbreaker;
    Map                             m_LetterDict;
    Map                             m_LetterPlog;
    QMap<QString, Entry*> *         m_EntryDict;
    //QMap<QString, int>              m_TrueDictionary;
    QMap<QString, Word*> *          m_TrueDictionary;


    // remove these; they go inside the entries themselves
    QMap<QString, WordHistory*>     m_EntryHistories; // for hypothesized words
    QMap<QString, WordHistory*>     m_WordHistories;  // for true words

    double                          m_DictionaryLength; // = 0   #in bits! Check this is base 2, looks like default base in python
    int                             m_SizeOfLongestEntry;
    double                          m_CorpusCost;
    int                             m_current_iteration;
    int                             m_total_word_count_in_parse;
    QList<QStringList*>             m_ParsedCorpus;
    QStringList                     m_parsed_corpus_display;// this is for convenience of programmer; it is m_ParsedCorpus, where each line is "displayed" as a QString instead of the original list of Qstrings
    int                             m_NumberOfHypothesizedRunningWords;
    int                             m_NumberOfTrueRunningWords;
    QList< QList<int> >             m_true_breakpoint_list;
    QList< StringCount >            m_DeletionList;// = list()  # these are the words that were nominated and then not used in any line-parses *at all*.
    Map                             m_DeletionDict;// = dict()  # They never stop getting nominated.
    QList< QPair<double,double> >   m_Break_based_RecallPrecisionHistory;// = list()
    QList< QPair<double,double> >   m_Token_based_RecallPrecisionHistory;// = list()
    QList< QPair<double,double> >   m_Type_based_RecallPrecisionHistory;//= list()
    QList<double>                   m_DictionaryLengthHistory;// = list()
    QList<double>                   m_CorpusCostHistory;// = list()
public:

    //Lexicon(Wordbreaker*);
    explicit Lexicon(Wordbreaker*, QObject *parent = nullptr);
    virtual ~Lexicon() {};

    QStringList                  * get_corpus() {return m_wordbreaker->get_corpus();}
    QStringList                  * get_original_corpus() {return m_wordbreaker->get_raw_original_corpus();}
    QStringList                  * get_parsed_corpus_display() {return & m_parsed_corpus_display;}
    QMap<QString, Entry*>        * get_entry_dict() {return  m_EntryDict;}
    QMap<QString, WordHistory*>  * get_WordHistories() {return & m_WordHistories;};
    //void        open_lexicon();
    void        add_entry(StringCount);
    void        FilterZeroCountEntries(int iteration_number);
    void        read_corpus(QString infile_name);
    void        read_broken_corpus(QString infile_name, int numberoflines = 100);
    void        compute_dict_frequencies();
    void        compute_dictionary_length();
    Wordbreaker* get_wordbreaker() {return m_wordbreaker;}
    void                            parse_corpus( int  );
    QPair<QStringList*,double >     parse_word(QString word );
    void                            PrintParsedCorpus(QString filenameout);
    QList<StringCount>                     generate_candidates(int how_many);
//    void                            Expectation();
//    void                            Maximization();
    void                            Forward( QString, QMap<int, double>& );
    void                            Backward(QString, QMap<int, double>& );
    void        print_lexicon(QFile&);
    void        RecallPrecision(int iteration_number, int total_word_count_in_parse);

    void     put_wordlist_on_tablewidget(QMap<QString, Word* > *, QTableWidget* );
    void     write_lexicon_to_json(QJsonArray &);


public slots:
    void commence();




};


#endif // LEXICON_H
