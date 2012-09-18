Dash.Robot = function(socket) {
	this.socket = socket;
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