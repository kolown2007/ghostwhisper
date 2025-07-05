#pragma once

#include "Arduino.h"
#include <vector>

// URL filtering and caching functions
std::vector<String> getFilteredUrls(const String* urls, size_t count);
void cacheValidUrls(const std::vector<String>& urls);
std::vector<String> getCachedUrls();
