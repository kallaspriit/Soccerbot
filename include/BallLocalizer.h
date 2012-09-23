#ifndef BALLLOCALIZER_H
#define BALLLOCALIZER_H

struct Ball {
    Ball();

    static int instances;
    int id;
};

class BallLocalizer {
    public:
        BallLocalizer();
        ~BallLocalizer();

    private:
};

#endif // BALLLOCALIZER_H
