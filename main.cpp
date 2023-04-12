#include <QGuiApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "wordbarchart.h"
#include "models/ChartModel.h"
#include <set>

static int getInt(QString const& str, int defVal)
{
    bool ok = false;
    int value = str.toInt(&ok);
    return ok ? value : defVal;
}

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("word-bar-chart");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Word Bar Chart");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption("s", "Sorting", "1"));
    parser.addOption(QCommandLineOption("n", "Bar Number", "15"));
    parser.addOption(QCommandLineOption("d", "Delay", "32"));
    parser.process(app);

    int mode = getInt(parser.value("s"), 0);
    int barNum = getInt(parser.value("n"), 15);
    int delay = getInt(parser.value("d"), 32);

    if (mode != 0) { mode = 1; }
    if (barNum < 3 || barNum > 32) { barNum = 15; }
    if (delay < 32 || delay > 1000) { delay = 0; }

    WordBarChart* chart = new WordBarChart(mode, barNum, delay, &app);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);

    QQmlContext *ctx = engine.rootContext();

    ctx->setContextProperty("WordBarChart", QVariant::fromValue(chart));
    ctx->setContextProperty("chartModel", QVariant::fromValue(chart->model()));

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
