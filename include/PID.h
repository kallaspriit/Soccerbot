#ifndef PID_H
#define PID_H

class PID {
    public:
		PID() : p(1), i(0), d(0), iLimit(0) {}
        PID(float p, float i, float d, float iLimit);
        void setTarget(float target) { this->target = target; }
        float getValue(float feedback, double dt);
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
