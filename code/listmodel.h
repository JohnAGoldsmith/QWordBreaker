#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QObject>
#include <QAbstractListModel>



class ListModel : public QAbstractListModel
{
    QStringList * m_list;

public:
                ListModel(QStringList * list, QObject * parent = nullptr);



    int         rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant    data(const QModelIndex &index, int role = Qt::DisplayRole) const;


};

#endif // LISTMODEL_H
