#ifndef CAMERA_H
#define CAMERA_H

#include "Thread.h"

#include <Windows.h>
#include "xiApi.h"
#include "xiExt.h"
#include "libyuv.h"

#include <string>

class Camera : private Thread {
    public:
        struct YUYV {
            int y1, u, y2, v;
        };

        struct FrameRaw {
            unsigned char* data;
            int size;
            int width;
            int height;
            int number;
            bool fresh;
            double timestamp;
        };

        struct FrameYUYV : public FrameRaw {
            int strideY;
            int strideU;
            int strideV;
            unsigned char* dataY;
            unsigned char* dataU;
            unsigned char* dataV;
            unsigned char* dataYUYV;

            YUYV* getPixelAt(int x, int y) const {
				if (
					x < 0
					|| x > width - 1
					|| y < 0
					|| y > height - 1
				) {
					return NULL;
				}

				YUYV* pixel = new YUYV();

				int yPos = strideY * y + x;
				int uvPos = strideU * (y / 2) + (x / 2);

				int stride = width * 1;

                pixel->y1 = dataY[yPos];
                pixel->u = dataU[uvPos];
                pixel->y2 = dataY[yPos];
                pixel->v = dataV[uvPos];

                return pixel;
            }
        };

        Camera();
        ~Camera();

        bool open(int serial = 0);
        void startAcquisition();
        void stopAcquisition();
        FrameRaw* getFrame();
        //const FrameYUV& getFrameYUV();
        FrameYUYV* getFrameYUYV();
		FrameYUYV* getLastFrame() { return &frameYUV; }
		inline bool capturing() const { return opened; }
        void close();

        std::string getName() { return getStringParam(XI_PRM_DEVICE_NAME); }
        std::string getDeviceType() { return getStringParam(XI_PRM_DEVICE_TYPE); }
        std::string getApiVersion() { return getStringParam(XI_PRM_API_VERSION XI_PRM_INFO); }
        std::string getDriverVersion() { return getStringParam(XI_PRM_DRV_VERSION XI_PRM_INFO); }
        int getSerialNumber() { return getIntParam(XI_PRM_DEVICE_SN); }
        bool supportsColor() { return getIntParam(XI_PRM_IMAGE_IS_COLOR) == 1; }
        int getAvailableBandwidth() { return getIntParam(XI_PRM_AVAILABLE_BANDWIDTH); }
		int getExposure() { return getIntParam(XI_PRM_EXPOSURE); }
        int getGain() { return getIntParam(XI_PRM_GAIN); }

        void setFormat(int format) { setIntParam(XI_PRM_IMAGE_DATA_FORMAT, format); }
        void setExposure(int microseconds) { setIntParam(XI_PRM_EXPOSURE, microseconds); }
        void setGain(float value) { setIntParam(XI_PRM_GAIN, value); }
        void setDownsampling(int times) { setIntParam(XI_PRM_DOWNSAMPLING, times); }
        void setWhiteBalanceRed(float value) { setFloatParam(XI_PRM_WB_KR, value); }
        void setWhiteBalanceGreen(float value) { setFloatParam(XI_PRM_WB_KG, value); }
        void setWhiteBalanceBlue(float value) { setFloatParam(XI_PRM_WB_KB, value); }
        void setLuminosityGamma(float value) { setFloatParam(XI_PRM_GAMMAY, value); }
        void setChromaticityGamma(float value) { setFloatParam(XI_PRM_GAMMAC, value); }

        std::string getStringParam(const char* name);
        int getIntParam(const char* name);
        float getFloatParam(const char* name);

        void setStringParam(const char* name, std::string value);
        void setIntParam(const char* name, int value);
        void setFloatParam(const char* name, float value);

    private:
		void* run();

        XI_IMG image;
        FrameRaw frameRaw;
        FrameYUYV frameYUV;
        HANDLE device;
        bool opened;
        bool yuvInitialized;
		bool running;
		bool acquisitioning;
		int serialNumber;
        int lastFrameNumber;
};

#endif // CAMERA_H
