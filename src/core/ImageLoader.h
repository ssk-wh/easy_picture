#pragma once

#include "IImageLoader.h"
#include <QObject>
#include <memory>

namespace simplepic {

class ImageLoader : public QObject, public IImageLoader {
    Q_OBJECT

public:
    explicit ImageLoader(QObject* parent = nullptr);
    ~ImageLoader() override;

    QPixmap loadSync(const QString& filePath) override;
    void loadAsync(const QString& filePath) override;
    void cancelAll() override;
    QObject* asQObject() override { return this; }

    static QStringList supportedFilters();

signals:
    void imageLoaded(const QString& filePath, const QPixmap& pixmap);
    void loadFailed(const QString& filePath, const QString& error);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace simplepic
