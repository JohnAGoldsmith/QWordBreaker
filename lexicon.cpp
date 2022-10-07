#include "lexicon.h"
#include "wordbreaker.h"
#include <QFileDialog>

#include "tablemodel.h"



Lexicon::Lexicon(Wordbreaker* wordbreaker, QObject *parent):QObject(parent)
{   m_wordbreaker       = wordbreaker;
    m_mainwindow        = wordbreaker->m_main_window;
    m_EntryDict         = new  QMap<QString, Entry*> ;
    m_TrueDictionary    = new QMap<QString, Word*>;
    m_Limbo             = new QMap<QString, Entry*>;
    m_current_iteration = 0;
    m_SizeOfLongestEntry = 1;
    m_wordbreaker->m_main_window->m_progress_bar_2->setMinimum(0);
    m_wordbreaker->m_main_window->m_progress_bar_2->setMaximum(m_wordbreaker->m_numberofcycles-1);
}
void Lexicon::commence(){
    ingest_broken_corpus(m_wordbreaker->m_corpus_filename, m_wordbreaker->m_numberoflines  );
    for (m_current_iteration = 1; m_current_iteration <= m_wordbreaker-> m_numberofcycles; m_current_iteration++) {
       set_progress_bar_2(m_current_iteration);
       generate_candidates(m_wordbreaker-> m_how_many_candidates_per_iteration);
       parse_corpus (m_current_iteration);        
       //temporarily remove:
       //RecallPrecision(m_current_iteration );
     }
    copy_entries_to_entrylist(); // for qmodel of entries.
}
void Lexicon::copy_entries_to_entrylist(){
    m_EntryList.reserve(m_EntryDict->size());
    QMapIterator<QString, Entry*>  iter(*m_EntryDict);
    while ( iter.hasNext() ) {
        iter.next();
        string_count * SC  = new string_count( iter.key(), iter.value()->get_count() );
        m_EntryList.append( SC );
    }
}
void Lexicon::add_entry(Entry* entry){
    m_EntryDict->insert(entry->get_key(), entry);
    if (entry->get_key().length() > m_SizeOfLongestEntry){
        m_SizeOfLongestEntry = entry->get_key().length();
    }
}
void Lexicon::add_entry(string_count this_string_count){
    Entry* entry = new Entry(this_string_count);
    m_EntryDict->insert(this_string_count.m_string,  entry);
    if (this_string_count.m_string.length() > m_SizeOfLongestEntry) {
        m_SizeOfLongestEntry =  this_string_count.m_string.length();
    }
}
void Lexicon::FilterZeroCountEntries(int iteration_number){
    QMap<QString, Entry*> TempDeletionMap;
    foreach(QString key, m_EntryDict->keys()){
        if (key.length() == 1) {continue;}
        QMapIterator<QString, Entry*> iter(*m_EntryDict);
        Entry* entry = m_EntryDict->value(key);
        if (entry->get_count() == 0){
            //qDebug() << 82 << "iteration" << iteration_number << key << "size" << m_Limbo->count();
            m_Limbo->insert (key, m_EntryDict->value(key));
            if (m_EntryDict->contains(key) &&
                    m_EntryDict->value(key) ) {
                m_EntryDict->remove(key);
            }
        }
    }
 }

