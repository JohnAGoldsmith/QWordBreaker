#include <QSpinBox>
#include "wordbreaker.h"
#include "lexicon.h"
#include "tablemodel.h"


Wordbreaker::Wordbreaker(MainWindow * mainwindow, QObject * parent): QObject(parent)
{
    m_main_window = mainwindow;


    m_numberofcycles 		= 5;
    mainwindow->m_iteration_spinbox->setValue(m_numberofcycles);
    m_how_many_candidates_per_iteration 	= 100;
    m_numberoflines 		=  100000;
    m_datadirectory 		= "../../"; //"../Dropbox/data/english-browncorpus/";            // "../../data/english-browncorpus/";
    m_corpus_filename 		= "browncorpus100lines.txt";
    m_shortoutname 			= "wordbreaker-browncorpus-100lines.txt";
    m_corpus_filename 		= m_datadirectory  + m_corpus_filename;
    m_outdirectory 			= m_datadirectory + "wordbreaking/";
    m_outfilename 			= m_outdirectory + m_shortoutname+ QString::number(m_numberofcycles) + "i.txt";
    m_outfile_corpus_name   = m_outdirectory + m_shortoutname + QString::number(m_numberofcycles) + "_brokencorpus.txt";
    m_outfile_lexicon_name  = m_outdirectory + m_shortoutname+ QString::number(m_numberofcycles) + "_lexicon.txt";
    m_outfile_RecallPrecision_name= m_outdirectory + m_shortoutname + QString::number(m_numberofcycles) +  "_RecallPrecision.tsv";
    m_verboseflag = false;

    m_lexicon               = new Lexicon(this, this);

    m_corpus_model          = new ListModel( &m_corpus_with_spaces,  this);
    m_parsed_corpus_model   = new ListModel( m_lexicon->get_parsed_corpus_display(), this );
    //m_entries_model         = new TableModel( &m_lexicon->m_EntryList, this);

    //m_main_window->m_entry_list_tableview->setModel(m_entries_model);
    m_main_window->m_listview_1          ->setModel(m_corpus_model);
    m_main_window->m_listview_2          ->setModel(m_parsed_corpus_model);

    m_keyCtrlA = new QShortcut(mainwindow);
    m_keyCtrlA->setKey(Qt::CTRL + Qt::Key_A);

    m_keyCtrlJ = new QShortcut(mainwindow);
    m_keyCtrlJ->setKey(Qt::CTRL + Qt::Key_J);

    m_keyCtrlO = new QShortcut(mainwindow);
    m_keyCtrlO->setKey(Qt::CTRL + Qt::Key_O);

    m_keyCtrlQ = new QShortcut(mainwindow);
    m_keyCtrlQ->setKey(Qt::CTRL + Qt::Key_Q);

    m_keyCtrlS = new QShortcut(mainwindow);
    m_keyCtrlS->setKey(Qt::CTRL + Qt::Key_S);

    connect( m_keyCtrlA,  &QShortcut::activated, this,  &Wordbreaker::begin );
    connect( m_keyCtrlJ,  &QShortcut::activated, this,  &Wordbreaker::read_json );
    connect( m_keyCtrlO,  &QShortcut::activated, this,  &Wordbreaker::read_json );

    connect( m_keyCtrlQ,  &QShortcut::activated, m_lexicon,  &Lexicon::quit  );
    connect( m_keyCtrlS,  &QShortcut::activated, this,  &Wordbreaker::write_wordbreaker_to_json_ask_for_filename );
}

void Wordbreaker::begin(){
    m_lexicon->commence();

    m_parsed_corpus_model->emit dataChanged(QModelIndex(), QModelIndex());
    m_main_window->place_entrydict_on_table_widget(m_lexicon->m_EntryDict);
    m_entries_model         = new TableModel( &m_lexicon->m_EntryList, this);
    m_main_window->m_entry_list_tableview->setModel(m_entries_model);
    write_wordbreaker_to_json("test.json");
}

void Wordbreaker::load_entries_model(){
}

