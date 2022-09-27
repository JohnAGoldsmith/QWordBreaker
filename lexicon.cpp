#include "lexicon.h"
#include "wordbreaker.h"
#include <QFileDialog>

#include "tablemodel.h"



Lexicon::Lexicon(Wordbreaker* wordbreaker, QObject *parent):QObject(parent)
{
    m_wordbreaker       = wordbreaker;
    m_EntryDict         = new  QMap<QString, Entry*> ;
    m_TrueDictionary    = new QMap<QString, Word*>;
    m_Limbo             = new QMap<QString, Entry*>;
}

void Lexicon::commence(){
    m_current_iteration = 0;
    m_total_word_count_in_parse 	= 0;
    m_wordbreaker->m_main_window->m_progress_bar_2->setMinimum(0);
    m_wordbreaker->m_main_window->m_progress_bar_2->setMaximum(m_wordbreaker->m_numberofcycles-1);
    read_broken_corpus(m_wordbreaker->m_corpus_filename, m_wordbreaker->m_numberoflines  );
    for (m_current_iteration = 1; m_current_iteration < m_wordbreaker-> m_numberofcycles; m_current_iteration++) {
       m_wordbreaker->m_main_window->m_progress_bar_2->setValue(m_current_iteration);
       generate_candidates(m_wordbreaker-> m_how_many_candidates_per_iteration);
       //.....................................
       parse_corpus (m_current_iteration);
       //.....................................
       RecallPrecision(m_current_iteration,  m_total_word_count_in_parse);
     }

    m_wordbreaker->m_main_window->place_entrydict_on_table_widget(m_EntryDict);


    if (false) {
        foreach (QString real_word, m_TrueDictionary->keys()){
            if ( ! m_WordHistories.contains(real_word) ){
                qDebug() << "Problem!  Real word was not analyzed.";
            } else {
                WordHistory* word_history = m_WordHistories.value(real_word);
                if (! word_history) {
                    qDebug() << "Problem line 87" << real_word;
                }
            }
        }
    }
    m_wordbreaker->write_wordbreaker_to_json("test.json");
}
void Lexicon::add_entry(Entry* entry)
{
    m_EntryDict->insert(entry->get_key(), entry);
    if (entry->get_key().length() > m_SizeOfLongestEntry){
        m_SizeOfLongestEntry = entry->get_key().length();
    }
}

void Lexicon::add_entry(StringCount string_count){
    Entry* entry = new Entry(string_count);
    m_EntryDict->insert(string_count.first,  entry);
    if (string_count.first.length() > m_SizeOfLongestEntry) {
        m_SizeOfLongestEntry =  string_count.first.length();
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
            //m_DeletionList.append(StringCount(key,iteration_number));
            //m_DeletionDict[key] = 1;
            //TempDeletionMap[key] = entry;
        }
    }
    /*
    foreach (QString key, TempDeletionMap.keys()){
        m_EntryDict->remove(key);
        if (TempDeletionMap.contains(key)){
            delete TempDeletionMap[key];
        }
    }
    */
 }

