#include "mainWindow.h"
#include "colorPaletteExtractor.h"
#include <QPushButton>
#include <QPixmap>
#include <QColor>
#include <QLabel>
#include <QMimeData>
#include <QList>
#include <QDir>
#include "customGraphicsView.h"
#include "utils.h"
#include "opencv2/imgproc/types_c.h"
#include "asmOpenCV.h"
#include <QSettings>
#include <QFileDialog>
#include <QShortcut>
#include <QKeySequence>
#include <QMimeData>
#include <QClipboard>
#include <QBuffer>

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setAcceptDrops(true);
    QShortcut* clipboard_paste = new QShortcut(QKeySequence("ctrl+v"), this, [this] {this->clipboardPaste(); });
    if (!this->ui.results->layout()) {
        QVBoxLayout* l = new QVBoxLayout(this->ui.results);
        this->ui.results->setLayout(l);
    }
    connect(this->ui.pushButton, &QPushButton::clicked, this, [this] {
        const QString DEFAULT_DIR_KEY("default_dir");
        QSettings MySettings;

        QString SelectedFile = QFileDialog::getOpenFileName(
            this, "Select a file", MySettings.value(DEFAULT_DIR_KEY).toString());

        if (!SelectedFile.isEmpty()) {
            QDir CurrentDir;
            MySettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(SelectedFile));
            this->setImg(SelectedFile);
        }
    });
    connect(this->ui.originalImg, &customGraphicsView::droppedFile, this, [this](QString img) {this->setImg(img); });
    connect(this->ui.processedImg, &customGraphicsView::droppedFile, this, [this](QString img) {this->setImg(img); });
}

void mainWindow::clipboardPaste(){
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData* md = clipboard->mimeData();
    QImage image = QImage();

    if (md->formats().contains(QStringLiteral("text/uri-list"))) {
        image = QImage(QUrl(clipboard->text()).toLocalFile());
    }
    else {
        image = clipboard->image();
    }
    if (not image.isNull()) {
        this->setImg(QPixmap::fromImage(image));
    }
}

void mainWindow::setImg(QString path)
{
    this->ui.originalImg->setImage(path);
    this->processImg();
}

void mainWindow::setImg(QPixmap pixmap)
{
    this->ui.originalImg->setImage(pixmap);
    this->processImg();
}

//bool cmp(std::pair<cv:vec4b, int>& a,
//    pair<string, int>& b)
//{
//    return a.second < b.second;
//}

