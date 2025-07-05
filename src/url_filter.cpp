#include "url_filter.h"

// Static cache for validated URLs
static std::vector<String> validatedUrls;

std::vector<String> getFilteredUrls(const String* urls, size_t count) {
    // Simple filtering - no network validation for fast startup
    std::vector<String> urlVec;
    for (size_t i = 0; i < count; ++i) {
        String url = urls[i];
        
        // Basic filtering to avoid known problematic notes
        if (url.indexOf("Cb") == -1 && url.indexOf("Fb") == -1 && 
            url.indexOf("0.mp3") == -1) { // Skip all octave 0 notes
            urlVec.push_back(url);
        }
    }
    return urlVec;
}

void cacheValidUrls(const std::vector<String>& urls) {
    validatedUrls = urls;
}

std::vector<String> getCachedUrls() {
    return validatedUrls;
}