void Lexicon::read_corpus(QString infilename){
    // broken, don't use it at all.
         QString foldername;
         if (false &&  infilename.isEmpty()){
             infilename= QFileDialog::getOpenFileName(m_wordbreaker->m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.txt)");
             if(infilename.isEmpty())
                 return;
         }
         QFile fileIn(m_wordbreaker->m_corpus_filename);
         //qDebug() << m_wordbreaker->m_corpus_filename;
         if (!fileIn.open(QIODevice::ReadWrite | QIODevice::Text))
             return;
         QByteArray bytearray = fileIn.readAll();
         //qDebug() << bytearray;

   foreach (QString line, *get_corpus()){
       Entry* entry;
        for (int i = 0; i < line.length(); i++){
            QString letter = line.mid(i,1);
            if (!m_EntryDict->contains(letter)){
                entry = new Entry(letter,1);
            } else{
                entry = m_EntryDict->value(letter);
                entry->increment_count();
            }

        }
   }
   m_SizeOfLongestEntry = 1;
   compute_dict_frequencies();
}
void Lexicon::add_word(Word * word){
    m_TrueDictionary->insert(word->get_key(), word);
}
void Lexicon::read_broken_corpus(QString infilename, int numberoflines) {
    QStringList original_raw_corpus;
    infilename= QFileDialog::getOpenFileName(m_wordbreaker->m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.txt)");
    QFile fileIn(infilename);
    if (!fileIn.open(QIODevice::ReadWrite | QIODevice::Text))
        return;
    QTextStream in(&fileIn);

    // ------------------  read file -------------------------------//
    while(!in.atEnd()) {
        QString line = in.readLine();
        original_raw_corpus << line;
    }
    fileIn.close();

    //---------- analyze each line  --------------------------------------//
    QStringList  line_list;
    foreach (QString line, original_raw_corpus) {
        QString     this_line = "";       
        QList<int>  breakpoint_list;
        get_original_corpus()->append(line);
        line_list = line.split(' ', QString::SkipEmptyParts);
        if ( line_list.length() <=  1 ) {
            continue;
        }
        foreach (QString word, line_list){
            m_NumberOfTrueRunningWords += 1;
            if (!m_TrueDictionary->contains(word) ){
                    Word * p_word = new Word(word, 1);
                    //m_TrueDictionary->insert( word, p_word);
                    add_word(p_word);
            }else{
                m_TrueDictionary->value(word)->increment_count(1);
            }
            this_line += word;
            breakpoint_list.append( this_line.length() );            
        }
        get_corpus()->append(this_line);
        // add letters to form the initial set of entries...
        foreach (QString letter,   line) {
            if (! m_EntryDict->contains(letter) ) {
                Entry* this_entry = new Entry(letter, 1);
                m_EntryDict->insert(letter, this_entry);
            } else {
                m_EntryDict->value(letter)->increment_count(1);
            }
        }
        m_true_breakpoint_list.append(breakpoint_list);
        if (numberoflines > 0 and  get_corpus()->length() > numberoflines){
            break;
        }
    }
    m_SizeOfLongestEntry = 1;
    compute_dict_frequencies();

    // -------------   place information on GUI -------------------------------------------------//
    QModelIndex index1 = m_wordbreaker->m_corpus_model->index(0,0);
    QModelIndex index2 = m_wordbreaker->m_corpus_model->index(get_corpus()->length(),0);
    m_wordbreaker->m_corpus_model->emit dataChanged(index1, index2);
    m_wordbreaker->m_main_window->m_true_word_list_tablewidget->setRowCount(m_TrueDictionary->count());
    m_wordbreaker->m_main_window->m_true_word_list_tablewidget->setColumnCount(2);
    put_wordlist_on_tablewidget(   m_wordbreaker->m_main_window->m_true_word_list_tablewidget );
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
        foreach (QString word, m_EntryDict->keys() ) {
            int wordlength = 0;
            foreach (QString letter, word.split("") ){
                if (letter.length() == 0) {continue;}   // QString split on null string puts a null string at beginning and end;
                wordlength += m_LetterPlog[letter];
            }
            DictionaryLength += wordlength;
        }
        m_DictionaryLength = DictionaryLength;
        m_DictionaryLengthHistory.append(DictionaryLength);
        foreach(QString word,  m_EntryDict->keys() ) {
            double wordlength = 0.0;
            QStringList letters ( word.split("") );
            foreach (QString letter,   letters){
                if (letter.length() == 0) continue;
                wordlength += m_LetterPlog[letter];
            }
            DictionaryLength += wordlength;
        }
        m_DictionaryLength = DictionaryLength;
        m_DictionaryLengthHistory.append(DictionaryLength);
}
QList<int> convert_stringlist_to_breakpoints(QStringList * line, QList<int> & result){
    result.append(0);
    int length = 0;
    for (int n = 0; n < line->length(); n++){
        length += line->value(n).length();
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
       m_wordbreaker->m_main_window->m_progress_bar_1->setMinimum(0);
       m_wordbreaker->m_main_window->m_progress_bar_1->setMaximum(get_corpus()->length());

       QStringList result;
       foreach (QStringList * list, m_ParsedCorpus){
            delete list;
       }
       m_ParsedCorpus.clear();
       m_parsed_corpus_display.clear();
       m_CorpusCost = 0.0;
       m_NumberOfHypothesizedRunningWords = 0;


       if(false){
           QMapIterator<QString, Entry*> iter(* m_EntryDict);
           if (current_iteration > 0){
               while (iter.hasNext() ){
                   iter.next();
                   iter.value()->reset_counts(current_iteration);
               }
           }
       }

       if(true){
           QMapIterator<QString, Entry*> iter(* m_EntryDict);
           while (iter.hasNext() ){
                   iter.next();
                   iter.value()->set_count(0);
           }
        }


       QPair<QStringList*,double > pair;
       int lineno = 0;
       WordHistory * word_history;
       foreach (QString line, *get_corpus()){
           m_wordbreaker->m_main_window->m_progress_bar_1->setValue(lineno);
           //...........................//
           pair = parse_word(line);
           //...........................//

           // Note: pair.first is a pointer which we are responsible to clear
           QStringList* parsed_line (pair.first);
           double       bit_cost =   pair.second;
           m_ParsedCorpus.append(parsed_line);
           m_parsed_corpus_display.append(parsed_line->join(" "));
           m_CorpusCost += bit_cost;

           // iterate through entries, i.e., hypothetical words:
           // { .......................................................................................
           foreach (QString entry, *parsed_line){
               m_EntryDict->value(entry)->increment_count(1);
               m_NumberOfHypothesizedRunningWords += 1;
           }
           QList<int> hypothesized_breakpoint_list;
           convert_stringlist_to_breakpoints(parsed_line, hypothesized_breakpoint_list);
           // ....................................................................................... }




           // iterate through true words:
           // { .......................................................................................
           QList<int> true_breakpoint_list;
                                                              // todo make the true_breakpoint list start with zero!
           true_breakpoint_list << 0;
           for (int n = 0; n < m_true_breakpoint_list[lineno].length(); n++){
               true_breakpoint_list << m_true_breakpoint_list[lineno][n];
           }

           result.clear();
           for (int n = 1; n < true_breakpoint_list.length(); n++){
                int word_start = true_breakpoint_list[n-1];
                int word_end = true_breakpoint_list[n];
                QString true_word = line.mid(word_start, word_end - word_start);
                Word * p_word = m_TrueDictionary->value(true_word);
                if ( ! p_word ) {
                    qDebug() << 396 << "Problem! Real word not present in QMap keys.";
                }
                find_wordstring_covering_from_wordstart_to_wordend(hypothesized_breakpoint_list,
                                                                   line,
                                                                   word_start,
                                                                   word_end,
                                                                   result);
                p_word->get_history()->respond_to_parse_used_on_this_iteration(result, current_iteration);



                 // remove?
                if (false) {
                    if (!m_WordHistories.contains(true_word)){
                        word_history = new WordHistory(true_word);
                        m_WordHistories.insert(true_word, word_history);
                    } else{
                        word_history = m_WordHistories[true_word];
                    }
                    word_history->respond_to_parse_used_on_this_iteration(result, current_iteration);
                }
                //...................................................................................... }
            }
           lineno++;
       } // end of analyzing one particular line


        // This isn't quite right for reporting purposes: we need to know what the previous analyses were...
       FilterZeroCountEntries(current_iteration);

       compute_dict_frequencies();
       compute_dictionary_length();

       if (true){
       QMapIterator<QString, Entry*> iter(* m_EntryDict);
       while (iter.hasNext()){
           iter.next();
           iter.value()->place_count_in_history(current_iteration);
       }
       }

       if(false){
           QMapIterator<QString, Entry*> iter(* m_EntryDict);
           if (current_iteration > 0){
               while (iter.hasNext() ){
                   iter.next();
                   iter.value()->reset_counts(current_iteration);
               }
           }
       }



       QModelIndex index1 = m_wordbreaker->m_parsed_corpus_model->index(0,0);
       QModelIndex index2 = m_wordbreaker->m_parsed_corpus_model->index(get_parsed_corpus_display()->length(),0);
       m_wordbreaker->m_parsed_corpus_model->emit dataChanged(index1, index2);

}

void Lexicon::PrintParsedCorpus(QString outfile){
        foreach (QStringList* line, m_ParsedCorpus){
            // PrintList(line,outfile);
        }
}
/*
trxa9ze4
0803
oceane.adonis @ lcl.com
269318
*/
QPair<QStringList*, double> Lexicon::parse_word(QString word){
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
               else
               {
                    //if verboseflag: print >>outfile,"   %5s" % "No. ",
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
    for (int n = 0; n < wordlength; n++){
        delete Parse[n];
    }
    bitcost = BestCompressedLength[ wordlength ];
    return QPair<QStringList*, double> (Parse[wordlength], bitcost);
}
bool myLessThan(const StringCount s1, const StringCount s2){
    return s1.second >  s2.second;
}
QList<StringCount> Lexicon::generate_candidates(int how_many){
    Map NomineesMap;
    QList<StringCount> NomineeCountList;
    QList<StringCount> NomineeList;
    int repeat_candidate_count(0);

    foreach(QStringList* parsed_line,  m_ParsedCorpus){
        for(int wordno = 0; wordno < parsed_line->length()-1; wordno++){
            QString candidate = parsed_line->at(wordno) + parsed_line->at(wordno + 1);
            if  ( m_EntryDict->contains(candidate) ) {
                  if (m_EntryDict->contains(candidate)) {
                        continue;
                  }
            }
            if (NomineesMap.contains(candidate)){
                NomineesMap[candidate] += 1;
            }
            else{
                NomineesMap[candidate] = 1;
            }
        }
    }
    foreach (QString nominee, NomineesMap.keys()){
        NomineeCountList.append(StringCount(nominee, NomineesMap[nominee] ) );

    }
    std::sort (NomineeCountList.begin(), NomineeCountList.end(), myLessThan);
    foreach (StringCount nominee, NomineeCountList){
        // currently nothing goes into m_DeletionDict !
        if (m_DeletionDict.contains(nominee.first)) {
            continue;
        }
        NomineeList.append(nominee);
        if (NomineeList.count() == how_many){
            break;
        }
    }

    QStringList latex_data;
    foreach(StringCount nominee, NomineeList ){
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
void Lexicon::RecallPrecision(int iteration_number, int total_word_count_in_parse) {
        int total_true_positive_for_break = 0;
        int total_number_of_hypothesized_words = 0;
        int total_number_of_true_words = 0;
        for (int linenumber = 0; linenumber < m_true_breakpoint_list.length(); linenumber++){
            QList<int> truth = m_true_breakpoint_list[linenumber];
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
            QString this_line(m_wordbreaker->m_corpus[linenumber]);
            //qDebug() << 525 << this_line;










            foreach (QString piece,  *m_ParsedCorpus[linenumber]) {
                hypothesis_line_length +=  piece.length();
                hypothesis.append(hypothesis_line_length);
                temp += piece + " ";
            }
            //qDebug() << 593 << temp;

            QString Truth;
            Truth += "T ";
            for (int a = 0; a < truth.length(); a++){
                Truth += " " +  QString::number(truth[a]);
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
                int next_truth = truth[0];
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
                        else
                        {
                                if (next_truth < next_hypothesis)
                                {
                                    real_word_lag += 1;
                                    state = 1;
                                    //pointer = truth.takeFirst();
                                    truth.removeFirst();
                                    if (truth.length() == 0){
                                        qDebug() << 504 << "Problem";
                                    }
                                }
                                else
                                {
                                    //pointer = hypothesis.takeFirst();
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
                            //qDebug() << "Correct break 2: "<< this_line.mid(0,next_hypothesis);

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
                              //pointer = hypothesis.takeFirst();
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
                            //pointer = truth.takeFirst();
                            truth.removeFirst();
                            hypothesis.removeFirst();
                            //qDebug() << "Correct break 3: "<< this_line.mid(0,next_hypothesis);

                        }
                        else
                        {
                            if (next_truth < next_hypothesis)
                            {
                                real_word_lag += 1;
                                state = 1;
                                if (truth.length() == 0){
                                    qDebug() << 576 << "Problem";
                                }
                                pointer = truth.takeFirst();

                            }
                            else
                            {
                                hypothesis_word_lag += 1;
                                state =2;
                                if (hypothesis.length() == 0){
                                    qDebug() << 586 << "Problem";
                                }
                                pointer = hypothesis.takeFirst();

                            }
                        }
                        break;
                    }
                    default:{

                    }
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
//void Lexicon::put_wordlist_on_tablewidget(QMap<QString, Word* > * wordlist, QTableWidget* widget){
void Lexicon::put_wordlist_on_tablewidget(QTableWidget* widget){
    int row(0);
    widget->clear();
    widget->setRowCount(m_TrueDictionary->count());
    foreach (QString word,  m_TrueDictionary->keys()){
        QTableWidgetItem* item = new QTableWidgetItem(word);
        widget->setItem(row,0,item);
        //qDebug() << 868 << m_TrueDictionary->value(word)->get_count() << m_TrueDictionary->value(word)->get_key();
        item = new QTableWidgetItem(QString::number(m_TrueDictionary->value(word)->get_count()));
        widget->setItem(row,1,item);

        row++;
    }
}
void Lexicon::quit(){
        QCoreApplication::quit();
}


//void Lexicon::save_file(){
//    write_lexicon_to_json();
//}












