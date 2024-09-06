/**
 * button.h
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <QToolButton>


class Button : public QToolButton
{
    Q_OBJECT

public:
    explicit Button(const QString &text, QWidget *parent = nullptr);

    QSize sizeHint() const override;
};

#endif /* __BUTTON_H__ */
