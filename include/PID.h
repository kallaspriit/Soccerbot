#ifndef PID_H
#define PID_H

class PID
{
    public:
        PID(float p, float i, float d, float iLimit);
        void setTarget(float target) { this->target = target; }
        float getGain(float feedback, double dt);
        void reset();

        float p;
        float i;
        float d;
        float iLimit;

    private:
        float target;
        float lastError;
        float integral;
};

#endif // PID_H
