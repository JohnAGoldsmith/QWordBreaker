#include <QModelIndex>
#include "lexicon.h"
#include "tablemodel.h"
#include "wordbreaker.h"

TableModel::TableModel(QList< string_count * > * list, QObject * parent ) {
    m_list = list;

}
TableModel::~TableModel(){
    if (m_list){
        foreach (string_count * sc, *m_list){
            delete sc;
        }
    }
}
int TableModel::rowCount(const QModelIndex &parent) const{
    return m_list->count();
}
int TableModel::columnCount(const QModelIndex &parent) const{
    return 2;
}
QVariant TableModel::data(const QModelIndex &index, int role ) const{
    if (!index.isValid()) { return QVariant(); }
    if (index.row() >= m_list->size() || index.row() < 0)
        return QVariant();
    if (role == Qt::DisplayRole) {
        switch (index.column()){
           case 0:
                return QVariant(m_list->value(index.row())->m_string);
           case 1:
                 return QVariant(m_list->value(index.row())->m_count);
        }
     }
     if (role == Qt::FontRole){
        QFont font;
        font.setBold(true);
        return font;
     }
     if (role==Qt::ForegroundRole  ){
        QBrush brush;
        brush.setColor(Qt::GlobalColor(Qt::darkBlue));
        return brush;
     }
     if (role==Qt::CheckStateRole  && index.column() == 0){
            return Qt::Unchecked;
     }
     return QVariant();
}
void  TableModel::refresh_list(QList< string_count* > * list)
{
        m_list = list;
}
void TableModel::call_refresh(){
   emit dataChanged(QModelIndex(), QModelIndex(),   QVector<int>() );
}
/*
bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role == Qt::CheckStateRole && index.column() == 0) {
        //QAb  *item = static_cast<TreeItem*>(index.internalPointer());
        QTreeWidgetItem *check = static_cast<QTreeWidgetItem*>(index.internalPointer());

        if (value == Qt::Checked) {
            checkedState_ = Qt::Checked;
        }
        else {
            checkedState_ = Qt::Unchecked;
        }
    }
    emit dataChanged(index, index);
    return true;
}
*/