void Lexicon::add_word(Word * word){
    m_TrueDictionary->insert(word->get_key(), word);
}
void Lexicon::read_in_broken_corpus(QString infilename, int numberoflines){ // QStringList & original_raw_corpus, int numberoflines){
    infilename= QFileDialog::getOpenFileName(m_wordbreaker->m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.txt)");
    QFile fileIn(infilename);
    if (!fileIn.open(QIODevice::ReadWrite | QIODevice::Text))
        return;
    QTextStream in(&fileIn);
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line.length() == 0) {continue;}
        m_wordbreaker->m_corpus_with_spaces.append(line);
    }
    fileIn.close();
}
void  Lexicon::add_word_to_True_Dictionary(QString string){
    if (!m_TrueDictionary->contains(string) ){
            Word * p_word = new Word(string, 1);
            add_word(p_word);

    }else{
        m_TrueDictionary->value(string)->increment_count(1);
    }
}
void Lexicon::analyze_line(QString line ){
    QStringList line_list = line.split(' ', QString::SkipEmptyParts);
    QList<int> * breakpoint_list = new QList<int>;
    QString new_line;
    foreach (QString word, line_list){
        m_NumberOfTrueRunningWords += 1;
        add_word_to_True_Dictionary(word);
        new_line += word;
        breakpoint_list->append( new_line.length() );
    }
    m_true_breakpoint_list.append(* breakpoint_list);
    m_corpus_without_spaces.append(new_line);
}
void Lexicon::ingest_broken_corpus(QString infilename, int numberoflines) {

    read_in_broken_corpus(infilename, numberoflines); //original_raw_corpus, numberoflines );

    //---------- analyze each line  --------------------------------------//
    QStringList  line_list;
    foreach (QString line, *m_wordbreaker->get_corpus_with_spaces() ) { // *get_original_corpus() ) {            //original_raw_corpus) {

        analyze_line(line);

        /*  add letters to form the initial set of entries... */
        foreach (QString letter, line) {
            if (! m_EntryDict->contains(letter) ) {
                Entry* this_entry = new Entry(letter, 1);
                m_EntryDict->insert(letter, this_entry);
            } else {
                m_EntryDict->value(letter)->increment_count(1);
            }
        }
        if (numberoflines > 0 and  m_corpus_without_spaces.length() > numberoflines){
            break;
        }
    }

    compute_dict_frequencies();

    // -------------   place information on GUI -------------------------------------------------//
    //                 move this to wordbreaker class  ??                                         //
    m_wordbreaker->m_corpus_model->emit dataChanged(QModelIndex(), QModelIndex() );
    m_wordbreaker->m_main_window->m_true_word_list_tablewidget->setRowCount(m_TrueDictionary->count());
    m_wordbreaker->m_main_window->m_true_word_list_tablewidget->setColumnCount(2);
    put_wordlist_on_tablewidget(m_wordbreaker->m_main_window->m_true_word_list_tablewidget );
}

void Lexicon::compute_dict_frequencies(){
        double  TotalCount = 0.0;
        foreach (QString key, m_EntryDict->keys()){
          TotalCount += m_EntryDict->value(key)->get_count();
        }
        foreach (QString key, m_EntryDict->keys()){
            m_EntryDict->value(key)->set_frequency( m_EntryDict->value(key)->get_count() / TotalCount );
        }
        TotalCount = 0.0;
        foreach (QString letter, m_LetterDict.keys()){
            TotalCount += m_LetterDict[letter];
        }
        foreach (QString letter, m_LetterDict.keys() ) {
            m_LetterDict[letter] /= TotalCount;
            m_LetterPlog[letter] = -1.0 * log(m_LetterDict[letter]);
        }
}

