#include "lexicon.h"
#include "wordbreaker.h"
#include <QFileDialog>
#include "tablemodel.h"


/*
Lexicon::Lexicon(Wordbreaker * wordbreaker)
{
    m_wordbreaker = wordbreaker;

}
*/
Lexicon::Lexicon(Wordbreaker* wordbreaker, QObject *parent):QObject(parent)
{
    m_wordbreaker = wordbreaker;
    m_EntryDict = new  QMap<QString, Entry*> ;
    //m_EntryDict = new QMap<QString, Entry*> ;
    //m_EntryList = nullptr;




}

void Lexicon::commence(){
   //read_corpus(QString());
   read_broken_corpus(m_wordbreaker->m_corpus_filename, m_wordbreaker->m_numberoflines  );
   m_current_iteration = 0;
   m_total_word_count_in_parse 	= 0;
   m_wordbreaker->m_main_window->m_progress_bar_2->setMinimum(0);
   m_wordbreaker->m_main_window->m_progress_bar_2->setMaximum(m_wordbreaker->m_numberofcycles-1);

   for (m_current_iteration = 1; m_current_iteration < m_wordbreaker-> m_numberofcycles; m_current_iteration++) {
       //qDebug() << "iteration "<< m_current_iteration;
       m_wordbreaker->m_main_window->m_progress_bar_2->setValue(m_current_iteration);
       generate_candidates(m_wordbreaker-> m_how_many_candidates_per_iteration);
       parse_corpus (m_current_iteration);
       RecallPrecision(m_current_iteration,  m_total_word_count_in_parse);
    }

   QTableWidget * widget =  m_wordbreaker->m_main_window->m_wordlist_tablewidget;
    widget->clear();
    widget->setColumnCount(2);
    widget->setRowCount(m_EntryDict->count()    );

    int row = 0;
    foreach (Entry* entry, m_EntryDict->values()){
        QTableWidgetItem* item = new QTableWidgetItem(entry->get_key());
        widget->setItem(row,0,item);
        QString  count  = QString::number(row);
        count = QString::number(entry->get_count());
        QTableWidgetItem* item2 = new QTableWidgetItem( count);
        widget->setItem(row,1,item2);
        qDebug() << row << entry->get_key() << count;
        row += 1;
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
        Entry* entry = m_EntryDict->value(key);
        if (entry->get_count() == 0){
            m_DeletionList.append(StringCount(key,iteration_number));
            m_DeletionDict[key] = 1;
            TempDeletionMap[key] = entry;
            //qDebug() << "Excluding this bad candidate"<< key;
        }
    }
    foreach (QString key, TempDeletionMap.keys()){
        m_EntryDict->remove(key);
        if (TempDeletionMap.contains(key)){
            delete TempDeletionMap[key];
        }
    }
 }

