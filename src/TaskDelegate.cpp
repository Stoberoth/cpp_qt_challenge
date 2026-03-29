#include "../include/TaskDelegate.hpp"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QStyle>

TaskDelegate::TaskDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    bool isEven = (index.row() % 2) == 0;
    QColor backgroundColor = isEven ? QColor("#e5e5e5") : QColor("#ffffff");

    if (option.state & QStyle::State_Selected)
        backgroundColor = option.palette.highlight().color();

    painter->fillRect(option.rect, backgroundColor);
    painter->setPen(QColor("#e0e0e0"));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 10);

    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.highlightedText().color());
    else
        painter->setPen(QColor("#333333"));

    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    bool isCompleted = index.data(Qt::UserRole).toBool();
    if (isCompleted)
    {
        QRect checkboxRect(option.rect.right() - 30, option.rect.top() + 5, 20, 20);
        painter->drawText(checkboxRect, Qt::AlignCenter, "✓");
    }
    painter->restore();
}

QSize TaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 40);
}