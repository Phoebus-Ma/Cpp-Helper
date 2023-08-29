/**
 * calculator.h - Calculator.
 */

#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__

#include <QWidget>


QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE
class Button;


class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);

private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();

private:
    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);

    double sumInMemory;
    double sumSoFar;
    double factorSoFar;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;

    QLineEdit *display;

    enum
    {
        NumDigitButtons = 10
    };

    Button *digitButtons[NumDigitButtons];
};

#endif /* __CALCULATOR_H__ */
