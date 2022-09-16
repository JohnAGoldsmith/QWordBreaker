#include <QDebug>
#include "listmodel.h"


ListModel::ListModel(QStringList * list, QObject * parent)
{
    m_list = list;
}

int ListModel::rowCount(const QModelIndex &parent) const{
    return m_list->length();
}


QVariant ListModel::data(const QModelIndex &index, int role ) const{
    if (role == Qt::DisplayRole){
        //qDebug() << m_list->at(index.row());
        return QVariant( m_list->at( index.row() ) );
    }
    return QVariant();
}

