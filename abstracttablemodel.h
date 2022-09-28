#ifndef ABSTRACTTABLEMODEL_H
#define ABSTRACTTABLEMODEL_H
#include <QAbstractTableModel>


class Entry;

class AbstractTableModel   : public QAbstractTableModel
{
    QMap<QString, Entry*>  *           m_entries;


public:
    AbstractTableModel();
    int rowCount(){return m_entries->size();}
    int columnCount() {return 2;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole);

};

#endif // ABSTRACTTABLEMODEL_H
