#include "../include/TaskDelegate.hpp"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QStyle>
#include <QDateTime>
#include "../include/TaskData.hpp"
#include <QDebug>

TaskDelegate::TaskDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // 1. Déterminer si c'est une ligne impaire/paire (pour couleur alternée)
    bool isEven = (index.row() % 2) == 0;
    QColor backgroundColor = isEven ? QColor("#e5e5e5") : QColor("#ffffff");

    // 2. Si l'item est sélectionné, utiliser la couleur de sélection
    if (option.state & QStyle::State_Selected)
        backgroundColor = option.palette.highlight().color();

    // 3. Dessiner le fond
    painter->fillRect(option.rect, backgroundColor);

    // 4. Dessiner une bordure subtile
    painter->setPen(QColor("#e0e0e0"));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    // 5. Dessiner le texte (récupéré du modèle)

    QString text = index.data(TaskRoles::NameRole).toString();
    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 10);
    textRect.setRight(textRect.right() - 10);

    // Couleur du texte

    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.highlightedText().color());
    else
        painter->setPen(QColor("#333333"));

    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    bool isCompleted = index.data(TaskRoles::CompletedRole).toBool();
    
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