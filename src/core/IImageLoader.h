#pragma once

#include <QPixmap>
#include <QString>
#include <QStringList>

class QObject;

namespace simplepic {

// 规范：图片加载器接口
// 实现类须继承此接口，并通过 asQObject() 暴露 QObject 指针以支持信号连接
// 实现类须定义信号：
//   void imageLoaded(const QString& filePath, const QPixmap& pixmap);
//   void loadFailed(const QString& filePath, const QString& error);
class IImageLoader {
public:
    virtual ~IImageLoader() = default;

    // 同步加载图片，成功返回有效 QPixmap，失败返回 null QPixmap
    virtual QPixmap loadSync(const QString& filePath) = 0;

    // 异步加载图片，完成后通过 imageLoaded 信号通知
    virtual void loadAsync(const QString& filePath) = 0;

    // 取消所有正在进行的异步加载
    virtual void cancelAll() = 0;

    // 返回 QObject 指针，用于信号槽连接
    virtual QObject* asQObject() = 0;
};

} // namespace simplepic
