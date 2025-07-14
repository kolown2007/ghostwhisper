#pragma once

#include "Arduino.h"
#include <vector>

// Stream management functions
void connectToStream(const String& url);
void handleStreamReconnection();
void tryNextStream();
void clearStreamCache();
bool isStreamConnected();
void handleStreamProgram();

// Stream state management
struct StreamState {
    String currentStreamURL;
    bool streamConnected;
    int reconnectAttempts;
    std::vector<String> availableStreams;
};

StreamState& getStreamState();
