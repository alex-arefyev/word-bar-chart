#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <QStringList>

class LineParser
{
public:
    LineParser();

    QStringList parseLine(QString const& line) const;

private:
    QRegExp mPattern;
};

#endif // LINEPARSER_H
