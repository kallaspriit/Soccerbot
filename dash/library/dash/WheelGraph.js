Dash.WheelGraph = function(id) {
	this.id = id;
	this.element = null;
	this.c = null;
};

Dash.WheelGraph.prototype.init = function() {
	this.element = document.getElementById(this.id);
	this.c = this.element.getContext('2d');
	this.element.width = this.width = $('#' + this.id).width();
	this.element.height = this.height = $('#' + this.id).height();
};

Dash.WheelGraph.prototype.render = function(state, name) {
	this.c.clearRect(0, 0, this.width, this.height);
	
	var x = this.width,
		currentState = state,
		multiplier = 2.0,
		first = true,
		skipCount = 1,
		info,
		targetOmega,
		realOmega,
		y,
		i;
	
	// draw target omega
	this.c.strokeStyle = '#F00';
	this.c.beginPath();
	
	while (currentState != null && x >= 0) {
		info = currentState[name];
		targetOmega = parseFloat(info.targetOmega);
		y = targetOmega * multiplier + this.height / 2;

		if (first) {
			this.c.moveTo(x, y);
			first = false;
		} else {
			this.c.lineTo(x, y);
		}
		
		x--;
		
		for (i = 0; i < skipCount; i++) {
			currentState = currentState.previous;
			
			if (currentState == null) {
				break;
			}
		}
	}
	
	this.c.stroke();
	
	// draw actual omega
	x = this.width;
	currentState = state;
		
	this.c.strokeStyle = '#0F0';
	this.c.beginPath();
	
	while (currentState != null && x >= 0) {
		info = currentState[name];
		realOmega = parseFloat(info.realOmega);
		y = realOmega * multiplier + this.height / 2;

		if (first) {
			this.c.moveTo(x, y);
			first = false;
		} else {
			this.c.lineTo(x, y);
		}
		
		x--;
		
		for (i = 0; i < skipCount; i++) {
			currentState = currentState.previous;
			
			if (currentState == null) {
				break;
			}
		}
	}
	
	this.c.stroke();
};