void Lexicon::compute_dictionary_length(){
        double DictionaryLength = 0.0;
        QMapIterator<QString, Entry*> iter(*m_EntryDict);
        while(iter.hasNext()){
            iter.next();
            if (iter.value()->get_count() == 0){ continue; }
            int wordlength = 0;
            foreach (QString letter, iter.key().split("") ){
                if (iter.key().length() == 0) {continue;}   // QString split on null string puts a null string at beginning and end;
                wordlength += m_LetterPlog[letter];
            }
            DictionaryLength += wordlength;
        }
        m_DictionaryLength = DictionaryLength;
        m_DictionaryLengthHistory.append(DictionaryLength);
}
QList<int> convert_stringlist_to_breakpoints(QStringList  line, QList<int> & result){
    result.append(0);
    int length = 0;
    for (int n = 0; n < line.length(); n++){
        length += line.value(n).length();
        result.append(length);
    }
    return result;
}
QStringList find_wordstring_covering_from_wordstart_to_wordend(
                                                   QList<int> hypothesized_breakpoints,
                                                   QString line,
                                                   int word_start,
                                                   int word_end,
                                                   QStringList & result)
{
// We want to see how true words were analyzed in terms of the hypothesized words.
// We find last hypothesized_breakpoint at or before true word_start; call that start.
// find first hypothesized_breakpoint at or after word_end; call that end.
// concatenate all the hypothesized words from start to end.
    int start(0), end(0);
    int i, j;
    result.clear();
    for (i = 0; i < hypothesized_breakpoints.length(); i++){
        if (hypothesized_breakpoints[i] == word_start){
            start = i;
            break;
        } else {
            if (hypothesized_breakpoints[i] > word_start){
                i = i-1;
                start = i;
                break;
            }
        }
    }
    for (j = i+1; j < hypothesized_breakpoints.length(); j++){
        if (hypothesized_breakpoints[j] == word_end){
            end = j;
            break;
        } else{
            if (hypothesized_breakpoints[j] > word_end) {
                end = j;
                break;
            }
        }
    }
    for (int k = start; k < end; k++ ){
        int length = hypothesized_breakpoints[k+1]-hypothesized_breakpoints[k];
        result << line.mid(hypothesized_breakpoints[k], length );
    }
    return result;
}
void Lexicon::parse_corpus(int current_iteration) {
       m_mainwindow->initialize_progress_bar_1();
       QString danger = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}";
       m_mainwindow->m_progress_bar_1->setStyleSheet(danger);

       QStringList result;

       m_ParsedCorpus.clear();
       m_parsed_corpus_display.clear();
       m_CorpusCost = 0.0;
       m_NumberOfHypothesizedRunningWords = 0;
       if(true){
           QMapIterator<QString, Entry*> iter(* m_EntryDict);
           while (iter.hasNext() ){
                   iter.next();
                   iter.value()->set_count(0);
           }
        }
       QPair<QStringList*,double > pair;
       int lineno = 0;
       Parses * word_history;
       foreach (QString line, m_corpus_without_spaces){
           m_wordbreaker->m_main_window->m_progress_bar_1->setValue(lineno);
           parse_return  this_parse_return = parse_word(line);
           m_ParsedCorpus.append( this_parse_return.m_parse );
           m_parsed_corpus_display.append( this_parse_return.m_parse.join(" ") );
           m_CorpusCost += this_parse_return.m_bit_cost;
           foreach (QString entry, this_parse_return.m_parse){
                 m_EntryDict->value(entry)->increment_count(1);
                 m_NumberOfHypothesizedRunningWords += 1;
            }
            QList<int> hypothesized_breakpoint_list;
            convert_stringlist_to_breakpoints(this_parse_return.m_parse, hypothesized_breakpoint_list);

           QList<int> true_breakpoint_list; // todo make the true_breakpoint list start with zero!
           true_breakpoint_list << 0;
           for (int n = 0; n < m_true_breakpoint_list[lineno].length(); n++){
               true_breakpoint_list << m_true_breakpoint_list[lineno].at(n);
           }
           result.clear();
           for (int n = 1; n < true_breakpoint_list.length(); n++){
                int word_start = true_breakpoint_list[n-1];
                int word_end = true_breakpoint_list[n];
                QString true_word = line.mid(word_start, word_end - word_start);
                Word * p_word = m_TrueDictionary->value(true_word);
                if ( ! p_word ) {
                    qDebug() << 396 << "Problem! Real word not present in QMap keys." << true_word;
                }
                find_wordstring_covering_from_wordstart_to_wordend(hypothesized_breakpoint_list,
                                                                   line,
                                                                   word_start,
                                                                   word_end,
                                                                   result);
                p_word->get_history()->respond_to_parse_used_on_this_iteration(result, current_iteration);
                //qDebug() << 281 << true_word << "result" << result;
            }
           lineno++;
       } // end of analyzing one particular line

       FilterZeroCountEntries(current_iteration); // This isn't quite right for reporting purposes: we need to know what the previous analyses were...
       compute_dict_frequencies();
       compute_dictionary_length();

       QMapIterator<QString, Word*> iter(*m_TrueDictionary);
       while(iter.hasNext()){
           Word* word = iter.next().value();
           word->get_history()->update_count_history(current_iteration);
       }
}
void Lexicon::PrintParsedCorpus(QString outfile){
            foreach (QStringList line, m_ParsedCorpus){
                // PrintList(line,outfile);
            }
    }
