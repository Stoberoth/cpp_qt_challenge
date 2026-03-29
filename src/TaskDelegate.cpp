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
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 10);

    // Couleur du texte
    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.highlightedText().color());
    else
        painter->setPen(QColor("#333333"));

    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    // 6. (Optionnel) Dessiner une checkbox si la tâche est complétée
    bool isCompleted = index.data(Qt::UserRole).toBool();
    if (isCompleted)
    {
        QRect checkboxRect(option.rect.right() - 30, option.rect.top() + 5, 20, 20);
        painter->drawText(checkboxRect, Qt::AlignCenter, "✓");
    }
    // Restaurer l'état
    painter->restore();
}

QSize TaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 40);
}