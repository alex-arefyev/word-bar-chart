#include "lineparser.h"

LineParser::LineParser()
    : mPattern("\\b[^\\d\\W]+\\b")
{}

QStringList LineParser::parseLine(QString const& line) const
{
    QStringList list;
    int pos = 0;
    while ((pos = mPattern.indexIn(line, pos)) != -1) {
        list << mPattern.capturedTexts();
        pos += mPattern.matchedLength();
    }
    return list;
}
