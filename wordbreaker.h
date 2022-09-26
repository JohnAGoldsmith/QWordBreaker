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
    QStringList     m_corpus;  // this is slightly modified during "read_broken_corpus()"; almost the same as m_raw_original_corpus.
    QStringList     m_raw_original_corpus;
    ListModel *     m_corpus_model;
    ListModel *     m_parsed_corpus_model;
    //TableModel *    m_word_table_model;

    QShortcut *     m_keyCtrlA; // start program



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

    void            write_wordbreaker_to_json(QJsonObject &);
    void            read_json(QString filename);
public:
    //void            commence() {m_lexicon->open_lexicon();}
    QStringList *   get_corpus(){return & m_corpus;}
    QStringList *   get_raw_original_corpus(){return & m_raw_original_corpus;}
    Lexicon *       get_lexicon() {return m_lexicon;}
public:
    Wordbreaker(MainWindow*, QObject * parent = nullptr);

};

#endif // WORDBREAKER_H

