/**
 * mainwindow.cpp
 *
 * Description:
 *      This module implements base calendar.
 */

#include "mainwindow.h"


/**
 * MainWindow - Calendar constructor.
 */
MainWindow::
MainWindow()
{
    fontSize     = 10;
    selectedDate = QDate::currentDate();

    QWidget *centralWidget  = new QWidget;
    QLabel *dateLabel       = new QLabel(tr("Date:"));
    QComboBox *monthCombo   = new QComboBox;

#ifdef USE_LANGUAGE_ENGLISH
    locale = QLocale(QLocale::English, QLocale::UnitedStates);
#else
    locale = QLocale();
#endif

    for (int month = 1; month <= 12; ++month)
        monthCombo->addItem(locale.monthName(month));

    QDateTimeEdit *yearEdit = new QDateTimeEdit;
    yearEdit->setDisplayFormat("yyyy");
    // 1753 was the first year in which the calendar was fully adopted.
    yearEdit->setDateRange(QDate(1753, 1, 1), QDate(8000, 1, 1));

    monthCombo->setCurrentIndex(selectedDate.month() - 1);
    yearEdit->setDate(selectedDate);

    QLabel *fontSizeLabel     = new QLabel(tr("Font size:"));
    QSpinBox *fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setRange(1, 64);

    editor = new QTextBrowser;
    insertCalendar();

    connect(monthCombo, QOverload<int>::of(&QComboBox::activated),
            this, &MainWindow::setMonth);
    connect(yearEdit, &QDateTimeEdit::dateChanged,
            this, &MainWindow::setYear);
    connect(fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::setFontSize);

    fontSizeSpinBox->setValue(10);

    // Date control panel.
    QHBoxLayout *controlsLayout = new QHBoxLayout;  // Horizontal layout, only control panel.
    controlsLayout->addWidget(dateLabel);
    controlsLayout->addWidget(yearEdit);
    controlsLayout->addWidget(monthCombo);
    controlsLayout->addSpacing(24);
    controlsLayout->addWidget(fontSizeLabel);
    controlsLayout->addWidget(fontSizeSpinBox);
    controlsLayout->addStretch(1);

    // Display objects.
    QVBoxLayout *centralLayout = new QVBoxLayout;   // Vertical layout.
    centralLayout->addLayout(controlsLayout);       // Date control panel.
    centralLayout->addWidget(editor, 1);            // Date display array.

    // Main layout.
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    // Set current date for title.
    setWindowTitle(tr("%1-%2-%3")
        .arg(selectedDate.year())
        .arg(selectedDate.month())
        .arg(selectedDate.day())
    );
}

/**
 * insertCalendar - Insert data to calendar.
 */
void MainWindow::
insertCalendar()
{
    editor->clear();
    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();

    QDate date(selectedDate.year(), selectedDate.month(), 1);

    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignHCenter);
    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(4);

    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14)
                << QTextLength(QTextLength::PercentageLength, 14);
    tableFormat.setColumnWidthConstraints(constraints);

    // 1 row, 7 columns, 7 day for a week.
    QTextTable *table = cursor.insertTable(1, 7, tableFormat);

    QTextFrame *frame            = cursor.currentFrame();
    QTextFrameFormat frameFormat = frame->frameFormat();
    frameFormat.setBorder(1);
    frame->setFrameFormat(frameFormat);

    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(fontSize);

    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold);

    // Define string background.
    QTextCharFormat highlightedFormat = boldFormat;
    highlightedFormat.setBackground(Qt::yellow);

    // Set week day title.
    for (int weekDay = 1; weekDay <= 7; ++weekDay)
    {
        QTextTableCell cell    = table->cellAt(0, weekDay - 1);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(locale.dayName(weekDay), boldFormat);
    }

    table->insertRows(table->rows(), 1);

    while (date.month() == selectedDate.month())
    {
        int weekDay            = date.dayOfWeek();
        QTextTableCell cell    = table->cellAt(table->rows() - 1, weekDay - 1);
        QTextCursor cellCursor = cell.firstCursorPosition();

        if (date == QDate::currentDate())   // Set current data background.
            cellCursor.insertText(QString("%1").arg(date.day()), highlightedFormat);
        else
            cellCursor.insertText(QString("%1").arg(date.day()), format);

        date = date.addDays(1);

        // Add new line.
        if ((7 == weekDay) && (date.month() == selectedDate.month()))
            table->insertRows(table->rows(), 1);
    }

    cursor.endEditBlock();
}

/**
 * setFontSize - Set font size.
 */
void MainWindow::
setFontSize(int size)
{
    fontSize = size;
    insertCalendar();
}

/**
 * setMonth - Set month.
 */
void MainWindow::
setMonth(int month)
{
    selectedDate = QDate(selectedDate.year(), month + 1, selectedDate.day());
    insertCalendar();
}

/**
 * setYear - Set year.
 */
void MainWindow::
setYear(QDate date)
{
    selectedDate = QDate(date.year(), selectedDate.month(), selectedDate.day());
    insertCalendar();
}
