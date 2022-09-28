#include <QAbstractTableModel>
#include "abstracttablemodel.h"
#include "lexicon.h"
#include "wordbreaker.h"

AbstractTableModel::AbstractTableModel():   QAbstractTableModel()
{


}
QVariant data(const QModelIndex &index, int role = Qt::DisplayRole){
    int row = index.row();

}
