Dash.Robot = function(socket) {
	this.socket = socket;
};

Dash.Robot.prototype.kick = function() {
	if (this.socket.isOpen()) {
		dash.dbg.log('! Kicking');
		
		this.socket.send('<kick:100>');
	} else {
		dash.dbg.log('- Unable to kick, socket is not open');
	}
};

Dash.Robot.prototype.setTargetDir = function(x, y, omega) {
	//dash.dbg.log('! Robot target', x, y, omega);
	
	if (this.socket.isOpen()) {
		this.socket.send('<target-vector:' + x + ',' + y + ',' + omega + '>');
	} else {
		dash.dbg.log('- Unable to set dir, socket is not open');
	}
};