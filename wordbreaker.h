#ifndef WORDBREAKER_H
#define WORDBREAKER_H

#include <QMainWindow>
#include <QString>
#include <QList>
#include <QFile>
#include <QPair>
#include <QDebug>
#include <QtMath>
#include <QMap>
#include <QShortcut>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
//#include "lexicon.h"
#include "mainwindow.h"


class Lexicon;
class TableModel;

typedef QMap<QString, double> Map;

struct string_count{
    QString m_string;
    int     m_count;
      string_count(QString s, int c) {m_string = s; m_count = c;}
      string_count() {m_string = ""; m_count = 0;}
      void operator= (string_count sc) {m_string = sc.m_string; m_count = sc.m_count; }
};
struct string_value{
    QString m_string;
    double  m_value;
    string_value(QString s, double v) {m_string = s; m_value = v;}
};

struct   EvaluationTriple{
    int iteration;
    double word_precision;
    double word_recall;

    EvaluationTriple(int i, double p, double r ) {
        iteration = i;
        word_precision = p;
        word_recall = r;
    }
};


class Wordbreaker : public QObject
{
    Q_OBJECT

    friend Lexicon;

    Lexicon*        m_lexicon;
    MainWindow *    m_main_window;
    QStringList     m_corpus_with_spaces;
    ListModel *     m_corpus_model;
    ListModel *     m_parsed_corpus_model;
    TableModel *    m_entries_model;


    QShortcut *     m_keyCtrlA; // start program
    QShortcut *     m_keyCtrlJ; // open json file
    QShortcut *     m_keyCtrlO; // open text file
    QShortcut *     m_keyCtrlQ; // quit
    QShortcut *     m_keyCtrlS; // save json file
    QShortcut *     m_keyCtrlPeriod; // toggle screen
    QShortcut *     m_keyCtrlComma; // toggle screen backwards


    //QString       g_encoding 			= "utf8";
    int             m_numberofcycles;
    int             m_how_many_candidates_per_iteration;
    int             m_numberoflines ;
    QString         m_datadirectory ;
    QString         m_corpus_filename 	;
    QString         m_shortoutname 	;
    bool            m_verboseflag ;
    QString         m_outdirectory;
    QString         m_outfilename;
    QString         m_outfile_corpus_name;
    QString         m_outfile_lexicon_name;
    QString         m_outfile_RecallPrecision_name;

    //void            write_wordbreaker_to_json(QString filename = QString());
public slots:
    void            write_wordbreaker_to_json(QString);
    void            write_wordbreaker_to_json_ask_for_filename();
    void            read_json();
    void            load_entries_model();
    void            begin();


public:
    //void            commence() {m_lexicon->open_lexicon();}
    QStringList *   get_corpus_with_spaces(){return & m_corpus_with_spaces;}
    //QStringList *   get_raw_original_corpus(){return & m_raw_original_corpus;}
    Lexicon *       get_lexicon() {return m_lexicon;}
    int             get_number_of_iterations(){return m_numberofcycles;}

public:
    Wordbreaker(MainWindow*, QObject * parent = nullptr);

};

#endif // WORDBREAKER_H

