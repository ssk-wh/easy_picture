#include "MainWindow.h"
#include "ImageView.h"
#include "ChangelogDialog.h"
#include "../core/IImageLoader.h"
#include "../core/IImageCache.h"
#include "../core/IImageNavigator.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

namespace simplepic {

MainWindow::MainWindow(
    std::unique_ptr<IImageLoader> loader,
    std::unique_ptr<IImageCache> cache,
    std::unique_ptr<IImageNavigator> navigator,
    QWidget* parent)
    : QWidget(parent)
    , m_loader(std::move(loader))
    , m_cache(std::move(cache))
    , m_navigator(std::move(navigator))
{
    setupUI();
    connectSignals();
    restoreWindowState();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    setWindowTitle(QStringLiteral("SimplePicture"));
    setMinimumSize(640, 480);
    resize(1024, 768);

    m_imageView = new ImageView(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_imageView);
}

void MainWindow::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));
}

void MainWindow::connectSignals()
{
    connect(m_imageView, &ImageView::nextImageRequested,
            this, &MainWindow::onNextImage);
    connect(m_imageView, &ImageView::previousImageRequested,
            this, &MainWindow::onPreviousImage);
    connect(m_imageView, &ImageView::fileDropped,
            this, &MainWindow::openFile);

    // 通过 asQObject() 桥接接口与 Qt 信号槽系统
    connect(m_loader->asQObject(), SIGNAL(imageLoaded(QString, QPixmap)),
            this, SLOT(onImageLoaded(QString, QPixmap)));
    connect(m_navigator->asQObject(), SIGNAL(currentFileChanged(QString, int, int)),
            this, SLOT(onCurrentFileChanged(QString, int, int)));
}

static bool isSvgFile(const QString& path)
{
    return path.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive);
}

static ImageInfo buildImageInfo(const QString& filePath)
{
    ImageInfo info;
    QFileInfo fi(filePath);
    info.filePath = fi.absoluteFilePath();
    info.fileSize = fi.size();
    info.lastModified = fi.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    QImageReader reader(filePath);
    reader.setDecideFormatFromContent(true);
    const QSize imgSize = reader.size();
    if (imgSize.isValid()) {
        info.pixelWidth = imgSize.width();
        info.pixelHeight = imgSize.height();
    }
    info.format = QString::fromLatin1(reader.format()).toUpper();
    if (info.format.isEmpty())
        info.format = fi.suffix().toUpper();

    const QImage::Format fmt = reader.imageFormat();
    if (fmt != QImage::Format_Invalid)
        info.bitDepth = QImage::toPixelFormat(fmt).bitsPerPixel();

    return info;
}

void MainWindow::displayFile(const QString& filePath)
{
    if (isSvgFile(filePath)) {
        m_imageView->setSvg(filePath);
        if (m_imageView->pixmap().isNull() && !filePath.isEmpty()) {
            m_imageView->setError(QStringLiteral("无法打开图片: SVG"));
        }
    } else {
        QPixmap cached = m_cache->get(filePath);
        if (!cached.isNull()) {
            m_imageView->setPixmap(cached);
        } else {
            QPixmap pix = m_loader->loadSync(filePath);
            if (!pix.isNull()) {
                m_cache->put(filePath, pix);
                m_imageView->setPixmap(pix);
            } else {
                const QString suffix = QFileInfo(filePath).suffix().toUpper();
                m_imageView->setError(
                    QStringLiteral("无法打开图片: %1").arg(suffix.isEmpty() ? filePath : suffix));
            }
        }
    }

    m_imageView->setImageInfo(buildImageInfo(filePath));
}

void MainWindow::openFile(const QString& filePath)
{
    QFileInfo fi(filePath);
    if (!fi.exists() || !fi.isFile())
        return;

    const QString absPath = fi.absoluteFilePath();

    displayFile(absPath);

    m_currentFile = absPath;

    QTimer::singleShot(0, this, [this, absPath]() {
        m_navigator->setCurrentFile(absPath);
        updateWindowTitle();
        preloadNeighbors();
    });
}

void MainWindow::onNextImage()
{
    if (m_navigator->goNext()) {
        loadCurrentImage();
    }
}

void MainWindow::onPreviousImage()
{
    if (m_navigator->goPrevious()) {
        loadCurrentImage();
    }
}

void MainWindow::onImageLoaded(const QString& filePath, const QPixmap& pixmap)
{
    if (isSvgFile(filePath))
        return;

    m_cache->put(filePath, pixmap);

    if (filePath == m_currentFile) {
        m_imageView->setPixmap(pixmap);
    }
}

void MainWindow::onCurrentFileChanged(const QString& filePath, int /*index*/, int /*total*/)
{
    m_currentFile = filePath;
    updateWindowTitle();
}

void MainWindow::loadCurrentImage()
{
    const QString file = m_navigator->currentFile();
    if (file.isEmpty())
        return;

    m_currentFile = file;
    displayFile(file);
    updateWindowTitle();
    preloadNeighbors();
}

void MainWindow::preloadNeighbors()
{
    const int current = m_navigator->currentIndex();
    const int total = m_navigator->totalCount();
    if (current < 0 || total <= 0)
        return;

    for (int delta : {-2, -1, 1, 2}) {
        const int idx = current + delta;
        if (idx < 0 || idx >= total)
            continue;

        const QString file = m_navigator->fileAt(idx);
        if (file.isEmpty() || m_cache->contains(file))
            continue;

        m_loader->loadAsync(file);
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F1) {
        showChangelog();
        return;
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::showChangelog()
{
    ChangelogDialog dlg(this);
    dlg.exec();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveWindowState();
    QWidget::closeEvent(event);
}

void MainWindow::saveWindowState()
{
    QSettings settings;
    settings.setValue(QStringLiteral("MainWindow/geometry"), saveGeometry());
}

void MainWindow::restoreWindowState()
{
    QSettings settings;
    const QByteArray geometry =
        settings.value(QStringLiteral("MainWindow/geometry")).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
}

void MainWindow::updateWindowTitle()
{
    const int index = m_navigator->currentIndex();
    const int total = m_navigator->totalCount();

    if (m_currentFile.isEmpty() || index < 0) {
        setWindowTitle(QStringLiteral("SimplePicture"));
        return;
    }

    const QString fileName = QFileInfo(m_currentFile).fileName();
    setWindowTitle(QStringLiteral("SimplePicture - %1 [%2/%3]")
                       .arg(fileName)
                       .arg(index + 1)
                       .arg(total));
}

} // namespace simplepic
