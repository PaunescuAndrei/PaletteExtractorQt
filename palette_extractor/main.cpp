#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include <QColor>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Palette_Extractor_MRC");
    a.setOrganizationName("MRC");

    QPalette paletteQt = QPalette();
    paletteQt.setColor(QPalette::Window, QColor(53, 53, 53));
    paletteQt.setColor(QPalette::WindowText, QColorConstants::White);
    paletteQt.setColor(QPalette::Base, QColor(35, 35, 35));
    paletteQt.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    paletteQt.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    paletteQt.setColor(QPalette::ToolTipText, QColorConstants::White);
    paletteQt.setColor(QPalette::Text, QColorConstants::White);
    paletteQt.setColor(QPalette::Button, QColor(53, 53, 53));
    paletteQt.setColor(QPalette::ButtonText, QColorConstants::White);
    paletteQt.setColor(QPalette::BrightText, QColorConstants::Red);
    paletteQt.setColor(QPalette::Link, QColor(42, 130, 218));
    paletteQt.setColor(QPalette::Highlight, QColor(242, 112, 242));
    paletteQt.setColor(QPalette::HighlightedText, QColor(35, 35, 35));
    paletteQt.setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53));
    paletteQt.setColor(QPalette::Disabled, QPalette::ButtonText, QColorConstants::DarkGray);
    paletteQt.setColor(QPalette::Disabled, QPalette::WindowText, QColorConstants::DarkGray);
    paletteQt.setColor(QPalette::Disabled, QPalette::Text, QColorConstants::DarkGray);
    paletteQt.setColor(QPalette::Disabled, QPalette::Light, QColor(53, 53, 53));

    a.setStyle("fusion");
    a.setPalette(paletteQt);
    mainWindow w;
    w.show();
    return a.exec();
}
