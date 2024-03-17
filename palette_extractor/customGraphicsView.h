#pragma once
#include <QGraphicsView>
#include <QEvent>
#include <QGraphicsItem>
#include <QPixmap>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>

class customGraphicsView :
    public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsPixmapItem* pixmap_item;
    QGraphicsScene* scene;
    QPixmap original_pixmap = QPixmap();
    QString imgpath;
    customGraphicsView(QWidget* parent = nullptr, QString imgpath = "");
    ~customGraphicsView();
    void resizeEvent(QResizeEvent* e) override;
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void setPixmap(QPixmap pixmap);
    QPixmap getResizedPixmap(QPixmap pixmap);
    QPixmap getResizedPixmapFast(QPixmap pixmap);
    QPixmap getResizedPixmap(QPixmap* pixmap);
    QPixmap getResizedPixmapFast(QPixmap* pixmap);
    void flipPixmap();
    void setImage(QString path, bool flip = false);
    void setImage(QPixmap pixmap, bool flip = false);
    void setImage(QString path, QPixmap pixmap, bool flip = false);
    void resizeImage();
    bool eventFilter(QObject* object, QEvent* event);
signals:
    void droppedFile(QString path);
    void mouseClicked(Qt::MouseButton button);
};
