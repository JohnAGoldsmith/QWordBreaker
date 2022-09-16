#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>

class Entry;

class TableModel : public QAbstractTableModel
{

    Q_OBJECT

    QList< QPair< QString, int > > * m_list;



public:
//    TableModel(QMap<QString, Entry*> *, parent QObject = nullptr );
    TableModel( QList<QPair<QString,int> > * , QObject * parent = nullptr);
    ~TableModel();
    void        refresh_list(QList<QPair<QString,int> > * list);
    int         get_count() {return m_list->length();}
    int         rowCount(const QModelIndex &parent = QModelIndex()) const;
    int         columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant    data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //void        refresh_1(QMap<QString,Entry*> * new_entries);
};

#endif // TABLEMODEL_H
