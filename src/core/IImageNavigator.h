#pragma once

#include <QString>
#include <QStringList>

class QObject;

namespace simplepic {

// 规范：图片导航器接口
// 实现类须继承此接口，并通过 asQObject() 暴露 QObject 指针以支持信号连接
// 实现类须定义信号：
//   void currentFileChanged(const QString& filePath, int index, int total);
class IImageNavigator {
public:
    virtual ~IImageNavigator() = default;

    // 扫描指定文件所在目录，定位到该文件
    virtual void setCurrentFile(const QString& filePath) = 0;

    // 当前文件路径
    virtual QString currentFile() const = 0;

    // 当前索引（从 0 开始），无文件时返回 -1
    virtual int currentIndex() const = 0;

    // 文件总数
    virtual int totalCount() const = 0;

    // 导航操作
    virtual bool goNext() = 0;
    virtual bool goPrevious() = 0;
    virtual bool jumpTo(int index) = 0;

    // 获取指定索引的文件路径
    virtual QString fileAt(int index) const = 0;

    // 获取文件列表
    virtual QStringList fileList() const = 0;

    // 返回 QObject 指针，用于信号槽连接
    virtual QObject* asQObject() = 0;
};

} // namespace simplepic