parse_return Lexicon::parse_word(QString word){
   int                  wordlength( word.length() );
   QMap<int, QStringList*>  Parse;
   QString              Piece, LastChunk;
   QMap<int, double>    BestCompressedLength;
                        BestCompressedLength[0] = 0;
   double               CompressedSizeFromInnerScanToOuterScan (0.0),
                        bitcost;
   int                  LastChunkStartingPoint (0);

   //         # <------------------ outerscan -----------><------------------> #
   //         #                  ^---starting point
   //         # <----prefix?----><----innerscan---------->
   //         #                  <----Piece-------------->

   for (int outerscan = 1; outerscan <= wordlength; outerscan++){
        double  MinimumCompressedSize = 0.0;
        int     startingpoint = 0;
        if (outerscan > m_SizeOfLongestEntry){
             startingpoint = outerscan - m_SizeOfLongestEntry;
        }
        for (int innerscan = startingpoint; innerscan < outerscan; innerscan++) {            // innerscan in range(startingpoint, outerscan):
               Piece = word.mid(innerscan, outerscan - innerscan);
               if (m_EntryDict->contains(Piece) ) {
                   CompressedSizeFromInnerScanToOuterScan = -1 *  log(  m_EntryDict->value(Piece)->get_frequency() );
                   double newvalue =  BestCompressedLength[innerscan]  + CompressedSizeFromInnerScanToOuterScan;
                   if  (MinimumCompressedSize == 0.0 || MinimumCompressedSize > newvalue){
                            MinimumCompressedSize = newvalue;
                            LastChunk = Piece;
                            LastChunkStartingPoint = innerscan;
                   }
               }
        }
        BestCompressedLength[outerscan] = MinimumCompressedSize;
        if (LastChunkStartingPoint > 0) {
           Parse[outerscan] =  new QStringList(*Parse[LastChunkStartingPoint]) ;
        } else {
          Parse[outerscan] = new QStringList();
        }
        Parse[outerscan]->append(LastChunk);
    }
    bitcost = BestCompressedLength[ wordlength ];
    parse_return this_parse_return (*Parse[wordlength], bitcost);
    for (int n = 0; n < wordlength; n++){
        delete Parse[n];
    }

    return this_parse_return;
}
bool myLessThan(const string_count s1, const string_count s2){
    return s1.m_count >  s2.m_count;
}
QList<string_count> Lexicon::generate_candidates(int how_many){
    Map NomineesMap;
    QList<string_count> NomineeCountList;
    QList<string_count> NomineeList;
    int repeat_candidate_count(0);
    m_mainwindow->initialize_progress_bar_1();
    foreach(QStringList parsed_line,  m_ParsedCorpus){
        for(int wordno = 0; wordno < parsed_line.length()-1; wordno++){
            QString candidate = parsed_line.at(wordno) + parsed_line.at(wordno + 1);
            if  ( m_EntryDict->contains(candidate) )
            {   continue; }
            if (NomineesMap.contains(candidate))
            {   NomineesMap[candidate] += 1; }
            else{ NomineesMap[candidate] = 1;  }
        }
    }
    foreach (QString nominee, NomineesMap.keys()){
        NomineeCountList.append(string_count(nominee, NomineesMap[nominee] ) );
    }
    std::sort (NomineeCountList.begin(), NomineeCountList.end(), myLessThan);
    foreach (string_count nominee, NomineeCountList){
        // currently nothing goes into m_DeletionDict !
        if (m_DeletionDict.contains(nominee.m_string)) {
            continue;
        }
        NomineeList.append(nominee);
        if (NomineeList.count() == how_many){
            break;
        }
    }

    QStringList latex_data;
    foreach(string_count nominee, NomineeList ){
        qDebug() << 394 << nominee.m_string << nominee.m_count;
        add_entry(nominee);
    }
    compute_dict_frequencies();
    return NomineeList;
}
/*
void Lexicon::Expectation( ){
    m_NumberOfHypothesizedRunningWords = 0;
    foreach (QString this_line, m_Corpus){
        int wordlength = this_line.length();
        QMap<int, double>               m_ForwardProb;
        QMap<int, double>               m_BackwardProb;

        Forward( this_line, m_ForwardProb);
        Backward(this_line, m_BackwardProb);
        double this_word_prob = m_BackwardProb[0];

        if (this_word_prob > 0) {
            for (int nPos= 0; nPos < wordlength; nPos++){
                for (int End = nPos; nPos < wordlength - 1; nPos++){
                        if (End - nPos + 1 > m_SizeOfLongestEntry) {
                            continue;
                        }
                        if (nPos == 0 && End == wordlength - 1) {
                            continue;
                        }
                        QString Piece = this_line.mid(nPos, End+1);
                        if (m_EntryDict.contains(Piece)) {
                            Entry* this_entry = m_EntryDict[Piece];
                            double CurrentIncrement =
                                    ((m_ForwardProb[nPos] * m_BackwardProb[End+1]) * this_entry->get_frequency() ) / this_word_prob;
                            this_entry->increment_count( CurrentIncrement );
                            m_NumberOfHypothesizedRunningWords += CurrentIncrement;
                        }
                    }
              }
        }
    }
}

void Lexicon::Maximization(){
    foreach (Entry*  entry, m_EntryDict.values() ){
            entry->set_frequency( entry->get_count() / m_NumberOfHypothesizedRunningWords);
    }
}
void Lexicon::Forward (QString this_line, QMap<int,double>& ForwardProb){
        ForwardProb[0]=1.0;
        for (int Pos = 1; Pos < Length + 1; Pos++) {
            ForwardProb[Pos] = 0.0
            if (Pos - i > self.m_SizeOfLongestEntry):
                break
            Piece = this_line[i,Pos+1]
            if Piece in self.m_EntryDict:
                this_Entry = self.m_EntryDict[Piece]
                vlProduct = ForwardProb[i] * this_Entry.m_Frequency
                ForwardProb[Pos] = ForwardProb[Pos] + vlProduct
        return ForwardProb
}
# ---------------------------------------------------------#
    def Backward(self, this_line,BackwardProb):

        Last = len(this_line) -1
        BackwardProb[Last+1] = 1.0
        for Pos in range( Last, Pos >= 0,-1):
            BackwardProb[Pos] = 0
            for i in range(Pos, i <= Last,-1):
                if i-Pos +1 > m_SizeOfLongestEntry:
                    Piece = this_line[Pos, i+1]
                    if Piece in self.m_EntryDict[Piece]:
                        this_Entry = self.m_EntryDict[Piece]
                        if this_Entry.m_Frequency == 0.0:
                            continue
                        vlProduct = BackwardProb[i+1] * this_Entry.m_Frequency
                        BackwardProb[Pos] += vlProduct
        return BackwardProb
*/

