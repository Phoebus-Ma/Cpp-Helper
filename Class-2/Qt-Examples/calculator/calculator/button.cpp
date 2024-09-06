/**
 * button.cpp - Keying matrix.
 */

#include "button.h"


/**
 * Button - Button constructor.
*/
Button::
Button(const QString &text, QWidget *parent)
    : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);
}

/**
 * sizeHint - Size hint.
*/
QSize Button::
sizeHint() const
{
    QSize size = QToolButton::sizeHint();

    size.rheight() += 20;
    size.rwidth()  = qMax(size.width(), size.height());

    return size;
}
