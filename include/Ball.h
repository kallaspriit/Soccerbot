#ifndef BALL_H
#define BALL_H

class Ball {
    public:
        Ball(float x, float y, float distance, float angle, double dt);
        void updateVisible(float x, float y, float distance, float angle, double dt);
        void updateInvisible(double dt);
        void markForRemoval(double afterSeconds);
        bool shouldBeRemoved();

        int id;
        double createdTime;
        double updatedTime;
        double removeTime;
        float x;
        float y;
        float velocityX;
        float velocityY;
        float distance;
        float angle;
        float elasticity;
        float radius;
        bool visible;
    private:
        static int instances;

        void applyDrag(double dt);
};

#endif // BALL_H
