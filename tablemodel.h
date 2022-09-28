#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include "wordbreaker.h"

class Entry;

class TableModel : public QAbstractTableModel
{

    Q_OBJECT

    QList< string_count * > * m_list;



public:

    TableModel( QList< string_count* > * , QObject * parent = nullptr);
    ~TableModel();
    void        refresh_list(QList< string_count* > * list);
    int         get_count() {return m_list->length();}
    int         rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int         columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant    data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void        call_refresh();
   // bool        setData(const QModelIndex & index, const QVariant & value, int role) override;

};

#endif // TABLEMODEL_H
