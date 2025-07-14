<?php
// GhostWhisper PHP Development Server
// Handle API endpoints for testing

$request_uri = $_SERVER['REQUEST_URI'];

// Mock API endpoints
if (strpos($request_uri, '/volume/') === 0) {
    header('Content-Type: application/json');
    
    if (strpos($request_uri, '/volume/up') !== false) {
        echo json_encode(['status' => 'success', 'volume' => 55]);
    } elseif (strpos($request_uri, '/volume/down') !== false) {
        echo json_encode(['status' => 'success', 'volume' => 35]);
    } elseif (strpos($request_uri, '/volume/set') !== false) {
        $level = isset($_GET['level']) ? (int)$_GET['level'] : 50;
        echo json_encode(['status' => 'success', 'volume' => $level]);
    }
    exit;
}

if ($request_uri === '/status') {
    header('Content-Type: application/json');
    echo json_encode([
        'volume' => 45,
        'wifi' => '192.168.1.100',
        'uptime' => 3600,
        'freeHeap' => 234567
    ]);
    exit;
}

if ($request_uri === '/test') {
    header('Content-Type: text/plain');
    echo 'Volume diagnostic test completed. Check Serial Monitor for results.';
    exit;
}

// Serve static files or load HTML with template replacement
if ($request_uri === '/' || $request_uri === '/index.html') {
    $html = file_get_contents('index.html');
    
    // Replace template variables
    $html = str_replace('%VOLUME%', '45', $html);
    $html = str_replace('%WIFI_IP%', '192.168.1.100', $html);
    $html = str_replace('%UPTIME%', '3600', $html);
    $html = str_replace('%FREE_HEAP%', '234567', $html);
    
    header('Content-Type: text/html');
    echo $html;
} else {
    // Let PHP serve static files normally
    return false;
}
?>