/*
void Lexicon::PrintLexicon( QFile& outfile){
 for key in sorted(self.m_EntryDict.iterkeys()):
            self.m_EntryDict[key].Display(outfile)
        for iteration, key in self.m_DeletionList:
            print >>outfile, iteration, key
}
*/
void Lexicon::RecallPrecision(int iteration_number ) {
        int total_true_positive_for_break = 0;
        int total_number_of_hypothesized_words = 0;
        int total_number_of_true_words = 0;
        m_mainwindow->initialize_progress_bar_1(m_true_breakpoint_list.size());
        QString safe= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );border-bottom-right-radius: 7px;border-bottom-left-radius: 7px;border: 1px solid black;}";
        m_mainwindow->m_progress_bar_1->setStyleSheet(safe);

        for (int linenumber = 0; linenumber < m_true_breakpoint_list.length(); linenumber++){
            m_wordbreaker->m_main_window->m_progress_bar_1->setValue(linenumber);
            QList<int>  truth = m_true_breakpoint_list[linenumber];
            if (truth.length() < 2){
                //print >>outfile, "Skipping this line:", self.m_Corpus[linenumber];
                continue;
            }
            int number_of_true_words = truth.length() -1;
            QList<int>  hypothesis;
            int         hypothesis_line_length = 0;
            int         accurate_word_discovery = 0;
            int         true_positive_for_break = 0;
            int         word_too_big = 0;
            int         word_too_small = 0;
            int         real_word_lag = 0;
            int         hypothesis_word_lag = 0;

            QString temp;
            QString this_line(m_wordbreaker->m_corpus_with_spaces[linenumber]);
            //qDebug() << 525 << this_line;
            foreach (QString piece,  m_ParsedCorpus[linenumber]) {
                hypothesis_line_length +=  piece.length();
                hypothesis.append(hypothesis_line_length);
                temp += piece + " ";
            }
            //qDebug() << 593 << temp;

            QString Truth;
            Truth += "T ";
            for (int a = 0; a < truth.length(); a++){
                Truth += " " +  QString::number(truth.at(a));
            }
            //qDebug() << Truth;
            QString Hypot;
            Hypot = "H ";
            for (int a = 0; a < hypothesis.length(); a++){
                Hypot += " " +  QString::number(hypothesis[a]);
            }
            //qDebug() << Hypot;
            //qDebug() << " ";

            int number_of_hypothesized_words = hypothesis.length();
            // state 0: at the last test, the two parses were in agreement
            // state 1: at the last test, truth was # and hypothesis was not
            // state 2: at the last test, hypothesis was # and truth was not
            int         pointer = 0;
            int         state = 0;
            while ( truth.length() > 0 && hypothesis.length() > 0) {
                int next_truth = truth.at(0);
                int next_hypothesis  = hypothesis[0];
                switch (state){
                    case 0:
                     {  real_word_lag = 0;
                        hypothesis_word_lag = 0;
                        if (next_truth == next_hypothesis)
                        {   true_positive_for_break += 1;
                            //qDebug() << "Correct break 1: "<< this_line.mid(0,next_hypothesis);
                            accurate_word_discovery += 1;
                            state = 0;
                            truth.removeFirst();
                            hypothesis.removeFirst();
                            if (truth.length() == 0 && hypothesis.length() == 0){
                                break;
                            }
                        }
                        else {
                            if (next_truth < next_hypothesis){
                                    real_word_lag += 1;
                                    state = 1;                                   
                                    truth.removeFirst();
                                    if (truth.length() == 0){
                                        qDebug() << 504 << "Problem";
                                    }
                            } else {
                                hypothesis.removeFirst();
                                hypothesis_word_lag = 1;
                                state = 2;
                                if (hypothesis.length() == 0){
                                   qDebug() << 513 << "Problem";
                                 }
                            }
                        }
                        break;
                     }
                     case 1:
                     {
                        if (next_truth == next_hypothesis){
                            true_positive_for_break += 1;
                            word_too_big += 1;
                            state = 0;
                            if (truth.length() == 0 && hypothesis.length() == 0){
                                break;
                            }
                            pointer = truth.takeFirst();
                            hypothesis.removeFirst();
                        }
                        else
                        {
                           if (next_truth < next_hypothesis)
                           {
                              real_word_lag += 1;
                              state = 1; // redundantly
                              if (truth.length() == 0){
                                  qDebug() << 539 << "Problem";
                              }
                              pointer = truth.takeFirst();
                           }
                           else
                           {
                              hypothesis_word_lag += 1;
                              state = 2;
                              if (hypothesis.length() == 0){
                                  qDebug() << 548 << "Problem";
                              }
                              hypothesis.removeFirst();
                           }
                        }
                        break;
                     }
                     case 2:
                     {
                        if (next_truth == next_hypothesis){
                            true_positive_for_break += 1;
                            word_too_small +=1;
                            state = 0;
                            if (truth.length() == 0 && hypothesis.length() == 0){
                                break;
                            }
                            truth.removeFirst();
                            hypothesis.removeFirst();
                        }
                        else
                        {   if (next_truth < next_hypothesis)
                            {
                                real_word_lag += 1;
                                state = 1;
                                if (truth.length() == 0){
                                    qDebug() << 576 << "Problem";
                                }
                                pointer = truth.takeFirst();
                            }
                            else
                            {   hypothesis_word_lag += 1;
                                state =2;
                                if (hypothesis.length() == 0){
                                    qDebug() << 586 << "Problem";
                                }
                                pointer = hypothesis.takeFirst();
                            }
                        }
                        break;
                     }
                     default:{     }
                } // end of switch statement.
            }
            float precision = float(true_positive_for_break) /  number_of_hypothesized_words;
            float recall    = float(true_positive_for_break) /  number_of_true_words;

            //qDebug() << "Precision" << QString::number(precision) << "Recall:" << QString::number(recall);

            total_true_positive_for_break += true_positive_for_break;
            total_number_of_hypothesized_words += number_of_hypothesized_words;
            total_number_of_true_words += number_of_true_words;
        // the following calculations are precision and recall *for breaks* (not for morphemes)
        //formatstring = "%30s %6.4f %12s %6.4f"
        double total_break_precision = double(total_true_positive_for_break) /  double(total_number_of_hypothesized_words);
        double total_break_recall    = double(total_true_positive_for_break) /  double(total_number_of_true_words);
        m_CorpusCostHistory.append( m_CorpusCost);



        m_Break_based_RecallPrecisionHistory.append(QPair<double,double>( total_break_precision, total_break_recall ) ) ;
        //print            formatstring %( "Break based Word Precision", total_break_precision, "recall", total_break_recall);
        //print >>outfile, formatstring %( "Break based Word Precision", total_break_precision, "recall", total_break_recall);

        // Token_based precision for word discovery:
        if (true){
            int true_positives = 0;
            int these_true_positives = 0;
            foreach (QString word,  m_EntryDict->keys()){
                if ( m_TrueDictionary->contains(word) ) {
                     int true_count = m_TrueDictionary->value(word)->get_count();
                     these_true_positives = qMin(true_count, m_TrueDictionary->value(word)->get_count() );
                }
                else {
                    these_true_positives = 0;
                }
                true_positives += these_true_positives;
            }
            double word_recall = double (true_positives) / double (m_NumberOfTrueRunningWords);
            double word_precision = double (true_positives) / double (m_NumberOfHypothesizedRunningWords);
            m_Token_based_RecallPrecisionHistory.append(QPair<double,double>( word_precision, word_recall ) );

            //print >>outfile, formatstring %( "Token_based Word Precision", word_precision, "recall", word_recall);
            //print            formatstring %( "Token_based Word Precision", word_precision, "recall", word_recall);
        }

        // Type_based precision for word discovery:
        if (true){
            int true_positives = 0;
            foreach (QString word, m_EntryDict->keys() ) {
                if ( m_TrueDictionary->contains(word) ) {
                    true_positives +=1;
                }
            }
            double word_recall = double (true_positives) / double (m_TrueDictionary->count() );
            double word_precision = double (true_positives) /  m_EntryDict->count();
            m_Type_based_RecallPrecisionHistory.append( QPair<double,double>( QPair<double,double>(  word_precision, word_recall ) ) );
            // print >>outfile, "\n\n***\n"
            // print "Type_based Word Precision  %6.4f; Word Recall  %6.4f" %(word_precision ,word_recall)
            // print >>outfile, formatstring %( "Type_based Word Precision", word_precision, "recall", word_recall)
            // print            formatstring %( "Type_based Word Precision", word_precision, "recall", word_recall)
        }
        }

}
void Lexicon::put_wordlist_on_tablewidget(QTableWidget* widget){
    int row(0);
    widget->clear();
    widget->setRowCount(m_TrueDictionary->count());
    foreach (QString word,  m_TrueDictionary->keys()){
        QTableWidgetItem* item = new QTableWidgetItem(word);
        widget->setItem(row,0,item);
        item = new QTableWidgetItem(QString::number(m_TrueDictionary->value(word)->get_count()));
        widget->setItem(row,1,item);
        row++;
    }
}
void Lexicon::quit(){
        QCoreApplication::quit();
}











