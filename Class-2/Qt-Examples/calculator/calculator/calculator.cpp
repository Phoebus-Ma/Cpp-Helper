/**
 * calculator.cpp - Calculator.
 */

#include "calculator.h"
#include "button.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QtMath>
#include <QAction>


/**
 * Calculator - Calculator constructor.
 */
Calculator::
Calculator(QWidget *parent)
    : QWidget(parent), sumInMemory(0.0), sumSoFar(0.0), factorSoFar(0.0), waitingForOperand(true)
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    for (int i = 0; i < NumDigitButtons; ++i)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));

    Button *pointButton         = createButton(tr("."),         SLOT(pointClicked()));
    Button *changeSignButton    = createButton(tr("\302\261"),  SLOT(changeSignClicked()));

    Button *backspaceButton     = createButton(tr("Backspace"), SLOT(backspaceClicked()));
    Button *clearButton         = createButton(tr("Clear"),     SLOT(clear()));
    Button *clearAllButton      = createButton(tr("Clear All"), SLOT(clearAll()));

    Button *clearMemoryButton   = createButton(tr("MC"),        SLOT(clearMemory()));
    Button *readMemoryButton    = createButton(tr("MR"),        SLOT(readMemory()));
    Button *setMemoryButton     = createButton(tr("MS"),        SLOT(setMemory()));
    Button *addToMemoryButton   = createButton(tr("M+"),        SLOT(addToMemory()));

    Button *divisionButton      = createButton(tr("\303\267"),  SLOT(multiplicativeOperatorClicked()));
    Button *timesButton         = createButton(tr("\303\227"),  SLOT(multiplicativeOperatorClicked()));
    Button *minusButton         = createButton(tr("-"),         SLOT(additiveOperatorClicked()));
    Button *plusButton          = createButton(tr("+"),         SLOT(additiveOperatorClicked()));

    Button *squareRootButton    = createButton(tr("Sqrt"),      SLOT(unaryOperatorClicked()));
    Button *powerButton         = createButton(tr("x\302\262"), SLOT(unaryOperatorClicked()));
    Button *reciprocalButton    = createButton(tr("1/x"),       SLOT(unaryOperatorClicked()));
    Button *equalButton         = createButton(tr("="),         SLOT(equalClicked()));

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(display,          0, 0, 1, 6);
    mainLayout->addWidget(backspaceButton,  1, 0, 1, 2);
    mainLayout->addWidget(clearButton,      1, 2, 1, 2);
    mainLayout->addWidget(clearAllButton,   1, 4, 1, 2);

    mainLayout->addWidget(clearMemoryButton, 2, 0);
    mainLayout->addWidget(readMemoryButton,  3, 0);
    mainLayout->addWidget(setMemoryButton,   4, 0);
    mainLayout->addWidget(addToMemoryButton, 5, 0);

    for (int i = 1; i < NumDigitButtons; ++i)
    {
        int row    = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;

        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0],  5, 1);
    mainLayout->addWidget(pointButton,      5, 2);
    mainLayout->addWidget(changeSignButton, 5, 3);

    mainLayout->addWidget(divisionButton,   2, 4);
    mainLayout->addWidget(timesButton,      3, 4);
    mainLayout->addWidget(minusButton,      4, 4);
    mainLayout->addWidget(plusButton,       5, 4);

    mainLayout->addWidget(squareRootButton, 2, 5);
    mainLayout->addWidget(powerButton,      3, 5);
    mainLayout->addWidget(reciprocalButton, 4, 5);
    mainLayout->addWidget(equalButton,      5, 5);

    setLayout(mainLayout);

    // Ctrl + Q to quit application.
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    addAction(quitAction);

    setWindowTitle(tr("Calculator"));
}

/**
 * digitClicked - Handle numeric press events.
 */
void Calculator::
digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue        = clickedButton->text().toInt();

    if (("0" == display->text()) && (0.0 == digitValue))
        return;

    if (waitingForOperand)
    {
        display->clear();
        waitingForOperand = false;
    }

    display->setText(display->text() + QString::number(digitValue));
}

/**
 * unaryOperatorClicked - Unary operator clicked.
 */
