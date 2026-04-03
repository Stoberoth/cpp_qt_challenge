#pragma once

#include <QStyledItemDelegate>

class TaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
   public:
    explicit TaskDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};