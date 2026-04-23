#pragma once

#include "IImageCache.h"
#include <QMutex>
#include <list>
#include <unordered_map>

namespace simplepic {

class ImageCache : public IImageCache {
public:
    static constexpr int kDefaultCapacity = 20;

    explicit ImageCache(int capacity = kDefaultCapacity);
    ~ImageCache() override;

    ImageCache(const ImageCache&) = delete;
    ImageCache& operator=(const ImageCache&) = delete;

    QPixmap get(const QString& filePath) override;
    void put(const QString& filePath, const QPixmap& pixmap) override;
    bool contains(const QString& filePath) const override;
    void clear() override;
    int size() const override;
    int capacity() const override;

private:
    struct QStringHash {
        size_t operator()(const QString& s) const { return qHash(s); }
    };

    using CacheList = std::list<std::pair<QString, QPixmap>>;
    using CacheMap = std::unordered_map<QString, CacheList::iterator, QStringHash>;

    void evict();

    int m_capacity;
    CacheList m_cacheList;
    CacheMap m_cacheMap;
    mutable QMutex m_mutex;
};

} // namespace simplepic
