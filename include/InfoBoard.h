#ifndef INFOBOARD_H
#define INFOBOARD_H

#include "Config.h"

#include <vector>

class Serial;

class InfoBoardListener {
	public:
		virtual void onGoRequestedChange(bool isGoRequested) = 0;
		virtual void onTargetSideChange(Side newTargetSide) = 0;
		virtual void onGyroChange(float deltaAngle) = 0;
};

class InfoBoard {
	public:
		InfoBoard(int serialId);
		~InfoBoard();

		void raiseError() { errorRaised = 1; }
		void removeError() { errorRaised = 0; }
		Side getTargetSide() { return targetSide; }
		void setTargetSide(Side side);
		bool isError() { return errorRaised; }
		bool isGo() { return goRequested; }
		void setGo(bool mode);
		void addListener(InfoBoardListener* listener);
		void step(double dt);

	private:
		int serialId;
		bool errorRaised;
		bool goReceived;
		bool goRequested;
		bool firstAngle;
		Side targetSide;
		Serial* serial;
		std::vector<InfoBoardListener*> listeners;
};

#endif //INFOBOARD_H