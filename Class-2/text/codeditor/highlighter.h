/**
 * highlighter.h
 *
 * Description:
 *      This module implements code editor header file.
 */

#ifndef __HIGHLIGHTER_H__
#define __HIGHLIGHTER_H__

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>


#define USE_DARK_MODERN

#ifdef USE_DARK_MODERN  /* Dark theme */
#define CONTENT_KEYWORD_FORMAT      Qt::magenta
#define TYPE_KEYWORD_FORMAT         Qt::darkBlue
#define CLASS_FORMAT                Qt::darkMagenta
#define SINGLE_COMMENT_FORMAT       Qt::darkGreen
#define MULTI_COMMENT_FORMAT        Qt::darkGreen
#define QUOTATION_FORMAT            Qt::darkRed
#define FUNCTION_FORMAT             Qt::blue
#else   /* Light theme. */
#define CONTENT_KEYWORD_FORMAT      Qt::magenta
#define TYPE_KEYWORD_FORMAT         Qt::darkBlue
#define CLASS_FORMAT                Qt::darkMagenta
#define SINGLE_COMMENT_FORMAT       Qt::red
#define MULTI_COMMENT_FORMAT        Qt::red
#define QUOTATION_FORMAT            Qt::darkGreen
#define FUNCTION_FORMAT             Qt::blue
#endif  /* USE_DARK_MODERN */


QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat contentKeywordFormat;
    QTextCharFormat typeKeywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif /* __HIGHLIGHTER_H__ */
