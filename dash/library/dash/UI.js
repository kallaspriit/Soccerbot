Dash.UI = function() {
	this.states = [];
	this.keystates = {};
	this.stateSlider = null;
	this.currentStateIndex = 0;
	this.currentStateIndexWrap = null;
	this.stateCountWrap = null;
	this.keyboardController = null;
	this.keyboardControllerEnabled = true;
};

Dash.UI.prototype = new Dash.Bindable();

Dash.UI.Event = {
	KEY_DOWN: 'key-down',
	KEY_UP: 'key-up'
}

Dash.UI.prototype.init = function() {
	this.initDebugListener();
	this.initSlider();
	this.initSocket();
	this.initRobot();
	this.initKeyboardController();
	this.initKeyListeners();
	this.initControls();
	
	/*this.addState(new Dash.State(0.125, '#CC0', 0.125, 0.125, 0));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 1, 0.125 + 1, Math.PI / 4));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 1, Math.PI / 2));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 2, Math.PI));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 3, 0.125 + 1, Math.PI * 3.0 / 4.0));*/
};

Dash.UI.prototype.initDebugListener = function() {
	dash.dbg.bind(Dash.Debug.Event.CONSOLE, function(event) {
		if (
			typeof(console) == 'object'
			&& typeof(console.log) == 'function'
		) {
			var time = Dash.Util.getTime();

			var args = [time];
			
			for (var i = 0; i < event.args.length; i++) {
				args.push(event.args[i]);
			}
			
			console.log.apply(console, args);
		}
	});
	
	dash.dbg.bind(Dash.Debug.Event.LOG, function(event) {
		var wrap = $('#log'),
			msgClass = 'normal',
			firstChar = event.args[0].substr(0, 1);

		if (firstChar == '+') {
			msgClass = 'positive';
		} else if (firstChar == '-') {
			msgClass = 'negative';
		} else if (firstChar == '!') {
			msgClass = 'info';
		}

		if (wrap.find('DIV').length >= 200) {
			wrap.html('');
		}

		var content = event.args[0],
			arg,
			args = [],
			i;

		for (i = 1; i < event.args.length; i++) {
			arg = event.args[i];

			if (typeof(arg) == 'string') {
				content += ', ' + arg;
			} else {
				content += ', ' + JSON.stringify(arg);
			}

			args.push(arg);
		}

		wrap.append(
			'<div class="' + msgClass + '">' + content + '</div>'
		);
			
		wrap.prop(
			'scrollTop',
			wrap.prop('scrollHeight')
		);
	});
	
	dash.dbg.bind(Dash.Debug.Event.ERROR, function(event) {
		if (
			typeof(console) == 'object'
			&& typeof(console.log) == 'function'
		) {
			var time = Dash.Util.getTime();

			var args = [time];
			
			for (var i = 0; i < event.args.length; i++) {
				args.push(event.args[i]);
			}
			
			console.error.apply(console, args);
		}
	});
};

Dash.UI.prototype.initSlider = function() {
	var self = this;
	
	this.stateSlider = $('#state-slider');
	this.currentStateIndexWrap = $('#current-state-index');
	this.stateCountWrap = $('#state-count');
	
	this.stateSlider.change(function() {
		self.showState(self.stateSlider.val() - 1);
	});
	
	this.currentStateIndexWrap.bind('change keyup click', function(e) {
		var newIndex = parseInt(self.currentStateIndexWrap.val()) - 1;
		
		if (newIndex > self.states.length - 1) {
			newIndex = self.states.length - 1;
		} else if (newIndex < 0) {
			newIndex = 0;
		}
		
		self.currentStateIndexWrap.html(newIndex + 1);
		
		self.showState(newIndex);
	});
};

Dash.UI.prototype.initSocket = function() {
	var self = this;
	
	dash.socket.bind(Dash.Socket.Event.OPEN, function(e) {
		dash.dbg.log(
			'+ Socket server opened on ' + e.socket.host + ':' + e.socket.port
		);
			
		$('#connecting').hide();
	});
	
	dash.socket.bind(Dash.Socket.Event.CLOSE, function(e) {
		dash.dbg.log('- Socket server closed');
		
		$('#connecting').show();
	});
	
	dash.socket.bind(Dash.Socket.Event.ERROR, function(e) {
		dash.dbg.log('- Socket error occured: ' + e.message);
	});
	
	dash.socket.bind(Dash.Socket.Event.MESSAGE_SENT, function(e) {
		self.flashClass('#tx', 'active', 100);
	});
	
	dash.socket.bind(Dash.Socket.Event.MESSAGE_RECEIVED, function(e) {
		try {
			var message = JSON.parse(e.message.data);
			
			self.handleMessage(message);
		} catch (e) {
			dash.dbg.log('- Invalid message', e.message.data);
		}
		
		self.flashClass('#rx', 'active', 100);
	});
	
	dash.socket.open(dash.config.socket.host, dash.config.socket.port);
	
	window.setInterval(function() {
		if (dash.socket.getState() != Dash.Socket.State.OPEN) {
			$('#connecting').show();
			
			dash.socket.open(dash.config.socket.host, dash.config.socket.port);
		} else {
			$('#connecting').hide();
		}
	}, 1000);
};

