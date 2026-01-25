# Graph Implementation Checklist

## ✅ Getting Started Guide

Follow these steps to start using the graph functionality:

### Step 1: Verify Installation
- [ ] Confirm `graph.h` exists in your CANslave folder
- [ ] Confirm `graph.cpp` exists in your CANslave folder
- [ ] Confirm `oled.h` has been updated (includes `#include "graph.h"`)
- [ ] Confirm `oled.cpp` has been updated (has `draw_graph()` method)

### Step 2: Test the Implementation
- [ ] Upload `graph_test.ino` to your Arduino
- [ ] Verify you see a sine wave scrolling on the display
- [ ] Check that values update at the top and bottom of the screen
- [ ] Confirm the graph scales properly

### Step 3: Understand the Basics
- [ ] Read `GRAPH_README.md` (5 min quick overview)
- [ ] Review `graph_example.ino` (see basic usage patterns)
- [ ] Look at `GRAPH_LAYOUT_GUIDE.ino` (understand coordinate system)

### Step 4: Plan Your Integration
- [ ] Decide which data you want to graph (power, temp, speed, etc.)
- [ ] Choose graph size and position for your layout
- [ ] Decide if you want auto-scaling or fixed range
- [ ] Plan how to integrate with your existing display code

### Step 5: Basic Integration
- [ ] Add `#include "graph.h"` to your CANslave.ino
- [ ] Create Graph object(s) globally: `Graph myGraph(10, 5, 108, 50);`
- [ ] Initialize in `setup()`: `myGraph.enable_auto_scale(true);`
- [ ] Push data when received: `myGraph.push_value(value);`
- [ ] Display in your update function: `oled.draw_graph(myGraph);`

### Step 6: Test Your Integration
- [ ] Compile and upload your modified code
- [ ] Verify graph appears on display
- [ ] Check that data updates correctly
- [ ] Adjust timing/update rate if needed

### Step 7: Refine and Customize
- [ ] Adjust graph size and position if needed
- [ ] Add text labels (title, current value, min/max)
- [ ] Fine-tune scaling (auto vs fixed)
- [ ] Integrate with button handlers if desired
- [ ] Add multiple graphs if needed

## 📋 Quick Reference Checklist

### For Each Graph You Create:

#### 1. Declaration
```cpp
Graph myGraph(x, y, width, height);  // Position and size
```
- [ ] x: 5-20 pixels from left (recommended 10)
- [ ] y: 5-10 pixels from top
- [ ] width: 50-108 pixels (recommended 108)
- [ ] height: 20-50 pixels (recommended 40-50)

#### 2. Configuration (in setup)
```cpp
myGraph.enable_auto_scale(true);  // or false for fixed
// If fixed:
// myGraph.set_scale_range(min, max);
```
- [ ] Choose auto-scale or fixed range
- [ ] If fixed, determine appropriate min/max

#### 3. Data Input (when data arrives)
```cpp
myGraph.push_value(value);
```
- [ ] Call when new data arrives
- [ ] Consider throttling if data arrives too fast
- [ ] Ensure value is a float or can be converted to float

#### 4. Display (in display update function)
```cpp
oled.clearDisplay();
oled.draw_graph(myGraph);
oled.oled_update();
```
- [ ] Clear display before drawing
- [ ] Draw graph
- [ ] Update display after drawing

## 🎯 Common Integration Patterns

### Pattern 1: Simple Single Graph
```cpp
// Global
Graph powerGraph(10, 5, 108, 50);

// Setup
powerGraph.enable_auto_scale(true);

// Loop
powerGraph.push_value(batteryPowerKW);
oled.clearDisplay();
oled.draw_graph(powerGraph);
oled.oled_update();
```
- [ ] Implemented
- [ ] Tested
- [ ] Working correctly

### Pattern 2: Graph with Labels
```cpp
oled.clearDisplay();
oled.send_to_oled_buffer(0, "Power (kW)");
oled.draw_graph(powerGraph);
oled.send_to_oled_buffer(7, String(currentPower, 1) + " kW");
oled.oled_update();
```
- [ ] Implemented
- [ ] Tested
- [ ] Working correctly

### Pattern 3: Multiple Graphs
```cpp
Graph graph1(5, 5, 55, 44);
Graph graph2(65, 5, 55, 44);

oled.clearDisplay();
oled.draw_graph(graph1);
oled.draw_graph(graph2);
oled.oled_update();
```
- [ ] Implemented
- [ ] Tested
- [ ] Working correctly

