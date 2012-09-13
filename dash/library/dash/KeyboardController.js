Dash.KeyboardController = function(robot) {
	this.robot = robot;
};

Dash.KeyboardController.prototype.onKeyDown = function(key) {
	if (key == 32) {
		this.robot.kick();
	}
	
	this.updateRobotDir();
};

Dash.KeyboardController.prototype.onKeyUp = function(key) {
	this.updateRobotDir();
};

Dash.KeyboardController.prototype.updateRobotDir = function() {
	var forwardDown = dash.ui.isKeyDown(87),
		leftDown = dash.ui.isKeyDown(65),
		reverseDown = dash.ui.isKeyDown(83),
		rightDown = dash.ui.isKeyDown(68),
		turnRightDown = dash.ui.isKeyDown(69),
		turnLeftDown = dash.ui.isKeyDown(81),
		shiftDown = dash.ui.isKeyDown(16),
		speed = shiftDown ? 2.5 : 1,
		turnRate = Math.PI * 2,
		x = 0,
		y = 0,
		omega = 0;
	
	if (forwardDown) {
		x = speed;
	} else if (reverseDown) {
		x = -speed;
	}
	
	if (rightDown) {
		y = speed;
	} else if (leftDown) {
		y = -speed;
	}
	
	if (turnRightDown) {
		omega = turnRate;
	} else if (turnLeftDown) {
		omega = -turnRate;
	}
	
	this.robot.setTargetDir(x, y, omega);
};