#include "Camera.h"
#include "Util.h"

#include <iostream>

Camera::Camera() : opened(false), yuvInitialized(false), acquisitioning(false) {
    image.size = sizeof(XI_IMG);
    image.bp = NULL;
    image.bp_size = 0;
    device = NULL;
	serialNumber = 0;

    frameRaw.data = NULL;
    frameYUV.data = NULL;
    frameYUV.dataY = NULL;
    frameYUV.dataU = NULL;
    frameYUV.dataV = NULL;
}

Camera::~Camera() {
    close();
}

bool Camera::open(int serial) {
	std::cout << "! Searching for a camera with serial: " << serial << std::endl;

    DWORD deviceCount = 0;
    xiGetNumberDevices(&deviceCount);

    if (deviceCount == 0) {
        std::cout << "- Failed to detect any cameras" << std::endl;

        return false;
    }

	if (serial != 0) {
		std::cout << "  > found " << deviceCount << " available devices" << std::endl;
	}

    int sn = 0;

    bool found = false;

    for (unsigned int i = 0; i < deviceCount; i++) {
		std::cout << "  > opening camera #" << i << ".. ";
        xiOpenDevice(i, &device);
		std::cout << "done!" << std::endl;

        xiGetParamInt(device, XI_PRM_DEVICE_SN, &sn);
        std::cout << "  > found camera with serial number: " << sn << ".. ";

        if (serial == 0 || serial == sn) {
            found = true;

			std::cout << "match found!" << std::endl;

			break;
        } else {
			std::cout << "not the right one, closing it" << std::endl;

            xiCloseDevice(device);
        }
    }

    if (!found) {
        std::cout << "- No camera with serial #" << serial << " could be found" << std::endl;

        return false;
    }

    xiSetParamInt(device, XI_PRM_EXPOSURE, 16000);
    //xiSetParamInt(device, XI_PRM_IMAGE_DATA_FORMAT, XI_MONO8);
    //xiSetParamInt(device, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24);
    xiSetParamInt(device, XI_PRM_BUFFER_POLICY, XI_BP_UNSAFE);
    //xiSetParamInt(device, XI_PRM_FRAMERATE, 60);
    //xiSetParamInt(device, XI_PRM_DOWNSAMPLING, 2); // @TEMP
    //xiSetParamInt(device, XI_PRM_DOWNSAMPLING_TYPE, XI_BINNING);
    //xiSetParamFloat(device, XI_PRM_GAIN, 5.0f);
    //xiSetParamInt(device, XI_PRM_ACQ_BUFFER_SIZE, 70*1000*1000);
    xiSetParamInt(device, XI_PRM_BUFFERS_QUEUE_SIZE, 1);
    //xiSetParamInt(device, XI_PRM_RECENT_FRAME, 1);
    xiSetParamInt(device, XI_PRM_AUTO_WB, 0);
    //xiSetParamFloat(device, XI_PRM_WB_KR, 1.0f);
    //xiSetParamFloat(device, XI_PRM_WB_KG, 1.0f);
    //xiSetParamFloat(device, XI_PRM_WB_KB, 1.0f);
    //xiSetParamFloat(device, XI_PRM_GAMMAY, 1.0f);
    //xiSetParamFloat(device, XI_PRM_GAMMAC, 1.0f);
    //xiSetParamFloat(device, XI_PRM_SHARPNESS, 0.0f);
    xiSetParamInt(device, XI_PRM_AEAG, 0);
    //xiSetParamInt(device, XI_PRM_BPC, 1); // fixes bad pixel
    //xiSetParamInt(device, XI_PRM_HDR, 1);

    opened = true;
	serialNumber = serial;

    return true;
}

Camera::FrameRaw* Camera::getFrame() {
	if (!opened) {
		return NULL;
	}

    xiGetImage(device, 100, &image);

    if (image.bp == NULL) {
        return NULL;
    }

    frameRaw.data = (unsigned char*)image.bp;
    frameRaw.size = image.bp_size;
    frameRaw.number = image.nframe;
    frameRaw.width = image.width;
    frameRaw.height = image.height;
    frameRaw.timestamp = (double)image.tsSec + (double)image.tsUSec / 1000000.0;
    frameRaw.fresh = frameRaw.number != lastFrameNumber;

    lastFrameNumber = frameRaw.number;

    return &frameRaw;
}

