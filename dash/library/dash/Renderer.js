Dash.Renderer = function(id) {
	this.id = id || 'canvas';
	this.element = null;
	this.c = null;
	
	this.wheelGraphs = {
		FL: null,
		FR: null,
		RL: null,
		RR: null
	};
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
	
	this.wheelGraphs = {
		FL: new Dash.WheelGraph('wheel-graph-fl'),
		FR: new Dash.WheelGraph('wheel-graph-fr'),
		RL: new Dash.WheelGraph('wheel-graph-rl'),
		RR: new Dash.WheelGraph('wheel-graph-rr')
	};
	
	for (var name in this.wheelGraphs) {
		this.wheelGraphs[name].init();
	}
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


Dash.Renderer.prototype.drawIntersections = function(yellowDistance, blueDistance) {
	this.c.save();

	this.c.lineWidth = 1 / this.canvasToWorldRatio;

	if (yellowDistance !== -1) {
		this.c.strokeStyle = '#DD0';
		this.c.beginPath();
		this.c.arc(0, Dash.Config.field.height / 2, yellowDistance, 0, Math.PI * 2, true);
		this.c.closePath();
		this.c.stroke();
	}

	if (blueDistance !== -1) {
		this.c.strokeStyle = '#00F';
		this.c.beginPath();
		this.c.arc(Dash.Config.field.width, Dash.Config.field.height / 2, blueDistance, 0, Math.PI * 2, true);
		this.c.closePath();
		this.c.stroke();
	}

	this.c.restore();
};

Dash.Renderer.prototype.drawParticle = function(x, y) {
	this.c.save();

	this.c.fillStyle = '#F00';
	this.c.fillRect(x, y, 0.01, 0.01);

	this.c.restore();
};

Dash.Renderer.prototype.drawRuler = function() {
	this.c.save();
	
	this.c.beginPath();
	this.c.lineWidth = 0.3;
	this.c.strokeStyle = '#090';
	this.c.moveTo(1.0, 1.0);
	this.c.lineTo(3.0, 1.0);
	this.c.closePath();
	this.c.stroke();
	
	this.c.restore();
};

Dash.Renderer.prototype.renderState = function(state) {
	this.c.clearRect(-1, -1, this.width + 1, this.height + 1);
	
	//this.drawRuler();
	
	this.drawRobot(
		dash.config.robot.radius,
		state.targetGoal == 1 ? '#00F' : '#DD0',
		state.x,
		state.y,
		state.orientation
	);
		
	this.drawRobot(
		dash.config.robot.radius,
		'#CCC',
		-0.4,
		0.4,
		state.gyroOrientation
	);

	if (state.controllerState.odometerLocalizer !== null && typeof(state.controllerState.odometerLocalizer) === 'object') {
		this.drawRobot(
			dash.config.robot.radius / 2,
			'#600',
			state.controllerState.odometerLocalizer.x,
			state.controllerState.odometerLocalizer.y,
			state.controllerState.odometerLocalizer.orientation
		);
	}

	if (state.controllerState.intersectionLocalizer !== null && typeof(state.controllerState.intersectionLocalizer) === 'object') {
		this.drawIntersections(
			state.controllerState.intersectionLocalizer.yellowDistance,
			state.controllerState.intersectionLocalizer.blueDistance
		);

		this.drawRobot(
			dash.config.robot.radius / 2,
			'#660',
			state.controllerState.intersectionLocalizer.x,
			state.controllerState.intersectionLocalizer.y,
			state.controllerState.intersectionLocalizer.orientation
		);
	}

	if (state.controllerState.kalmanLocalizer !== null && typeof(state.controllerState.kalmanLocalizer) === 'object') {
		this.drawRobot(
			dash.config.robot.radius / 2,
			'#006',
			parseFloat(state.controllerState.kalmanLocalizer.x),
			parseFloat(state.controllerState.kalmanLocalizer.y),
				parseFloat(state.controllerState.kalmanLocalizer.orientation)
		);
	}

	if (state.controllerState.particleLocalizer !== null && typeof(state.controllerState.particleLocalizer) === 'object') {
		this.drawRobot(
			dash.config.robot.radius / 2,
			'#060',
			state.controllerState.particleLocalizer.x,
			state.controllerState.particleLocalizer.y,
			state.controllerState.particleLocalizer.orientation
		);

		for (var i = 0; i < state.controllerState.particleLocalizer.particles.length; i++) {
			this.drawParticle(
				state.controllerState.particleLocalizer.particles[i][0],
				state.controllerState.particleLocalizer.particles[i][1]
			);
		}
	}
		
	this.wheelGraphs.FL.render.apply(this.wheelGraphs.FL, [state, 'wheelFL']);
	this.wheelGraphs.FR.render.apply(this.wheelGraphs.FR, [state, 'wheelFR']);
	this.wheelGraphs.RL.render.apply(this.wheelGraphs.RL, [state, 'wheelRL']);
	this.wheelGraphs.RR.render.apply(this.wheelGraphs.RR, [state, 'wheelRR']);
};