void Calculator::
unaryOperatorClicked()
{
    double result           = 0.0;
    double operand          = display->text().toDouble();
    Button *clickedButton   = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();

    if (tr("Sqrt") == clickedOperator)
    {
        if (operand < 0.0)
        {
            abortOperation();
            return;
        }

        result = std::sqrt(operand);
    }
    else if (tr("x\302\262") == clickedOperator)
    {
        result = std::pow(operand, 2.0);
    }
    else if (tr("1/x") == clickedOperator)
    {
        if (0.0 == operand)
        {
            abortOperation();
            return;
        }

        result = 1.0 / operand;
    }

    display->setText(QString::number(result));
    waitingForOperand = true;
}

/**
 * additiveOperatorClicked - Additive Operator Clicked.
 */
void Calculator::
additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());

    if (!clickedButton)
        return;

    QString clickedOperator = clickedButton->text();
    double operand          = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty())
    {
        if (!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }

        display->setText(QString::number(factorSoFar));

        operand     = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty())
    {
        if (!calculate(operand, pendingAdditiveOperator))
        {
            abortOperation();
            return;
        }

        display->setText(QString::number(sumSoFar));
    }
    else
    {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand       = true;
}

/**
 * multiplicativeOperatorClicked - Multiplicative Operator Clicked.
 */
void Calculator::
multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());

    if (!clickedButton)
        return;

    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty())
    {
        if (!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }

        display->setText(QString::number(factorSoFar));
    }
    else
    {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}

/**
 * equalClicked - Equal Clicked.
 */
void Calculator::
equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty())
    {
        if (!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }

        operand     = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty())
    {
        if (!calculate(operand, pendingAdditiveOperator))
        {
            abortOperation();
            return;
        }

        pendingAdditiveOperator.clear();
    }
    else
    {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));

    sumSoFar = 0.0;
    waitingForOperand = true;
}

/**
 * pointClicked - Dot clicked.
 */
void Calculator::
pointClicked()
{
    if (waitingForOperand)
        display->setText("0");

    if (!display->text().contains('.'))
        display->setText(display->text() + tr("."));

    waitingForOperand = false;
}

/**
 * changeSignClicked - Change sign clicked.
 */
void Calculator::
changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0)
    {
        text.prepend(tr("-"));
    }
    else if (value < 0.0)
    {
        text.remove(0, 1);
    }

    display->setText(text);
}

/**
 * backspaceClicked - Delete the last digit.
 */
void Calculator::
backspaceClicked()
{
    if (waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);

    if (text.isEmpty())
    {
        text = "0";
        waitingForOperand = true;
    }

    display->setText(text);
}

/**
 * clear - Clear current line data.
 */
void Calculator::
clear()
{
    if (waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}

/**
 * clearAll - Clear all data.
 */
void Calculator::
clearAll()
{
    sumSoFar    = 0.0;
    factorSoFar = 0.0;

    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();

    display->setText("0");
    waitingForOperand = true;
}

/**
 * clearMemory - Clear memory.
 */
void Calculator::
clearMemory()
{
    sumInMemory = 0.0;
}

/**
 * readMemory - Read memory.
 */
void Calculator::
readMemory()
{
    display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

/**
 * setMemory - Set memory.
 */
void Calculator::
setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

/**
 * addToMemory - Add to memory.
 */
void Calculator::
addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}

/**
 * createButton - Create button.
 */
Button *Calculator::
createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);

    return button;
}

/**
 * abortOperation - Abort operation.
 */
void Calculator::
abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}

/**
 * calculate - Mathematical calculation.
 */
bool Calculator::
calculate(double rightOperand, const QString &pendingOperator)
{
    if (tr("+") == pendingOperator)
    {
        sumSoFar += rightOperand;
    }
    else if (tr("-") == pendingOperator)
    {
        sumSoFar -= rightOperand;
    }
    else if (tr("\303\227") == pendingOperator)
    {
        factorSoFar *= rightOperand;
    }
    else if (tr("\303\267") == pendingOperator)
    {
        if (0.0 == rightOperand)
            return false;

        factorSoFar /= rightOperand;
    }

    return true;
}
