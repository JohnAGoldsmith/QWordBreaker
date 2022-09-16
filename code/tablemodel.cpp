#include <QModelIndex>
#include "lexicon.h"
#include "tablemodel.h"


TableModel::TableModel(QList< QPair< QString, int> > * list, QObject * parent ) {
    m_list = list;
}
TableModel::~TableModel(){
    if (m_list){
        delete m_list;
    }
}

int TableModel::rowCount(const QModelIndex &parent) const{
    //qDebug()<< 12  << m_list->count();
    return m_list->count();
}
int TableModel::columnCount(const QModelIndex &parent) const{
    return 2;
}
QVariant TableModel::data(const QModelIndex &index, int role ) const{
    if ( index.isValid() &&     role == Qt::EditRole ) {
        if (index.column() == 0) {
            //qDebug() << 32 << m_list->value(index.row()).first;
            return m_list->value(index.row()).first;
        } else
        { if (index.column() == 1)
            {
                return QVariant(m_list->value(index.row()).second);
            }
        }
    }
}
/*
void TableModel::refresh_1(QMap<QString,Entry*> * entries){
    m_entries =  entries;
    if (m_words) { delete m_words;}
    m_words = new QStringList(entries->keys());
    QModelIndex index1 = index(0,0);
    QModelIndex index2 = index (m_words->length(),1);
    dataChanged(index1, index2);
}
*/
void  TableModel::refresh_list(QList<QPair<QString,int> > * list)
{
        m_list = list;
}
