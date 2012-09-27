Dash.Robot = function(socket) {
	this.socket = socket;
};

Dash.Robot.prototype.setController = function(name) {
	if (this.socket.isOpen()) {
		dash.dbg.log('! Setting controller: ' + name);
		
		this.socket.send('<set-controller:' + name + '>');
	}
};

Dash.Robot.prototype.kick = function() {
	if (this.socket.isOpen()) {
		dash.dbg.log('! Kicking');
		
		this.socket.send('<kick:100>');
	}
};

Dash.Robot.prototype.setTargetDir = function(x, y, omega) {
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

Dash.Robot.prototype.runTest = function() {
	dash.dbg.log('! Running current test');
	
	dash.socket.send('<test>');
};