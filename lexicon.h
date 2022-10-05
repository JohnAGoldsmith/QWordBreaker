#ifndef LEXICON_H
#define LEXICON_H
//#include "wordbreaker.h"
#include "entry.h"
#include "mainwindow.h"
#include "wordHistory.h"
#include "word.h"

class Word;
class Wordbreaker;
struct line_analysis{
    QString     m_new_line;
    QList<int>  m_breakpoint_list;

};
struct parse_return {
    QStringList m_parse;
    double      m_bit_cost;
    parse_return(QStringList parse, double bit_cost){
        m_parse = parse;
        m_bit_cost = bit_cost;
    }
};
class Lexicon : public QObject
{
    friend Wordbreaker;

    Q_OBJECT

    Wordbreaker *                   m_wordbreaker;
    MainWindow *                    m_mainwindow;
    QStringList                     m_corpus_without_spaces;
    Map                             m_LetterDict;
    Map                             m_LetterPlog;
    QMap<QString, Entry*> *         m_EntryDict;
    QList <string_count*>           m_EntryList; // for use in Model
    QMap<QString, Word*> *          m_TrueDictionary;
    QMap<QString, Entry*> *         m_Limbo; // all Entries that have been removed because of zero counts.`

    // remove these; they go inside the entries themselves
    QMap<QString, WordHistory*>     m_EntryHistories; // for hypothesized words
    QMap<QString, WordHistory*>     m_WordHistories;  // for true words

    double                          m_DictionaryLength; // = 0   #in bits! Check this is base 2, looks like default base in python
    int                             m_SizeOfLongestEntry;
    double                          m_CorpusCost;
    int                             m_current_iteration;
    //int                             m_total_word_count_in_parse;
    QList<QStringList>              m_ParsedCorpus;
    QStringList                     m_parsed_corpus_display;// this is for convenience of programmer; it is m_ParsedCorpus, where each line is "displayed" as a QString instead of the original list of Qstrings
    int                             m_NumberOfHypothesizedRunningWords;
    int                             m_NumberOfTrueRunningWords;
    QList< QList<int> >             m_true_breakpoint_list;
    QList< string_count >            m_DeletionList;// = list()  # these are the words that were nominated and then not used in any line-parses *at all*.
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

    QStringList                   get_corpus_without_spaces() {return m_corpus_without_spaces;}
    //QStringList                  * get_original_corpus() {return m_wordbreaker->get_raw_original_corpus();}
    QStringList                  * get_parsed_corpus_display() {return & m_parsed_corpus_display;}
    QMap<QString, Entry*>        * get_entry_dict() {return  m_EntryDict;}
    QMap<QString, WordHistory*>  * get_WordHistories() {return & m_WordHistories;};
    //void        open_lexicon();
    void        add_entry(string_count);
    void        add_entry(Entry*);
    void        add_word(Word*);
        void        add_word_to_True_Dictionary(QString);
        void        analyze_line(QString);
    void        compress_records();
    void        FilterZeroCountEntries(int iteration_number);
    void        ingest_broken_corpus(QString infile_name, int numberoflines = 100);
    void        read_in_broken_corpus(QString infile_name, int numberoflines = 100); //QStringList & original_raw_corpus, int numberoflines = 100);
    //void        read_broken_corpus(QString infile_name, int numberoflines = 100);
    void        compute_dict_frequencies();
    void        compute_dictionary_length();
    Wordbreaker* get_wordbreaker() {return m_wordbreaker;}
    QMap<QString, Word*> *          get_TrueDictionary() {return m_TrueDictionary;}
    void                            parse_corpus( int  );
    parse_return     parse_word(QString word );
    void                            PrintParsedCorpus(QString filenameout);
    QList<string_count>                     generate_candidates(int how_many);
//    void                            Expectation();
//    void                            Maximization();
    void                            Forward( QString, QMap<int, double>& );
    void                            Backward(QString, QMap<int, double>& );
    void        print_lexicon(QFile&);
    void        RecallPrecision(int iteration_number );

    //void     put_wordlist_on_tablewidget(QMap<QString, Word* > *, QTableWidget* );
    void     put_wordlist_on_tablewidget(QTableWidget* );

    void     write_lexicon_to_json(QJsonObject & json_object);
    void     read_entries_from_json(QJsonObject & json_object);
    void     read_words_from_json(QJsonObject & words);
    void     copy_entries_to_entrylist();
    void    set_progress_bar_1(int i){m_wordbreaker->m_main_window->m_progress_bar_1->setValue(i);}
    void    set_progress_bar_2(int i){m_wordbreaker->m_main_window->m_progress_bar_2->setValue(i);}

public slots:
    void commence();

    void quit();
    //void save_file();




};


#endif // LEXICON_H
