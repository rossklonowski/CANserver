//
//  gueue.h
//  To be used with CANserver created by Ross Klonowski
//
//  Created by Ross Klonowski on January 15 2023.
//

#ifndef queue_h
#define queue_h

class my_queue {
    
    private:
        const int size = 127;

        int queue[];

    public:
        my_queue();

        void push(int);

        void print();

        int get_size();

        int get_val(int);
};

#endif /* queue_h */