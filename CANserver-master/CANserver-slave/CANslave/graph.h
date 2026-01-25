//
//  graph.h
//  To be used with CANserver created by Ross Klonowski
//
//  Created by Ross Klonowski on January 15 2023.
//

#ifndef graph_h
#define graph_h

// Forward declaration to avoid circular include
class Custom_Adafruit_SSD1325;

class Graph {
    
    private:
        static const int MAX_DATA_POINTS = 108; // 128 - 20 pixels for margins
        float data[MAX_DATA_POINTS];
        unsigned long timestamps[MAX_DATA_POINTS]; // Timestamp in milliseconds
        int data_count;
        
        float min_value;
        float max_value;
        bool auto_scale;
        
        int x_position;
        int y_position;
        int width;
        int height;
        
        float scale_min;
        float scale_max;
        
        unsigned long time_window_ms; // Time window in milliseconds (0 = disabled)
        bool use_time_based;
        
        void update_scale();
        
    public:
        Graph(int x, int y, int w, int h);
        
        void push_value(float value);
        
        void push_value(float value, unsigned long timestamp);
        
        void set_scale_range(float min_val, float max_val);
        
        void enable_auto_scale(bool enable);
        
        void set_time_window(unsigned long window_ms);
        
        void enable_time_based(bool enable);
        
        void clear();
        
        void draw(Custom_Adafruit_SSD1325& display);
        
        float get_min_value();
        
        float get_max_value();
        
        int get_data_count();
        
        unsigned long get_time_window();
};

#endif /* graph_h */