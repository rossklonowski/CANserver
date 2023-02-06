//
//  progress_bar.h
//

#ifndef ProgressBar_h
#define ProgressBar_h

class ProgressBar {
    private:
        int _xPos;
        int _yPos;
        int _barWidth;
        int _barHeight;
        int _percentFilled;
        bool _set = false;
        bool _border;
        int _borderX;
        int _borderY;
        int _borderWidth;
        int _borderHeight;
    public:
        ProgressBar();
        ProgressBar(int, int, int, int, bool);
        int getxPos();
        int getyPos();
        int getBarWidth();
        int getBarHeight();
        int getBorderX();
        int getBorderY();
        int getBorderWidth();
        int getBorderHeight();
        void setPercentFill(double);
        double getPercentFill();
        bool getSet();
};

#endif /* ProgressBar_h */