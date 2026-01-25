# Graph Functionality Guide

## Overview
The Graph class provides a simple way to display real-time, scrolling graphs on your SSD1325 OLED display. It automatically manages trailing data and updates as new values are added.

## Features
- **Trailing Data Display**: Shows historical data that scrolls from right to left
- **Auto-Scaling**: Automatically adjusts the Y-axis range based on data values
- **Fixed Scaling**: Option to set a fixed Y-axis range
- **Configurable Size**: Position and size the graph anywhere on the display
- **Efficient Updates**: Uses a circular buffer to manage up to 108 data points

## Quick Start

### 1. Include Required Headers
```cpp
#include "oled.h"
#include "graph.h"
```

### 2. Create Graph Object
```cpp
// Create a graph at position (x=10, y=5) with size (width=108, height=50)
Graph myGraph(10, 5, 108, 50);
```

### 3. Push Data and Display
```cpp
void loop() {
    // Get your data (sensor reading, calculation, etc.)
    float value = getSensorValue();
    
    // Add the value to the graph
    myGraph.push_value(value);
    
    // Clear display, draw graph, and update
    oled.clearDisplay();
    oled.draw_graph(myGraph);
    oled.oled_update();
    
    delay(100);  // Update rate
}
```

## API Reference

### Constructor
```cpp
Graph(int x, int y, int width, int height)
```
- `x`, `y`: Top-left corner position on display
- `width`, `height`: Size of the graph in pixels

### Methods

#### `void push_value(float value)`
Adds a new data point to the graph. Older data automatically scrolls left.
```cpp
myGraph.push_value(temperature);
```

#### `void enable_auto_scale(bool enable)`
Enable or disable automatic Y-axis scaling.
```cpp
myGraph.enable_auto_scale(true);  // Auto-scale based on data
myGraph.enable_auto_scale(false); // Use fixed scale
```

#### `void set_scale_range(float min_val, float max_val)`
Set a fixed Y-axis range. Automatically disables auto-scaling.
```cpp
myGraph.set_scale_range(0, 100);  // Y-axis from 0 to 100
```

#### `void clear()`
Clears all data points from the graph.
```cpp
myGraph.clear();
```

#### `void draw(Custom_Adafruit_SSD1325& display)`
Draws the graph on the display. Usually called via `oled.draw_graph()`.

#### `float get_min_value()`
Returns the minimum value in current data.

#### `float get_max_value()`
Returns the maximum value in current data.

#### `int get_data_count()`
Returns the number of data points currently stored.

## Usage Examples

### Example 1: Temperature Monitor
```cpp
Graph tempGraph(10, 5, 108, 50);

void setup() {
    oled.setupOLED();
    tempGraph.enable_auto_scale(true);
}

void loop() {
    float temp = readTemperature();
    tempGraph.push_value(temp);
    
    oled.clearDisplay();
    oled.send_to_oled_buffer(0, "Temp: " + String(temp, 1) + "C");
    oled.draw_graph(tempGraph);
    oled.oled_update();
    
    delay(200);
}
```

### Example 2: Battery Voltage with Fixed Scale
```cpp
Graph voltageGraph(10, 10, 100, 40);

void setup() {
    oled.setupOLED();
    voltageGraph.set_scale_range(3.0, 4.2);  // Li-ion range
}

void loop() {
    float voltage = readBatteryVoltage();
    voltageGraph.push_value(voltage);
    
    oled.clearDisplay();
    oled.draw_graph(voltageGraph);
    oled.oled_update();
    delay(500);
}
```

### Example 3: Multiple Graphs
```cpp
Graph speedGraph(5, 5, 55, 25);
Graph tempGraph(65, 5, 55, 25);
Graph powerGraph(5, 35, 118, 25);

void loop() {
    speedGraph.push_value(getSpeed());
    tempGraph.push_value(getTemp());
    powerGraph.push_value(getPower());
    
    oled.clearDisplay();
    
    // Draw all graphs
    oled.draw_graph(speedGraph);
    oled.draw_graph(tempGraph);
    oled.draw_graph(powerGraph);
    
    // Add labels
    oled.send_to_oled_buffer(0, "Speed");
    oled.send_to_oled_buffer(0, 8, "Temp");
    
    oled.oled_update();
    delay(100);
}
```

### Example 4: Integrating with Existing Code
```cpp
// In your global variables section
Graph myDataGraph(10, 5, 108, 50);

// In your main loop where you process data
void processData() {
    // ... your existing data processing ...
    
    // Add to graph
    myDataGraph.push_value(someValue);
    
    // When ready to display
    oled.clearDisplay();
    // ... your other display code ...
    oled.draw_graph(myDataGraph);
    oled.oled_update();
}
```

## Tips and Best Practices

1. **Graph Size**: Maximum width is about 108 pixels (128 - margins). The class can store up to 108 data points.

2. **Update Rate**: Adjust `delay()` to control how fast the graph updates. Too fast may make it hard to read, too slow may miss data changes.

3. **Auto-Scale vs Fixed**: 
   - Use **auto-scale** for data with unknown or varying ranges
   - Use **fixed scale** when you want consistent comparison across time

4. **Performance**: Drawing a graph is relatively fast, but updating the entire display takes time. Consider updating at 5-20 Hz for smooth visualization.

5. **Multiple Graphs**: You can have multiple graphs on one display, but keep them small enough to fit and be readable.

6. **Data Resolution**: For very noisy data, consider averaging a few readings before pushing to the graph.

## Troubleshooting

**Graph not appearing**: 
- Make sure you call `oled.clearDisplay()` before drawing
- Ensure you call `oled.oled_update()` after drawing
- Check that graph position and size fit within display bounds (128x64)

**Graph scales incorrectly**:
- If using auto-scale, make sure you're pushing numeric values
- For fixed scale, ensure your data falls within the set range
- Check if you need to add padding to your scale range

**Jumpy or erratic graph**:
- Your data may be too noisy - consider smoothing/filtering
- Update rate might be too fast - increase delay
- Make sure you're consistently pushing data at regular intervals

## Technical Details

- **Data Storage**: Circular buffer with 108 slots
- **Data Type**: float (4 bytes per point)
- **Memory Usage**: ~432 bytes per graph
- **Drawing Method**: Connects consecutive points with lines
- **Clipping**: Automatic clipping to graph boundaries
- **Orientation**: Newest data on the right, scrolls left
