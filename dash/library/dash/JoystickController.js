Dash.JoystickController = function(robot) {
	this.robot = robot;
	this.gamepad = null;
	this.useGamepad = false;
	this.enabled = true;
	this.fastMode = false;
	
	this.init();
};

Dash.JoystickController.prototype.init = function() {
	var self = this;
	
	this.gamepad = new Gamepad();
	
	this.gamepad.bind(Gamepad.Event.CONNECTED, function(device) {
		dash.dbg.log('! Controller connected', device);

		$('#gamepad').html(device.id);
	});

	this.gamepad.bind(Gamepad.Event.DISCONNECTED, function(device) {
		dash.dbg.log('! Controller disconnected', device);
		
		$('#gamepad').html('Gamepad disconnected');
		
		self.useGamepad = false;
	});

	this.gamepad.bind(Gamepad.Event.UNSUPPORTED, function(device) {
		$('#gamepad').html('Unsupported controller connected');
		
		dash.dbg.log('- Unsupported controller connected', device);
	});

	this.gamepad.bind(Gamepad.Event.TICK, function(gamepads) {
		self.onTick(gamepads);
	});

	$('#gamepad').html('Your browser supports gamepads, try connecting one');

	if (!this.gamepad.init()) {
		$('#gamepad').html('If you use latest Google Chrome or Firefox, you can use gamepads..');
	}
};

Dash.JoystickController.prototype.onTick = function(gamepads) {
	if (
		gamepads[0].state.RIGHT_STICK_X != 0
		|| gamepads[0].state.RIGHT_STICK_Y != 0
		|| gamepads[0].state.LEFT_STICK_X != 0
		|| gamepads[0].state.LEFT_STICK_Y != 0
	) {
		this.useGamepad = true;
	}

	if (!this.useGamepad || !this.enabled) {
		return;
	}
	
	var speed = dash.config.joystick.speed,
		turnRate = dash.config.joystick.turnRate;
		
	if (this.fastMode) {
		speed *= 2;
		turnRate *= 2;
	}

	this.robot.setTargetDir(
		gamepads[0].state.RIGHT_STICK_Y * -speed,
		gamepads[0].state.RIGHT_STICK_X * speed,
		gamepads[0].state.LEFT_STICK_X * turnRate
	);
		
	if (gamepads[0].state.Y) {
		this.fastMode = !this.fastMode;
	}

	if (gamepads[0].state.LB || gamepads[0].state.RB) {
		this.robot.kick();
	}
};