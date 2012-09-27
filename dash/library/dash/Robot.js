Dash.Robot = function(socket) {
	this.socket = socket;
	this.controller = 'manual';
};

Dash.Robot.prototype.setController = function(name) {
	if (this.socket.isOpen() && name != this.controller) {
		dash.dbg.log('! Setting controller: ' + name);
		
		this.socket.send('<set-controller:' + name + '>');
		
		this.controller = name;
	}
};

Dash.Robot.prototype.kick = function() {
	if (this.socket.isOpen()) {
		dash.dbg.log('! Kicking');
		
		this.socket.send('<kick:100>');
	}
};

Dash.Robot.prototype.setTargetDir = function(x, y, omega) {
	if (this.controller != 'manual') {
		return;
	}
	
	//dash.dbg.log('! Robot target', x, y, omega);
	
	if (this.socket.isOpen()) {
		this.socket.send('<target-vector:' + x + ',' + y + ',' + omega + '>');
	}
};

Dash.Robot.prototype.resetPosition = function() {
	dash.dbg.log('! Resetting position');
	
	dash.socket.send('<reset-position>');
};

Dash.Robot.prototype.turnBy = function(angle, speed) {
	speed = typeof(speed) == 'number' ? speed : 1.0;
	
	dash.dbg.log('! Turning by ' + Dash.Util.radToDeg(angle) + '°');
	
	dash.socket.send('<turn-by:' + angle + ',' + speed + '>');
};

Dash.Robot.prototype.driveTo = function(x, y, orientation, speed) {
	speed = typeof(speed) == 'number' ? speed : 1.0;
	
	dash.dbg.log('! Driving to ' + x + 'x' + y + ' @ ' + Dash.Util.radToDeg(orientation) + '°');
	
	dash.socket.send('<drive-to:' + x + ',' + y + ',' + orientation + ',' + speed + '>');
};

Dash.Robot.prototype.testRectangle = function() {
	dash.dbg.log('! Running rectangle test');
	
	dash.socket.send('<test-rectangle>');
};