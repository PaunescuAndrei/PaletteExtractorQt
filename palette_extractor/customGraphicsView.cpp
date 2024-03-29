#include "customGraphicsView.h"
#include <QFrame>
#include <Qt>
#include <QPainter>
#include "utils.h"
#include <QScrollBar>
#include <QMimeData>
#include <QDir>

customGraphicsView::customGraphicsView(QWidget* parent, QString imgpath) : QGraphicsView(parent) {
    this->setStyleSheet("background: transparent");
    this->setFrameShape(QFrame::Shape(0));
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->verticalScrollBar()->installEventFilter(this);
    this->horizontalScrollBar()->installEventFilter(this);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->scene = new QGraphicsScene(this);
    this->setAlignment(Qt::AlignCenter);
    this->setScene(scene);
    this->pixmap_item = new QGraphicsPixmapItem();
    this->pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene->addItem(this->pixmap_item);
    this->imgpath = imgpath;
    if (!imgpath.isEmpty()) {
        this->original_pixmap = openImage(this->imgpath);
    }
    this->setAcceptDrops(true);
}
customGraphicsView::~customGraphicsView() {
    delete this->pixmap_item;
    delete this->scene;
}
void customGraphicsView::resizeEvent(QResizeEvent* e) {
    if (!this->original_pixmap.isNull()) {
        this->resizeImage();
    }
    else {
        QGraphicsView::resizeEvent(e);
    }
}
void customGraphicsView::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->accept();
    }
    else {
        e->ignore();
    }
}
void customGraphicsView::dragMoveEvent(QDragMoveEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::CopyAction);
        e->accept();
    }
    else {
        e->ignore();
    }
}
void customGraphicsView::dropEvent(QDropEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::CopyAction);
        e->accept();
        QList<QUrl> urls = e->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString newimgpath = QDir::toNativeSeparators(urls.first().toLocalFile());
            emit droppedFile(newimgpath);
        }
    }
    else {
        e->ignore();
    }
}
void customGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseClicked(event->button());
    QGraphicsView::mouseReleaseEvent(event);
}
void customGraphicsView::setPixmap(QPixmap pixmap) {
    this->pixmap_item->setPixmap(pixmap);
    this->setSceneRect(this->pixmap_item->sceneBoundingRect());
    //this->horizontalScrollBar()->setValue((pixmap.width() - this->width()) / 2);
    this->fitInView(this->pixmap_item, Qt::KeepAspectRatio);
}
QPixmap customGraphicsView::getResizedPixmap(QPixmap pixmap) {
    return pixmap.scaled(pixmap.width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
QPixmap customGraphicsView::getResizedPixmapFast(QPixmap pixmap) {
    return pixmap.scaled(pixmap.width(), this->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
}
QPixmap customGraphicsView::getResizedPixmap(QPixmap* pixmap) {
    return pixmap->scaled(pixmap->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
QPixmap customGraphicsView::getResizedPixmapFast(QPixmap* pixmap) {
    return pixmap->scaled(pixmap->width(), this->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
}

void customGraphicsView::flipPixmap() {
    this->pixmap_item->setPixmap(this->pixmap_item->pixmap().transformed(QTransform().scale(-1, 1)));
    this->setSceneRect(this->pixmap_item->sceneBoundingRect());
    //this->horizontalScrollBar()->setValue((this->pixmap_item->pixmap().width() - this->width()) / 2);
    this->fitInView(this->pixmap_item, Qt::KeepAspectRatio);
}
void customGraphicsView::setImage(QString path, bool flip) {
    this->imgpath = path;
    //delete this->original_pixmap;
    QPixmap img = openImage(path);
    if (flip) {
        img = img.transformed(QTransform().scale(-1, 1));
    }
    this->original_pixmap = img;
    this->resizeImage();
}


void customGraphicsView::setImage(QPixmap pixmap, bool flip) {
    //delete this->original_pixmap;
    if (flip) {
        pixmap = pixmap.transformed(QTransform().scale(-1, 1));
    }
    this->original_pixmap = pixmap;
    this->resizeImage();
}

void customGraphicsView::setImage(QString path, QPixmap pixmap, bool flip) {
    this->imgpath = path;
    this->setImage(pixmap, flip);
}

void customGraphicsView::resizeImage() {
    if (!this->original_pixmap.isNull()) {
        this->setPixmap(this->getResizedPixmap(this->original_pixmap));
    }
}

bool customGraphicsView::eventFilter(QObject* object, QEvent* event)
{
    if (object == verticalScrollBar() && event->type() == QEvent::Wheel) {
        return true;
    }
    if (object == horizontalScrollBar() && event->type() == QEvent::Wheel) {
        return true;
    }

    return false;
}