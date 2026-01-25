# Graph Functionality for SSD1325 Display

## Overview
A complete graphing system for the SSD1325 OLED display that shows real-time, scrolling graphs with trailing data that updates as new values are added.

## Files Added/Modified

### New Files
- **graph.h** - Graph class header with API definitions
- **graph.cpp** - Graph class implementation
- **GRAPH_USAGE.md** - Complete usage guide and API reference
- **graph_example.ino** - Basic usage examples
- **graph_test.ino** - Test sketch to verify functionality
- **INTEGRATION_EXAMPLE.ino** - Shows how to integrate into CANslave project

### Modified Files
- **oled.h** - Added `#include "graph.h"` and `draw_graph()` method
- **oled.cpp** - Added `draw_graph()` implementation

## Quick Start

### 1. Basic Usage
```cpp
#include "oled.h"
#include "graph.h"

OLED oled;
Graph myGraph(10, 5, 108, 50);

void setup() {
    oled.setupOLED();
    myGraph.enable_auto_scale(true);
}

void loop() {
    float value = getSensorValue();
    myGraph.push_value(value);
    
    oled.clearDisplay();
    oled.draw_graph(myGraph);
    oled.oled_update();
    
    delay(100);
}
```

### 2. Test the Implementation
Upload **graph_test.ino** to verify everything works correctly. You should see a smooth sine wave scrolling across the display.

### 3. Integration
See **INTEGRATION_EXAMPLE.ino** for detailed examples of integrating graphs into your existing CANslave project.

## Features

✅ **Real-time Updates** - Push new data and see it appear immediately
✅ **Trailing Data** - Shows up to 108 historical data points
✅ **Auto-Scaling** - Automatically adjusts Y-axis to fit your data
✅ **Fixed Scaling** - Option to set a specific Y-axis range
✅ **Multiple Graphs** - Display multiple graphs simultaneously
✅ **Configurable Size** - Position and size graphs anywhere on display
✅ **Memory Efficient** - Uses ~432 bytes per graph instance

## Key API Methods

| Method | Description |
|--------|-------------|
| `Graph(x, y, w, h)` | Create graph at position (x,y) with size (w,h) |
| `push_value(float)` | Add new data point to graph |
| `enable_auto_scale(bool)` | Enable/disable automatic Y-axis scaling |
| `set_scale_range(min, max)` | Set fixed Y-axis range |
| `clear()` | Clear all data points |
| `get_min_value()` | Get minimum value in current data |
| `get_max_value()` | Get maximum value in current data |

## Display Integration

Use with your OLED object:
```cpp
oled.clearDisplay();
oled.draw_graph(myGraph);
oled.oled_update();
```

## Examples Included

1. **graph_example.ino** - Basic examples with sine wave, random data, and sensor readings
2. **graph_test.ino** - Test sketch to verify functionality
3. **INTEGRATION_EXAMPLE.ino** - Integration with CANslave project

## Documentation

See **GRAPH_USAGE.md** for:
- Complete API reference
- Multiple usage examples
- Tips and best practices
- Troubleshooting guide
- Performance considerations

## Configuration

### Graph Size
- **Display**: 128x64 pixels (SSD1325)
- **Recommended graph width**: 108 pixels (leaves 10px margins on each side)
- **Maximum data points**: 108 (matches max width)
- **Recommended graph height**: 40-50 pixels (leaves room for labels)

### Performance
- **Update rate**: Recommended 10-20 Hz (50-100ms between updates)
- **Memory**: ~432 bytes per Graph object
- **Drawing time**: ~5-10ms per graph

## Integration Tips

1. **Add to existing project**:
   - Include "graph.h" at the top
   - Create Graph object(s) globally
   - Call `push_value()` when you receive/calculate data
   - Call `oled.draw_graph()` in your display update routine

2. **Multiple pages**:
   - Create different Graph objects for different pages
   - Only draw the graph for the current page

3. **Data smoothing**:
   - For noisy data, average several readings before pushing
   - Example: `graph.push_value((val1 + val2 + val3) / 3.0)`

4. **Throttling updates**:
   ```cpp
   static unsigned long lastGraphUpdate = 0;
   if (millis() - lastGraphUpdate > 100) {
       myGraph.push_value(value);
       lastGraphUpdate = millis();
   }
   ```

## Troubleshooting

**Graph not showing**: Ensure you're calling `oled.clearDisplay()` before and `oled.oled_update()` after drawing.

**Compilation errors**: Make sure all files are in the same directory and Arduino IDE can find them.

**Graph looks wrong**: Check that your graph position and size fit within the 128x64 display bounds.

**Jumpy display**: Reduce update rate or smooth your data before pushing to graph.

## Technical Details

- **Data Storage**: Fixed-size array (circular buffer)
- **Data Type**: float (32-bit)
- **Max Points**: 108 data points
- **Drawing**: Connects consecutive points with lines
- **Scaling**: Automatic or manual with 10% padding
- **Orientation**: Newest data on right, scrolls left

## Requirements

- SSD1325 OLED Display (128x64)
- Custom_Adafruit_SSD1325 library (included in project)
- Custom_Adafruit_GFX library (included in project)

## License

Created by Ross Klonowski for use with CANserver project.

---

For questions or issues, refer to GRAPH_USAGE.md for detailed documentation.
