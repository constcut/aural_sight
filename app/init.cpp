#include "init.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "log.hpp"
#include <QFontDatabase>
#include <QDir>

#include "app/loghandler.h"

#include "music/graphicmap.h"
#include "music/midiengine.h"
#include "music/midirender.h"
#include "music/midifile.h"

#include <QTextCodec>

int mainInit(int argc, char *argv[]) {

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    Q_INIT_RESOURCE(soundfonts);
    QFile::copy(":/soundfonts/instrument.sf2","instrument.sf2");

    qmlRegisterType<ConsoleLogQML>("mther.app",1,0,"ConsoleLog");
    qmlRegisterType<PianoQML>("mther.app",1,0,"Piano");

    QGuiApplication app(argc, argv);
    Q_INIT_RESOURCE(fonts);
    qDebug() << "Current working path "<<QDir::currentPath();
    int fontId = QFontDatabase::addApplicationFont(":/fonts/prefont.ttf");
    QStringList famList = QFontDatabase::applicationFontFamilies(fontId) ;
    qDebug() << famList << " font families for id "<<fontId;
    if (famList.isEmpty() == false)
        app.setFont(QFont(famList[0], 11, QFont::Normal, false));
    else
        qWarning() << "Failed to load font";

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251")); //Настройки //KOI8-R //ISO 8859-5 //UTF-8 //Windows-1251
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int res = app.exec();
    return res;
}
