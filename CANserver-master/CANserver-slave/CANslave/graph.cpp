//
//  graph.cpp
//
//

#include "graph.h"
#include "Custom_Adafruit_SSD1325.h"
#include <Arduino.h>

Graph::Graph(int x, int y, int w, int h) {
    x_position = x;
    y_position = y;
    width = w;
    height = h;
    data_count = 0;
    min_value = 0;
    max_value = 100;
    auto_scale = true;
    scale_min = 0;
    scale_max = 100;
    time_window_ms = 0;
    use_time_based = false;
    
    // Initialize data arrays
    for (int i = 0; i < MAX_DATA_POINTS; i++) {
        data[i] = 0;
        timestamps[i] = 0;
    }
}

void Graph::push_value(float value) {
    push_value(value, millis());
}

void Graph::push_value(float value, unsigned long timestamp) {
    // Shift all values and timestamps to the left
    for (int i = MAX_DATA_POINTS - 1; i > 0; i--) {
        data[i] = data[i - 1];
        timestamps[i] = timestamps[i - 1];
    }
    
    // Add new value and timestamp at the beginning
    data[0] = value;
    timestamps[0] = timestamp;
    
    // Increment count up to max
    if (data_count < MAX_DATA_POINTS) {
        data_count++;
    }
    
    // Remove old data points if using time-based mode
    if (use_time_based && time_window_ms > 0) {
        // Remove points older than the time window
        for (int i = 1; i < data_count; i++) {
            if (timestamp - timestamps[i] > time_window_ms) {
                data_count = i;
                break;
            }
        }
    }
    
    // Update scale if auto-scaling is enabled
    if (auto_scale) {
        update_scale();
    }
}

void Graph::set_scale_range(float min_val, float max_val) {
    scale_min = min_val;
    scale_max = max_val;
    auto_scale = false;
}

void Graph::enable_auto_scale(bool enable) {
    auto_scale = enable;
    if (enable) {
        update_scale();
    }
}

void Graph::update_scale() {
    if (data_count == 0) {
        scale_min = 0;
        scale_max = 100;
        return;
    }
    
    // Find min and max in current data
    float current_min = data[0];
    float current_max = data[0];
    
    for (int i = 0; i < data_count; i++) {
        if (data[i] < current_min) current_min = data[i];
        if (data[i] > current_max) current_max = data[i];
    }
    
    min_value = current_min;
    max_value = current_max;
    
    // Add some padding (10%)
    float range = max_value - min_value;
    if (range < 0.1) range = 0.1; // Minimum range to avoid division by zero
    
    scale_min = min_value - range * 0.1;
    scale_max = max_value + range * 0.1;
}

void Graph::set_time_window(unsigned long window_ms) {
    time_window_ms = window_ms;
    use_time_based = (window_ms > 0);
}

void Graph::enable_time_based(bool enable) {
    use_time_based = enable;
    if (!enable) {
        time_window_ms = 0;
    }
}

void Graph::clear() {
    data_count = 0;
    for (int i = 0; i < MAX_DATA_POINTS; i++) {
        data[i] = 0;
        timestamps[i] = 0;
    }
}

void Graph::draw(Custom_Adafruit_SSD1325& display) {
    // Draw frame/axes
    display.drawRect(x_position, y_position, width, height, WHITE);
    
    if (data_count < 2) {
        return; // Need at least 2 points to draw
    }
    
    // Calculate the range for scaling
    float range = scale_max - scale_min;
    if (range < 0.001) range = 0.001; // Prevent division by zero
    
    if (use_time_based && time_window_ms > 0) {
        // TIME-BASED DRAWING
        unsigned long current_time = timestamps[0]; // Most recent timestamp
        unsigned long oldest_time = current_time - time_window_ms;
        int graph_width = width - 2; // Subtract borders
        
        // Draw lines between consecutive points based on timestamps
        for (int i = 0; i < data_count - 1; i++) {
            // Skip if the second point is outside time window
            if (timestamps[i + 1] < oldest_time) break;
            
            // Calculate X positions based on time window
            // Right side = current_time (0ms ago), left side = oldest_time (time_window_ms ago)
            int x1 = x_position + width - 1 - (int)((float)(current_time - timestamps[i]) * graph_width / time_window_ms);
            int x2 = x_position + width - 1 - (int)((float)(current_time - timestamps[i + 1]) * graph_width / time_window_ms);
            
            // Clamp X to graph bounds
            if (x1 < x_position + 1) x1 = x_position + 1;
            if (x1 > x_position + width - 2) x1 = x_position + width - 2;
            if (x2 < x_position + 1) x2 = x_position + 1;
            if (x2 > x_position + width - 2) x2 = x_position + width - 2;
            
            // Map data values to pixel coordinates
            int y1 = y_position + height - 1 - (int)((data[i] - scale_min) / range * (height - 2));
            int y2 = y_position + height - 1 - (int)((data[i + 1] - scale_min) / range * (height - 2));
            
            // Clamp Y to graph bounds
            if (y1 < y_position + 1) y1 = y_position + 1;
            if (y1 > y_position + height - 2) y1 = y_position + height - 2;
            if (y2 < y_position + 1) y2 = y_position + 1;
            if (y2 > y_position + height - 2) y2 = y_position + height - 2;
            
            // Draw line between points
            display.drawLine(x1, y1, x2, y2, WHITE);
        }
    } else {
        // SEQUENTIAL DRAWING (original behavior)
        // Determine how many points we can display based on width
        int max_points = width - 2; // Subtract 2 for borders
        int points_to_draw = (data_count < max_points) ? data_count : max_points;
        
        // Draw the graph lines from right to left (newest to oldest)
        for (int i = 0; i < points_to_draw - 1; i++) {
            // Map data values to pixel coordinates
            int y1 = y_position + height - 1 - (int)((data[i] - scale_min) / range * (height - 2));
            int y2 = y_position + height - 1 - (int)((data[i + 1] - scale_min) / range * (height - 2));
            
            // Clamp to graph bounds
            if (y1 < y_position + 1) y1 = y_position + 1;
            if (y1 > y_position + height - 2) y1 = y_position + height - 2;
            if (y2 < y_position + 1) y2 = y_position + 1;
            if (y2 > y_position + height - 2) y2 = y_position + height - 2;
            
            // X coordinates (right to left)
            int x1 = x_position + width - 2 - i;
            int x2 = x_position + width - 2 - (i + 1);
            
            // Draw line between consecutive points
            display.drawLine(x1, y1, x2, y2, WHITE);
        }
    }
}

float Graph::get_min_value() {
    return min_value;
}

float Graph::get_max_value() {
    return max_value;
}

int Graph::get_data_count() {
    return data_count;
}

unsigned long Graph::get_time_window() {
    return time_window_ms;
}
