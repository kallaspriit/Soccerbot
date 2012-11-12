#ifndef INFOBOARD_H
#define INFOBOARD_H

#include "Config.h"

class Serial;

class InfoBoard {
	public:
		InfoBoard(int serialId);
		~InfoBoard();

		void raiseError() { errorRaised = 1; }
		void removeError() { errorRaised = 0; }
		Side getTargetSide() { return targetSide; }
		bool isError() { return errorRaised; }
		bool isGo() { return goRequested; }
		void step(double dt);

	private:
		int serialId;
		bool errorRaised;
		bool goRequested;
		Side targetSide;
		Serial* serial;
};

#endif //INFOBOARD_H