Dash.Renderer = function(id) {
	this.id = id || 'canvas';
	this.element = null;
	this.c = null;
};

Dash.Renderer.prototype.init = function() {
	this.element = document.getElementById(this.id);
	this.c = this.element.getContext('2d');
	
	this.widthToHeightRatio = 4.5 / 3.0;
	this.canvasWidth = 600;
	this.canvasHeight = this.canvasWidth / this.widthToHeightRatio;
	this.canvasToWorldRatio = this.canvasWidth / 4.5;
	this.fieldOffsetX = -(6.0 - 4.5) / 2;
	this.fieldOffsetY = -(4.0 - 3.0) / 2;
	
	this.c.setTransform(
		this.canvasToWorldRatio,
		0,
		0,
		this.canvasToWorldRatio,
		100,
		66
	);
		
	this.width = this.element.width;
	this.height = this.element.height;
};

Dash.Renderer.prototype.drawRobot = function(radius, color, x, y, orientation) {
	this.c.save();
	
	this.c.translate(x, y);
	this.c.rotate(orientation);
	this.c.fillStyle = color;
	this.c.beginPath();
	this.c.arc(0, 0, radius, 0, Math.PI * 2, true); 
	this.c.closePath();
	this.c.fill();
	
	var dirHeight = radius / 5;
	
	
	this.c.fillStyle = '#FFF';
	this.c.fillRect(0, -dirHeight / 2.0, radius, dirHeight);
	
	this.c.restore();
};

Dash.Renderer.prototype.renderState = function(state) {
	this.c.clearRect(-1, -1, this.width + 1, this.height + 1);
	
	this.drawRobot(
		dash.config.robot.radius,
		dash.config.robot.color,
		state.x,
		state.y,
		state.orientation
	);
};