void Lexicon::read_corpus(QString infilename){
         //qDebug() <<  "Name of data file: " << infilename;
         //ui->setupUi(this);
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
/*
         if not os.path.isfile(infilename):
             print "Warning: ", infilename, " does not exist."
         if g_encoding == "utf8":
             infile = codecs.open(infilename, encoding = 'utf-8')`
         else:
             infile = open(infilename)
         self.m_Corpus = infile.readlines() # bad code if the corpus is very large -- but then we won't use python.
 */
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
void Lexicon::read_broken_corpus(QString infilename, int numberoflines) {

    infilename= QFileDialog::getOpenFileName(m_wordbreaker->m_main_window, "Choose File", "/home/jagoldsm/Dropbox/data/english-browncorpus", "text files (*.txt)");

    qDebug() << "Name of data file: " << m_wordbreaker->m_corpus_filename ;
    QFile fileIn(infilename);

    if (!fileIn.open(QIODevice::ReadWrite | QIODevice::Text))
        return;
    QTextStream in(&fileIn);


    QStringList raw_corpus;
    while(!in.atEnd()) {
        QString line = in.readLine();
        raw_corpus << line;
    }
    fileIn.close();
    QStringList  line_list;
    foreach (QString line, raw_corpus) {
        QString     this_line = "";
        QList<int>  breakpoint_list;
        line = line.replace(".", " .");
        line = line.replace('?', " ?");
        get_original_corpus()->append(line);
        line_list = line.split(' ', QString::SkipEmptyParts);
        if ( line_list.length() <=  1 ) {
            continue;
        }
        qDebug() << line_list;
        foreach (QString word, line_list){
            m_NumberOfTrueRunningWords += 1;
            if (!m_TrueDictionary.contains(word) ){
                    m_TrueDictionary[word] = 1;
            }else{
                m_TrueDictionary[word] += 1;
            }
            this_line += word;
            breakpoint_list.append( this_line.length() );
        }
        get_corpus()->append(this_line);
        //qDebug() << 171 << this_line;
        foreach (QString letter,   line) {
            if (! m_EntryDict->contains(letter) ) {
                Entry* this_lexicon_entry = new Entry();
                this_lexicon_entry->set_key( letter );
                this_lexicon_entry->set_count(1);
                m_EntryDict->insert(letter, this_lexicon_entry);
            } else {
                m_EntryDict->value(letter)->increment_count(1);
            }
            if (! m_LetterDict.contains(letter) ){
                m_LetterDict[letter] =  1.0 ;
            }else{
                m_LetterDict[letter] += 1;
            }
        }
        if (numberoflines > 0 and  get_corpus()->length() > numberoflines){
            break;
        }
        m_BreakPointList.append(breakpoint_list);
    }
    m_SizeOfLongestEntry = 1; // Why???
    compute_dict_frequencies();
    QModelIndex index1 = m_wordbreaker->m_corpus_model->index(0,0);
    QModelIndex index2 = m_wordbreaker->m_corpus_model->index(get_corpus()->length(),0);
    m_wordbreaker->m_corpus_model->emit dataChanged(index1, index2);
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

void Lexicon::parse_corpus(int current_iteration) {
       m_wordbreaker->m_main_window->m_progress_bar_1->setMinimum(0);
       m_wordbreaker->m_main_window->m_progress_bar_1->setMaximum(get_corpus()->length());

       m_ParsedCorpus.clear();
       m_parsed_corpus_display.clear();
       m_CorpusCost = 0.0;
       m_NumberOfHypothesizedRunningWords = 0;
       //total_word_count_in_parse = 0;
       foreach(Entry* entry, m_EntryDict->values() ){
           entry->reset_counts(current_iteration);
       }
       QPair<QStringList*,double > pair;
       int lineno = 0;
       foreach (QString line, *get_corpus()){
           lineno++;
           m_wordbreaker->m_main_window->m_progress_bar_1->setValue(lineno);
           pair = parse_word(line);
           QStringList* parsed_line (pair.first);
           double bit_cost = pair.second;
           m_ParsedCorpus.append(parsed_line);
           m_parsed_corpus_display.append(parsed_line->join(" "));
           //qDebug() << "parsed corpus" << parsed_line->join("-");
           m_CorpusCost += bit_cost;
           foreach (QString word, *parsed_line){
               m_EntryDict->value(word)->increment_count(1);
               m_NumberOfHypothesizedRunningWords += 1;
           }
       }
       FilterZeroCountEntries(current_iteration);
       compute_dict_frequencies();
       compute_dictionary_length();

       QModelIndex index1 = m_wordbreaker->m_parsed_corpus_model->index(0,0);
       QModelIndex index2 = m_wordbreaker->m_parsed_corpus_model->index(get_parsed_corpus_display()->length(),0);
       m_wordbreaker->m_parsed_corpus_model->emit dataChanged(index1, index2);

       //if (m_wordbreaker->m_word_table_model) {delete m_wordbreaker->m_word_table_model;}
       //m_wordbreaker->m_word_table_model = new TableModel(& m_EntryDict);
       //m_wordbreaker->m_main_window->get_wordlist_tableview()->setModel(m_wordbreaker->m_word_table_model);


        /*
        foreach (QString word, m_EntryDict->keys()){
            QTableWidgetItem* item = new QTableWidgetItem(word);
            widget->setItem(row,0,item);
            QTableWidgetItem* item2 = new QTableWidgetItem(QString::number(m_EntryDict->value(word)->get_count()));
            widget->setItem(row,1,item2);
            row += 1;
        }
        */
       /*
       m_EntryList = new QList<QPair<QString, int> >;
       foreach (QString word, m_EntryDict->keys()){
           m_EntryList->append(QPair <QString, int>(word,  m_EntryDict->value(word)->get_count() ) ) ;
       }
       if (!m_wordbreaker->m_word_table_model) {
           m_wordbreaker->m_word_table_model = new TableModel(m_EntryList);
       }  else{
            m_wordbreaker->m_word_table_model->refresh_list(m_EntryList);
       }
       //m_wordbreaker->m_word_table_model->refresh_1(m_EntryDict);
    m_wordbreaker->m_main_window->m_wordlist_tableview ->setModel(m_wordbreaker->m_word_table_model);
    m_wordbreaker->m_word_table_model->refresh_list(m_EntryList);
    */
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

   /*
   if (verboseflag){
           print >>outfile, "\nOuter\tInner"
           print >>outfile, "scan:\tscan:\tPiece\tFound?"
    }
    */
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
    //qDebug() << "generate candidates";
    foreach(QStringList* parsed_line,  m_ParsedCorpus){
        for(int wordno = 0; wordno < parsed_line->length()-1; wordno++){
            QString candidate = parsed_line->at(wordno) + parsed_line->at(wordno + 1);
            if  ( m_EntryDict->contains(candidate) ) {
                continue;
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
        //qDebug() << nominee;
    }
    std::sort (NomineeCountList.begin(), NomineeCountList.end(), myLessThan);
    foreach (StringCount nominee, NomineeCountList){
        if (m_DeletionDict.contains(nominee.first)) {
            continue;
        }
        NomineeList.append(nominee);
        if (NomineeList.count() == how_many){
            break;
        }
    }
    //print "Nominees:"
    QStringList latex_data;
    latex_data.append(QString("piece   count   status"));
    foreach(StringCount nominee, NomineeList ){
        add_entry(nominee);
        //print "%20s   %8i" %(nominee, count)
        //latex_data.append(nominee.first +  "\t" + "{:,}".format(count) )
    }
    //MakeLatexTable(latex_data,outfile);
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
        for (int linenumber = 0; linenumber < m_BreakPointList.length(); linenumber++){
            QList<int> truth = m_BreakPointList[linenumber];
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
            qDebug() << 525 << this_line;
            foreach (QString piece,  *m_ParsedCorpus[linenumber]) {
                hypothesis_line_length +=  piece.length();
                hypothesis.append(hypothesis_line_length);
                temp += piece + " ";
            }
            qDebug() << temp;

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
                                    pointer = truth.takeFirst();
                                    if (truth.length() == 0){
                                        qDebug() << 504 << "Problem";
                                    }
                                }
                                else
                                {
                                    pointer = hypothesis.takeFirst();
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
                              pointer = hypothesis.takeFirst();

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
                            pointer = truth.takeFirst();
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

            qDebug() << "Precision" << QString::number(precision) << "Recall:" << QString::number(recall);

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
                if ( m_TrueDictionary.contains(word) ) {
                     int true_count = m_TrueDictionary[word];
                     these_true_positives = qMin(true_count, m_TrueDictionary[word] );
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
                if ( m_TrueDictionary.contains(word) ) {
                    true_positives +=1;
                }
            }
            double word_recall = double (true_positives) / double (m_TrueDictionary.count() );
            double word_precision = double (true_positives) /  m_EntryDict->count();
            m_Type_based_RecallPrecisionHistory.append( QPair<double,double>( QPair<double,double>(  word_precision, word_recall ) ) );
            // print >>outfile, "\n\n***\n"
            // print "Type_based Word Precision  %6.4f; Word Recall  %6.4f" %(word_precision ,word_recall)
            // print >>outfile, formatstring %( "Type_based Word Precision", word_precision, "recall", word_recall)
            // print            formatstring %( "Type_based Word Precision", word_precision, "recall", word_recall)
        }
        }

}
