/**
 * @file web_routes.h
 * @brief Web server route definitions and setup
 * @details Handles HTTP route registration and server initialization
 */

#pragma once

#include <WebServer.h>

// External web server instance
extern WebServer server;

/**
 * @brief Initialize and configure all web server routes
 */
void setupWebRoutes();

/**
 * @brief Start the web server
 * @return true if server started successfully
 */
bool startWebServer();

/**
 * @brief Stop the web server
 */
void stopWebServer();

/**
 * @brief Check if web server is active
 * @return true if server is running
 */
bool isWebServerActive();

/**
 * @brief Get web server status string
 * @return Status information string
 */
String getWebServerStatus();