void mainWindow::processImg() {
    auto palette = palette_extractor_with_image(this->ui.originalImg->original_pixmap, 10, 333);
    QPixmap img_reduced = ASM::cvMatToQPixmap(palette.second);
    this->ui.processedImg->setImage(img_reduced);
    if (this->colors) {
        this->ui.centralWidget->layout()->removeWidget(this->colors);
        this->colors->deleteLater();
        this->colors = nullptr;
    }
    this->colors = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this->colors);
    layout->setContentsMargins(0, 0, 0, 0);
    this->colors->setLayout(layout);
    int i = 1;
    std::sort(palette.first.first.begin(), palette.first.first.end(), greater_color_area);
    std::sort(palette.first.second.begin(), palette.first.second.end(), greater_color_area);


    color_area selectedColor;
    if (!palette.first.first.isEmpty()) {
        selectedColor = get_weighted_random_color(palette.first.first);
    }else if(!palette.first.second.isEmpty()){
        selectedColor = *select_randomly(palette.first.second.begin(), palette.first.second.end());
    }
    if (selectedColor.color.isValid()) {
        QString area = QString::number(100.f * selectedColor.area_percent, 'f', 2) % "%";
        QColor color_inverted = complementary_color(selectedColor.color);
        float l = selectedColor.color.lightnessF();
        float h = selectedColor.color.hsvHue();
        float s = selectedColor.color.hsvSaturationF();
        int v = selectedColor.color.value();
        QString lightness_label = QString::number(l, 'f', 2);
        QString hue_label = QString::number(h, 'f', 2);
        QString saturation_label = QString::number(s, 'f', 2);
        QString value_label = QString::number(v);
        this->ui.selectedColor->setMinimumHeight(30);
        this->ui.selectedColor->setStyleSheet(QString("background-color:" + selectedColor.color.name() + ";" + "color:" + color_inverted.name() + ";"));
        this->ui.selectedColor->setText(QString::number(i) % ". " % selectedColor.color.name().toUpper() % " " % QString("(%1)").arg(area) % " " % QString("[H:%1 S:%2 L:%3 V:%4]").arg(hue_label, saturation_label, lightness_label, value_label));
        this->ui.selectedColor->setAlignment(Qt::AlignCenter);
        this->ui.selectedColor->setTextInteractionFlags(Qt::TextSelectableByMouse);
    }
    for (auto& c : palette.first.first)
    {
        QString area = QString::number(100.f * c.area_percent,'f',2) % "%";
        QLabel* label = new QLabel();
        QColor color_inverted = complementary_color(c.color);
        float l = c.color.lightnessF();
        float h = c.color.hsvHue();
        float s = c.color.hsvSaturationF();
        int v = c.color.value();
        QString lightness_label = QString::number(l,'f',2);
        QString hue_label = QString::number(h, 'f', 2);
        QString saturation_label = QString::number(s, 'f', 2);
        QString value_label = QString::number(v);
        label->setMinimumHeight(30);
        label->setStyleSheet(QString("background-color:" + c.color.name() + ";" + "color:" + color_inverted.name() + ";"));
        label->setText(QString::number(i) % ". " % c.color.name().toUpper() % " " % QString("(%1)").arg(area) % " " % QString("[H:%1 S:%2 L:%3 V:%4]").arg(hue_label,saturation_label,lightness_label,value_label) );
        label->setAlignment(Qt::AlignCenter);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout->addWidget(label);
        i++;
    }
    for (auto& c : palette.first.second)
    {
        QString area = QString::number(100.f * c.area_percent, 'f', 2) % "%";
        QLabel* label = new QLabel();
        QColor color_inverted = complementary_color(c.color);
        float l = c.color.lightnessF();
        float h = c.color.hsvHue();
        float s = c.color.hsvSaturationF();
        int v = c.color.value();
        QString lightness_label = QString::number(l, 'f', 2);
        QString hue_label = QString::number(h, 'f', 2);
        QString saturation_label = QString::number(s, 'f', 2);
        QString value_label = QString::number(v);
        auto f = label->font();
        f.setStrikeOut(true);
        label->setFont(f);
        label->setMinimumHeight(30);
        label->setStyleSheet(QString("background-color:" + c.color.name() + ";" + "color:" + color_inverted.name() + ";"));
        label->setText(QString::number(i) % ". " % c.color.name().toUpper() % " " % QString("(%1)").arg(area) % " " % QString("[H:%1 S:%2 L:%3 V:%4]").arg(hue_label, saturation_label, lightness_label, value_label));
        label->setAlignment(Qt::AlignCenter);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout->addWidget(label);
        i++;
    }
    this->ui.results->setWidget(this->colors);
    this->ui.results->setWidgetResizable(true);
}

void mainWindow::dragEnterEvent(QDragEnterEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->accept();
    }
    else {
        e->ignore();
    }
}
void mainWindow::dragMoveEvent(QDragMoveEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::CopyAction);
        e->accept();
    }
    else {
        e->ignore();
    }
}
void mainWindow::dropEvent(QDropEvent* e) {
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::CopyAction);
        e->accept();
        QStringList pathList;
        foreach(const QUrl & url, e->mimeData()->urls()) {
            QString fileName = QDir::toNativeSeparators(url.toLocalFile());
            pathList.append(fileName);
        }
        if (!pathList.isEmpty()) {
            setImg(pathList.first());
        }
    }
    else {
        e->ignore();
    }
}

mainWindow::~mainWindow()
{}
