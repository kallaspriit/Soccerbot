Dash.Robot = function(socket) {
	this.socket = socket;
};

Dash.Robot.prototype.kick = function() {
	dash.dbg.log('! Kick requested');
};

Dash.Robot.prototype.setTargetDir = function(x, y, omega) {
	//dash.dbg.log('! Robot target', x, y, omega);
	
	this.socket.send('<target-vector:' + x + ',' + y + ',' + omega + '>');
};