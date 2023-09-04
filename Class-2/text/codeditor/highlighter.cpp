/**
 * highlighter.cpp
 *
 * Description:
 *      This module implements code editor.
 */

#include "highlighter.h"

/**
 * Highlighter - Highlighter constructor.
 */
Highlighter::
Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    const QString contentKeyword[] = {
        QStringLiteral("\\bfor\\b"),            // for
        QStringLiteral("\\bgoto\\b"),           // goto
        QStringLiteral("\\bif\\b"),             // if
        QStringLiteral("\\belse\\b"),           // else
        QStringLiteral("\\breturn\\b"),         // return
        QStringLiteral("\\bwhile\\b"),          // while

        QStringLiteral("\\bdefine\\b"),         // define
        QStringLiteral("\\belif\\b"),           // elif
        QStringLiteral("\\bendif\\b"),          // endif
        QStringLiteral("\\bifdef\\b"),          // ifdef
        QStringLiteral("\\bifndef\\b"),         // ifndef
        QStringLiteral("\\binclude\\b"),        // include
    };

    const QString typeKeyword[] = {
        QStringLiteral("\\bbool\\b"),           // bool
        QStringLiteral("\\bchar\\b"),           // char
        QStringLiteral("\\bclass\\b"),          // class
        QStringLiteral("\\bconst\\b"),          // const
        QStringLiteral("\\bdouble\\b"),         // double
        QStringLiteral("\\benum\\b"),           // enum
        QStringLiteral("\\bexplicit\\b"),       // explicit
        QStringLiteral("\\bfloat\\b"),          // float
        QStringLiteral("\\bfriend\\b"),         // friend
        QStringLiteral("\\binline\\b"),         // inline
        QStringLiteral("\\bint\\b"),            // int
        QStringLiteral("\\blong\\b"),           // long
        QStringLiteral("\\bnamespace\\b"),      // namespace
        QStringLiteral("\\boperator\\b"),       // operator
        QStringLiteral("\\bprivate\\b"),        // private
        QStringLiteral("\\bprotected\\b"),      // protected
        QStringLiteral("\\bpublic\\b"),         // public
        QStringLiteral("\\bsignals\\b"),        // signals
        QStringLiteral("\\bsigned\\b"),         // signed
        QStringLiteral("\\bshort\\b"),          // short
        QStringLiteral("\\bslots\\b"),          // slots
        QStringLiteral("\\bstatic\\b"),         // static
        QStringLiteral("\\bstruct\\b"),         // struct
        QStringLiteral("\\btemplate\\b"),       // template
        QStringLiteral("\\btypedef\\b"),        // typedef
        QStringLiteral("\\btypename\\b"),       // typename
        QStringLiteral("\\bunsigned\\b"),       // unsigned
        QStringLiteral("\\bvirtual\\b"),        // virtual
        QStringLiteral("\\bunion\\b"),          // union
        QStringLiteral("\\bvoid\\b"),           // void
        QStringLiteral("\\bvolatile\\b"),       // volatile
    };

    // Content keyword format.
    contentKeywordFormat.setForeground(CONTENT_KEYWORD_FORMAT);
    contentKeywordFormat.setFontWeight(QFont::Bold);

    for (const QString &pattern : contentKeyword)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = contentKeywordFormat;
        highlightingRules.append(rule);
    }

    // Type keyword format.
    typeKeywordFormat.setForeground(TYPE_KEYWORD_FORMAT);
    typeKeywordFormat.setFontWeight(QFont::Bold);

    for (const QString &pattern : typeKeyword)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = typeKeywordFormat;
        highlightingRules.append(rule);
    }

    // Class format.
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(CLASS_FORMAT);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format  = classFormat;
    highlightingRules.append(rule);

    // Single line comment format.
    singleLineCommentFormat.setForeground(SINGLE_COMMENT_FORMAT);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format  = singleLineCommentFormat;
    highlightingRules.append(rule);

    // Multi line comment format.
    multiLineCommentFormat.setForeground(MULTI_COMMENT_FORMAT);

    // Quotation (\"\") format.
    quotationFormat.setForeground(QUOTATION_FORMAT);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format  = quotationFormat;
    highlightingRules.append(rule);

    // Function format.
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(FUNCTION_FORMAT);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format  = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression   = QRegularExpression(QStringLiteral("\\*/"));
}

/**
 * highlightBlock - Format the file code according to the set formatting rules.
 */
void Highlighter::
highlightBlock(const QString &text)
{
    int startIndex = 0;

    for (const HighlightingRule &rule : qAsConst(highlightingRules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;

        if (-1 == endIndex)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