### Pattern 4: Page-Based Display
```cpp
switch(currentPage) {
    case PAGE_GRAPH:
        oled.clearDisplay();
        oled.draw_graph(myGraph);
        oled.oled_update();
        break;
    // ... other pages
}
```
- [ ] Implemented
- [ ] Tested
- [ ] Working correctly

## 🔍 Troubleshooting Checklist

### Graph Not Appearing
- [ ] Called `oled.clearDisplay()` before drawing?
- [ ] Called `oled.oled_update()` after drawing?
- [ ] Graph position/size within display bounds (128x64)?
- [ ] Graph object properly initialized?
- [ ] At least 2 data points pushed to graph?

### Graph Looks Wrong
- [ ] Check x, y, width, height values make sense
- [ ] Verify data values are reasonable (not NaN or infinity)
- [ ] If using fixed scale, range includes your data?
- [ ] Graph size is at least 30x20 pixels?

### Data Not Updating
- [ ] Calling `push_value()` with new data?
- [ ] Data actually changing?
- [ ] Display update function being called?
- [ ] Not clearing graph accidentally?

### Performance Issues
- [ ] Update rate reasonable (not too fast)?
- [ ] Not creating/destroying graphs in loop?
- [ ] Display update not called too frequently?

### Compilation Errors
- [ ] `graph.h` and `graph.cpp` in correct folder?
- [ ] `#include "graph.h"` at top of file?
- [ ] oled.h updated with graph.h include?
- [ ] All files saved?

## 📚 Documentation Reference

- **Quick Start**: GRAPH_README.md
- **Full API**: GRAPH_USAGE.md
- **Examples**: graph_example.ino
- **Test Code**: graph_test.ino
- **Integration**: INTEGRATION_EXAMPLE.ino
- **CANserver Specific**: CANserver_GRAPH_EXAMPLES.ino
- **Layout Guide**: GRAPH_LAYOUT_GUIDE.ino
- **This Checklist**: CHECKLIST.md

## 💡 Best Practices

### DO:
- [ ] Use auto-scale for unknown data ranges
- [ ] Use fixed scale for consistent comparisons
- [ ] Update at reasonable rate (50-200ms typically)
- [ ] Leave margins around graphs (5-10 pixels)
- [ ] Test with real data before deploying
- [ ] Add labels when space permits

### DON'T:
- [ ] Update faster than you can see (~10ms minimum)
- [ ] Create graphs in loop() (do it globally)
- [ ] Forget to clear display before drawing
- [ ] Forget to update display after drawing
- [ ] Make graphs too small (minimum 30x20)
- [ ] Push invalid data (NaN, infinity)

## 🎓 Learning Path

### Beginner (Start Here)
1. [ ] Read GRAPH_README.md
2. [ ] Upload and test graph_test.ino
3. [ ] Review graph_example.ino
4. [ ] Try Pattern 1 (Simple Single Graph)

### Intermediate
1. [ ] Read GRAPH_USAGE.md
2. [ ] Study INTEGRATION_EXAMPLE.ino
3. [ ] Try Pattern 2 (Graph with Labels)
4. [ ] Integrate one graph into your project

### Advanced
1. [ ] Review CANserver_GRAPH_EXAMPLES.ino
2. [ ] Study GRAPH_LAYOUT_GUIDE.ino
3. [ ] Try multiple graphs or page-based display
4. [ ] Create custom layouts for your needs

## ✨ Success Criteria

You've successfully integrated graphs when:
- [ ] Test sketch (graph_test.ino) runs correctly
- [ ] At least one graph displays your actual data
- [ ] Graph updates in real-time as data changes
- [ ] Graph scales appropriately (auto or fixed)
- [ ] Display is readable and performs well
- [ ] Code compiles without errors or warnings
- [ ] You understand how to add more graphs if needed

## 🎉 Next Steps After Success

- [ ] Experiment with different graph sizes
- [ ] Try multiple graphs on one screen
- [ ] Add more data streams to graph
- [ ] Integrate with button navigation
- [ ] Create custom layouts for your use case
- [ ] Share your implementation!

---

**Need Help?**
- Review the documentation files
- Check the troubleshooting section
- Look at the example files
- Verify each checklist item

**Ready to Start?**
Begin with Step 1 and work through each item systematically.
