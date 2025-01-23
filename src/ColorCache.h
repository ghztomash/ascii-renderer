#pragma once
#include "ofLog.h"
#include "ofMain.h"
#include <optional>

class ColorCache {
    public:
    ColorCache() {};
    ~ColorCache() {};

    void clear() {
        colorCache.clear();
    };

    void update(ofColor col, size_t index) { colorCache[col] = index; };

    std::optional<size_t> get(ofColor col) {
        if (auto search = colorCache.find(col); search != colorCache.end()) {
            return search->second;
        }
        return std::nullopt;
    };

    void print() {
        ofLogNotice() << "ColorCache: ";
        for (const auto &kv : colorCache) {
            const ofColor &col = kv.first;
            size_t idx = kv.second;
            ofLogNotice()
                << "Color (r=" << (int)col.r
                << ", g=" << (int)col.g
                << ", b=" << (int)col.b
                << ", a=" << (int)col.a
                << ") => " << idx;
        }
    };

    struct ofColorHash {
        std::size_t operator()(const ofColor &c) const {
            uint64_t h = 0;
            // Pack RGBA in 4 bytes
            h |= (uint64_t)c.r;
            h |= (uint64_t)(c.g) << 8;
            h |= (uint64_t)(c.b) << 16;
            h |= (uint64_t)(c.a) << 24;
            return std::hash<uint64_t>()(h);
        }
    };

    struct ofColorEqual {
        bool operator()(const ofColor &a, const ofColor &b) const {
            return (a.r == b.r && a.g == b.g &&
                    a.b == b.b && a.a == b.a);
        }
    };

    private:
    std::unordered_map<ofColor, size_t, ofColorHash, ofColorEqual> colorCache;
};