/*
const Camera::FrameYUV& Camera::getFrameYUV() {
    xiGetImage(device, 100, &image);

    frameYUV.data = (unsigned char*)image.bp;
    frameYUV.size = image.bp_size;
    frameYUV.number = image.nframe;
    frameYUV.width = image.width;
    frameYUV.height = image.height;
    frameYUV.timestamp = (double)image.tsSec + (double)image.tsUSec / 1000000.0d;
    frameYUV.fresh = frameYUV.number != lastFrameNumber;

    if (!yuvInitialized) {
        frameYUV.strideY = frameYUV.width;
        frameYUV.strideU = (frameYUV.width + 1) / 2;
        frameYUV.strideV = (frameYUV.width + 1) / 2;

        frameYUV.dataY = new uint8[frameYUV.width * frameYUV.height];
        frameYUV.dataU = new uint8[(frameYUV.width / 2) * (frameYUV.height / 2)];
        frameYUV.dataV = new uint8[(frameYUV.width / 2) * (frameYUV.height / 2)];
        frameYUV.dataYUV = new uint8[frameYUV.width * frameYUV.height * 3];

        yuvInitialized = true;
    }

    lastFrameNumber = frameYUV.number;

    libyuv::BayerRGGBToI420(
        frameYUV.data,
        frameYUV.width,
        frameYUV.dataY,
        frameYUV.strideY,
        frameYUV.dataU,
        frameYUV.strideU,
        frameYUV.dataV,
        frameYUV.strideV,
        frameYUV.width,
        frameYUV.height
    );

    int row;
    int col;
    int indexUV;

    for (int i = 0; i < frameYUV.width * frameYUV.height; i++) {
        row = i / frameYUV.width;
        col = i - row * frameYUV.width;
        indexUV = (row / 2) * (frameYUV.width / 2) + (col / 2);

        frameYUV.dataYUV[i * 3] = frameYUV.dataY[i];
        frameYUV.dataYUV[i * 3 + 1] = frameYUV.dataU[indexUV];
        frameYUV.dataYUV[i * 3 + 2] = frameYUV.dataV[indexUV];
    }

    return frameYUV;
}
*/

Camera::FrameYUYV* Camera::getFrameYUYV() {
	if (!opened) {
		return NULL;
	}

    //double s = Util::millitime();

    xiGetImage(device, 100, &image);

    if (image.bp == NULL) {
        return NULL;
    }

    //std::cout << "Get: " << (Util::millitime() - s) << std::endl;

    frameYUV.data = (unsigned char*)image.bp;
    frameYUV.size = image.bp_size;
    frameYUV.number = image.nframe;
    frameYUV.width = image.width;
    frameYUV.height = image.height;
    frameYUV.timestamp = (double)image.tsSec + (double)image.tsUSec / 1000000.0;
    frameYUV.fresh = frameYUV.number != lastFrameNumber;

    if (!yuvInitialized) {
        frameYUV.strideY = frameYUV.width;
        frameYUV.strideU = (frameYUV.width + 1) / 2;
        frameYUV.strideV = (frameYUV.width + 1) / 2;

        frameYUV.dataY = new uint8[frameYUV.width * frameYUV.height];
        frameYUV.dataU = new uint8[(frameYUV.width / 2) * (frameYUV.height / 2)];
        frameYUV.dataV = new uint8[(frameYUV.width / 2) * (frameYUV.height / 2)];
        frameYUV.dataYUYV = new uint8[frameYUV.width * frameYUV.height * 3];

        yuvInitialized = true;
    }

    lastFrameNumber = frameYUV.number;

    //double s = Util::millitime();

    libyuv::BayerRGGBToI420(
        frameYUV.data,
        frameYUV.width,
        frameYUV.dataY,
        frameYUV.strideY,
        frameYUV.dataU,
        frameYUV.strideU,
        frameYUV.dataV,
        frameYUV.strideV,
        frameYUV.width,
        frameYUV.height
    );

    //std::cout << "RGGB > I420: " << (Util::millitime() - s) << std::endl;

    //double s = Util::millitime();

	
    /*int row;
    int col;
    int indexUV;
    int elements = frameYUV.width * frameYUV.height;
    int halfWidth = frameYUV.width / 2;
    bool alt = false;

    for (int i = 0; i < elements; i++) {
        row = i / frameYUV.width;
        col = i - row * frameYUV.width;
        indexUV = (row >> 1) * halfWidth + (col >> 1);

        frameYUV.dataYUYV[i << 1] = frameYUV.dataY[i];
        frameYUV.dataYUYV[(i << 1) + 1] = alt ? frameYUV.dataV[indexUV] : frameYUV.dataU[indexUV];

        alt = !alt;
    }*/
	
	/*
	//bool alt = false;
	int index = 0;
	int uvIndex = 0;
	//int uvCounter = 0;

	int dstStride = frameYUV.width * 2;
	int uvStride = frameYUV.width / 2;

	for (int i = 0; i < frameYUV.width * frameYUV.height; i += 4) {
		frameYUV.dataYUYV[index] = frameYUV.dataY[i];
		frameYUV.dataYUYV[index + 1] = frameYUV.dataU[uvIndex];
		frameYUV.dataYUYV[index + dstStride] = frameYUV.dataY[i + 1];
		frameYUV.dataYUYV[index + dstStride + 1] = frameYUV.dataV[uvIndex];
		
		frameYUV.dataYUYV[index + 4] = frameYUV.dataY[i + 2];
		frameYUV.dataYUYV[index + 5] = frameYUV.dataU[uvIndex];
		frameYUV.dataYUYV[index + 6] = frameYUV.dataY[i + 3];
		frameYUV.dataYUYV[index + 7] = frameYUV.dataV[uvIndex];

		//alt = !alt;
		index += 8;
		uvIndex++;
	}
	*/

	// for each U,V pixel create four destination pixels
	// most time i've ever spent figuring out an algorithm..
	int dstIndex = 0;
	int yIndex = 0;
	int dstStride = frameYUV.width * 2;
	int yStride = frameYUV.width;
	int uvStride = frameYUV.width / 2;
	int row = 0;
	int pixelsInRow = 0;
	int elementCount = (frameYUV.width / 2) * (frameYUV.height / 2);

	for (int uvIndex = 0; uvIndex < elementCount; uvIndex++) {
		frameYUV.dataYUYV[dstIndex]     = frameYUV.dataY[yIndex];
		frameYUV.dataYUYV[dstIndex + 1] = frameYUV.dataU[uvIndex];
		frameYUV.dataYUYV[dstIndex + 2] = frameYUV.dataY[yIndex + 1];
		frameYUV.dataYUYV[dstIndex + 3] = frameYUV.dataV[uvIndex];

		frameYUV.dataYUYV[dstIndex + dstStride]     = frameYUV.dataY[yIndex + yStride];
		frameYUV.dataYUYV[dstIndex + dstStride + 1] = frameYUV.dataU[uvIndex];
		frameYUV.dataYUYV[dstIndex + dstStride + 2] = frameYUV.dataY[yIndex + yStride + 1];
		frameYUV.dataYUYV[dstIndex + dstStride + 3] = frameYUV.dataV[uvIndex];

		dstIndex += 4;
		yIndex += 2;
		pixelsInRow += 2;

		if (pixelsInRow == frameYUV.width) {
			row += 2;
			yIndex = row * yStride;
			dstIndex = row * dstStride;
			pixelsInRow = 0;
		}
	}


    //std::cout << "I420 > YUYV: " << (Util::millitime() - s) << std::endl;

    return &frameYUV;
}

