#pragma once

#include <QPixmap>
#include <QString>

namespace simplepic {

// 规范：图片缓存接口
class IImageCache {
public:
    virtual ~IImageCache() = default;

    // 获取缓存的图片，未命中返回 null QPixmap
    virtual QPixmap get(const QString& filePath) = 0;

    // 放入缓存，超出容量时淘汰最久未使用的条目
    virtual void put(const QString& filePath, const QPixmap& pixmap) = 0;

    // 查询是否已缓存
    virtual bool contains(const QString& filePath) const = 0;

    // 清空缓存
    virtual void clear() = 0;

    // 当前缓存条目数
    virtual int size() const = 0;

    // 缓存容量
    virtual int capacity() const = 0;
};

} // namespace simplepic
