#ifndef GUI_H
#define GUI_H

class Fl_Double_Window;
class DisplayWindow;
class Command;

class Gui {
    public:
        Gui(int width, int height);
        ~Gui();

        void setFrontCameraImage(unsigned char* image);
        void update(double dt);
        bool handleCommand(const Command& cmd);

    private:
        int width;
        int height;
        DisplayWindow* window;
};

#endif // GUI_H
