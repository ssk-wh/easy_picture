#pragma once

#include "IImageNavigator.h"
#include <QObject>
#include <QStringList>

namespace simplepic {

class ImageNavigator : public QObject, public IImageNavigator {
    Q_OBJECT

public:
    explicit ImageNavigator(QObject* parent = nullptr);
    ~ImageNavigator() override;

    void setCurrentFile(const QString& filePath) override;
    QString currentFile() const override;
    int currentIndex() const override;
    int totalCount() const override;
    bool goNext() override;
    bool goPrevious() override;
    bool jumpTo(int index) override;
    QString fileAt(int index) const override;
    QStringList fileList() const override;
    QObject* asQObject() override { return this; }

signals:
    void currentFileChanged(const QString& filePath, int index, int total);

private:
    void scanDirectory(const QString& dirPath);

    QStringList m_fileList;
    int m_currentIndex = -1;
};

} // namespace simplepic