void* Camera::run() {
	running = true;

	while (running) {
		Camera::FrameYUYV* frame = getFrameYUYV();

		if (frame == NULL) {
			std::cout << "- Didn't get a frame from camera #" << serialNumber << std::endl;
		}
	}

	return NULL;
}

void Camera::startAcquisition() {
    if (!opened) {
        return;
    }

    xiStartAcquisition(device);

	running = true;
	acquisitioning = true;

	start();
}

void Camera::stopAcquisition() {
    if (!opened) {
        return;
    }

	running = false;

    xiStopAcquisition(device);

	running = false;
	acquisitioning = false;

	join();
}

void Camera::close() {
    if (opened) {
		if (acquisitioning) {
			stopAcquisition();
		}

        xiCloseDevice(device);

        device = NULL;

		opened = false;
    }
}

std::string Camera::getStringParam(const char* name) {
	if (!opened) {
		return "n/a";
	}

    char stringParam[254];

    xiGetParamString(device, name, stringParam, sizeof(stringParam));

    return std::string(stringParam);
}

int Camera::getIntParam(const char* name) {
	if (!opened) {
		return -1;
	}

    int intParam = 0;

    xiGetParamInt(device, name, &intParam);

    return intParam;
}

float Camera::getFloatParam(const char* name) {
	if (!opened) {
		return -1.0f;
	}

    float floatParam = 0;

    xiGetParamFloat(device, name, &floatParam);

    return floatParam;
}

void Camera::setStringParam(const char* name, std::string value) {
	if (!opened) {
		return;
	}

    xiSetParamString(device, name, (void*)value.c_str(), value.length());
}

void Camera::setIntParam(const char* name, int value) {
	if (!opened) {
		return;
	}

    xiSetParamInt(device, name, value);
}

void Camera::setFloatParam(const char* name, float value) {
	if (!opened) {
		return;
	}

    xiSetParamFloat(device, name, value);
}
