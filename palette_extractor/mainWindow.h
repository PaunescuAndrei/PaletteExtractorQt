#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_palette_extractor.h"
#include <QDropEvent>
#include <QString>
#include <QPixmap>

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWidget* colors = nullptr;
    mainWindow(QWidget *parent = nullptr);
    void clipboardPaste();
    void setImg(QString path);
    void setImg(QPixmap pixmap);
    void processImg();
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
    ~mainWindow();

private:
    Ui::palette_extractorClass ui;
};