Dash.UI.prototype.initRobot = function() {
	this.robot = new Dash.Robot(dash.socket);
};

Dash.UI.prototype.initKeyboardController = function() {
	this.keyboardController = new Dash.KeyboardController(this.robot);
};

Dash.UI.prototype.initKeyListeners = function() {
	var self = this;
	
	$(document.body).keydown(function(e) {
		if (typeof(self.keystates[e.keyCode]) == 'undefined' || self.keystates[e.keyCode] == false) {
			self.keystates[e.keyCode] = true;
			
			self.onKeyDown(e);
		}
		
		self.fire({
			type: Dash.UI.Event.KEY_DOWN,
			key: e.keyCode,
			event: e
		});
	});
	
	$(document.body).keyup(function(e) {
		if (typeof(self.keystates[e.keyCode]) == 'undefined' || self.keystates[e.keyCode] == true) {
			self.keystates[e.keyCode] = false;
			
			self.onKeyUp(e);
		}
		
		self.fire({
			type: Dash.UI.Event.KEY_UP,
			key: e.keyCode,
			event: e
		});
	});
	
	$(window).blur(function() {
		for (var key in self.keystates) {
			if (self.keystates[key] == true) {
				self.onKeyUp(key);
			}
		}
		
		self.keystates = {};
	});
};

Dash.UI.prototype.initControls = function() {
	var self = this;
	
	$('#state-info-btn').click(function() {
		self.showCurrentStateInfo();
		
		return false;
	}).bind('clickoutside', function() {
		self.hideStateInfo();
	});
};

Dash.UI.prototype.onKeyDown = function(e) {
	//dash.dbg.log('! Key down: ' + e.keyCode);
	
	if (this.keyboardControllerEnabled) {
		this.keyboardController.onKeyDown(e.keyCode);
	}
};

Dash.UI.prototype.onKeyUp = function(e) {
	//dash.dbg.log('! Key up: ' + e.keyCode);
	
	if (this.keyboardControllerEnabled) {
		this.keyboardController.onKeyUp(e.keyCode);
	}
};

Dash.UI.prototype.isKeyDown = function(key) {
	return typeof(this.keystates[key]) != 'undefined' && this.keystates[key] == true;
};

Dash.UI.prototype.handleMessage = function(message) {
	if (typeof(message.id) != 'string') {
		dash.dbg.log('- Unknown message', message);
	}
	
	switch (message.id) {
		case 'state':
			this.handleStateMessage(message.payload);
		break;
		
		default: 
			dash.dbg.log('- Unsupported message received: ' + message.id);
		break;
	}
};

Dash.UI.prototype.handleStateMessage = function(state) {
	this.addState(state);
};

Dash.UI.prototype.addState = function(state) {
	state.index = this.states.length;
	
	this.states.push(state);
	
	this.stateSlider.attr({
		max: this.states.length
	});
	
	this.stateCountWrap.html(this.states.length);
	
	if (this.states.length == 1 || this.currentStateIndex == this.states.length - 2) {
		this.showState(this.states.length - 1);
	}
};

Dash.UI.prototype.showState = function(index) {
	this.currentStateIndexWrap.val(index + 1);
	
	this.stateSlider.attr({
		value: index + 1
	});
	
	this.currentStateIndex = index;
	
	dash.renderer.renderState(this.states[index]);
};

Dash.UI.prototype.showCurrentStateInfo = function() {
	if (this.states.length == 0) {
		return;
	}
	
	var currentState = this.states[this.currentStateIndex];
	
	this.showStateInfo(currentState);
};

Dash.UI.prototype.showStateInfo = function(state) {
	$('#state-info').html(Dash.Util.highlightJSON(state)).show();
};

Dash.UI.prototype.hideStateInfo = function() {
	$('#state-info').html('').hide();
};

Dash.UI.prototype.flashClass = function(el, className, duration) {
	if (typeof(el) == 'string') {
		el = $(el);
	}
	
	var timeout = el.data('flash-timeout');
	
	if (timeout != null) {
		window.clearTimeout(timeout);
	}
	
	if (!el.hasClass(className)) {
		el.addClass(className);
	}
	
	el.data('flash-timeout', window.setTimeout(function() {
		el.removeClass(className);
	}, duration));
};