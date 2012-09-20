Dash.UI = function() {
	this.states = [];
	this.keystates = {};
	this.reconnectTimeout = null;
	this.stateSlider = null;
	this.currentStateIndexWrap = null;
	this.stateCountWrap = null;
	this.keyboardController = null;
	this.joystickController = null;
	this.lastLogMessage = null;
	this.rxCounter = null;
	this.currentStateIndex = 0;
	this.repeatedLogCount = 0;
	
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
	this.initFpsCounter();
	this.initKeyboardController();
	this.initJoystickController();
	this.initKeyListeners();
	this.initControls();
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
	
	dash.dbg.bind([Dash.Debug.Event.LOG, Dash.Debug.Event.EXTERNAL], function(event) {
		var wrap = $('#log'),
			msgClass = 'normal',
			message = event.args[0],
			firstChar = message.substr(0, 1);
			
		if (message == this.lastLogMessage) {
			this.repeatedLogCount++;
			
			message = message + ' (' + (this.repeatedLogCount + 1) + ')';
			
			wrap.find('DIV:last').remove();
		}else {
			this.lastLogMessage = message;
			this.repeatedLogCount = 0;
		}

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

		var content = message,
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
		
		if (event.type == Dash.Debug.Event.EXTERNAL) {
			msgClass += ' external';
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
	
	$('#log').mousedown(function(e) {
		if (e.which == 3) {
			$(this).empty();
		}
	});
	
	$('#log').bind('contextmenu', function(e) {
		if (e.which == 3) {
			return false;
		}
	});
	
	$('#log').hover(
		function() {
			$(this).stop(true, false).animate({
				width: '1200px'
			}, 300);
		},
		function() {
			$(this).stop(true, false).animate({
				width: '300px'
			}, 150);
		}
	);
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
		if (self.reconnectTimeout != null) {
			window.clearTimeout(self.reconnectTimeout);
			
			self.reconnectTimeout = null;
		}
		
		dash.dbg.log(
			'+ Socket server opened on ' + e.socket.host + ':' + e.socket.port
		);
			
		$('#connecting').hide();
		$('.live-only').removeAttr('disabled');
		$('#rebuild-btn').text('Rebuild');
	});
	
	dash.socket.bind(Dash.Socket.Event.CLOSE, function(e) {
		dash.dbg.log('- Socket server closed');
		
		$('#connecting').show();
		$('.live-only').attr('disabled', 'disabled');
		
		if (self.reconnectTimeout != null) {
			window.clearTimeout(self.reconnectTimeout);
			
			self.reconnectTimeout = null;
		}
		
		self.reconnectTimeout = window.setTimeout(function() {
			dash.socket.open(dash.config.socket.host, dash.config.socket.port);
		}, 1000);
	});
	
	dash.socket.bind(Dash.Socket.Event.ERROR, function(e) {
		dash.dbg.log('- Socket error occured: ' + e.message);
	});
	
	dash.socket.bind(Dash.Socket.Event.MESSAGE_SENT, function(e) {
		self.flashClass('#tx', 'active', 100);
	});
	
	dash.socket.bind(Dash.Socket.Event.MESSAGE_RECEIVED, function(e) {
		var message;
		
		try {
			message = JSON.parse(e.message.data);
		} catch (ex) {
			dash.dbg.log('- Invalid message', e.message.data);
			
			return;
		}
		
		self.handleMessage(message);
		
		self.flashClass('#rx', 'active', 100);
	});
	
	dash.socket.open(dash.config.socket.host, dash.config.socket.port);
	
	/*window.setInterval(function() {
		if (dash.socket.getState() != Dash.Socket.State.OPEN) {
			$('#connecting').show();
			
			dash.socket.open(dash.config.socket.host, dash.config.socket.port);
		} else {
			$('#connecting').hide();
		}
	}, 1000);*/
};

Dash.UI.prototype.initRobot = function() {
	this.robot = new Dash.Robot(dash.socket);
};

Dash.UI.prototype.initFpsCounter = function() {
	this.rxCounter = new Dash.FpsCounter(function(fps) {
		console.log('rx.fps', fps);
	});
};

Dash.UI.prototype.initKeyboardController = function() {
	this.keyboardController = new Dash.KeyboardController(this.robot);
	this.keyboardController.enabled = true;
};

Dash.UI.prototype.initJoystickController = function() {
	this.joystickController = new Dash.JoystickController(this.robot);
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
	
	$('INPUT[name="keyboard-controller-enabled"]').iphoneStyle({
		onChange: function(elem, enabled) {
			self.keyboardController.enabled = enabled;
		}
	});
	
	$('INPUT[name="joystick-controller-enabled"]').iphoneStyle({
		onChange: function(elem, enabled) {
			self.joystickController.enabled = enabled;
			
			if (enabled) {
				$('INPUT[name="keyboard-controller-enabled"]')
					.removeAttr('checked')
					.iphoneStyle('refresh')
					.attr('disabled', 'disabled')
					.iphoneStyle('refresh');
			} else {
				$('INPUT[name="keyboard-controller-enabled"]')
					.removeAttr('disabled')
					.iphoneStyle('refresh');
			}
		}
	});
	
	$('#reset-position-btn').click(function() {
		self.robot.resetPosition();
	});
	
	$('#rebuild-btn').click(function() {
		var btn = $(this);
		
		btn.html('Building..').attr('disabled', 'disabled');
		
		self.rebuild(function() {
			btn.removeAttr('disabled').html('Rebuild');
		});
	});
	
	$('#kill-btn').click(function() {
		var btn = $(this);
		
		btn.html('Killing it..').attr('disabled', 'disabled');
		
		self.kill(function() {
			btn.removeAttr('disabled').html('Kill');
		});
	});
	
	$('#shutdown-btn').click(function() {
		var btn = $(this);
		
		btn.html('Shutting down..').attr('disabled', 'disabled');
		
		self.shutdown(function() {
			btn.removeAttr('disabled').html('Shutdown');
		});
	});
	
	$('#turn-btn').click(function() {
		self.robot.turnBy(Math.PI / 2.0, 1);
	});
};

Dash.UI.prototype.onKeyDown = function(e) {
	//dash.dbg.log('! Key down: ' + e.keyCode);
	
	if (this.keyboardController.enabled) {
		this.keyboardController.onKeyDown(e.keyCode);
	}
};

Dash.UI.prototype.onKeyUp = function(e) {
	//dash.dbg.log('! Key up: ' + e.keyCode);
	
	if (this.keyboardController.enabled) {
		this.keyboardController.onKeyUp(e.keyCode);
	}
};

Dash.UI.prototype.isKeyDown = function(key) {
	return typeof(this.keystates[key]) != 'undefined' && this.keystates[key] == true;
};

Dash.UI.prototype.handleMessage = function(message) {
	if (typeof(message.id) != 'string') {
		dash.dbg.log('- Unknown message', message);
		
		return;
	}
	
	switch (message.id) {
		case 'state':
			this.handleStateMessage(message.payload);
		break;
		
		case 'log':
			this.handleLogMessage(message.payload);
		break;
		
		default:
			dash.dbg.log('- Unsupported message received: ' + message.id);
		break;
	}
	
	this.rxCounter.step();
};

Dash.UI.prototype.handleStateMessage = function(state) {
	this.addState(state);
};

Dash.UI.prototype.handleLogMessage = function(messages) {
	var lines = messages.split(/\n/g),
		i;
	
	for (i = 0; i < lines.length; i++) {
		if (lines[i].length > 0) {
			dash.dbg.external(lines[i]);
		}
	}
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
	var state = this.states[index];
	
	this.currentStateIndexWrap.val(index + 1);
	
	this.stateSlider.attr({
		value: index + 1
	});
	
	this.currentStateIndex = index;
	
	// @TODO show alive wheels..
	
	dash.renderer.renderState(state);
	
	this.showTasksQueue(state);
	this.showStateStats(state);
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

Dash.UI.prototype.showTasksQueue = function(state) {
	var wrap = $('#tasks'),
		i;
	
	wrap.empty();
	
	for (i = 0; i < state.tasks.length; i++) {
		wrap.append('<li>' + state.tasks[i] + '</li>');
	}
};

Dash.UI.prototype.showStateStats = function(state) {
	$('#time').html(Dash.Util.round(state.totalTime, 1) + 's (' + Dash.Util.round(state.dt * 1000, 1) + 'ms / ' + Dash.Util.round(state.duration * 1000, 2) + 'ms)');
	$('#load > SPAN').css('width', Math.ceil(state.load) + '%');
};

Dash.UI.prototype.rebuild = function(callback) {
	this.request('rebuild', callback);
};

Dash.UI.prototype.kill = function(callback) {
	this.request('kill', callback);
};

Dash.UI.prototype.shutdown = function(callback) {
	this.request('shutdown', callback);
};

Dash.UI.prototype.request = function(action, callback) {
	$.ajax({
		url: 'http://' + dash.config.socket.host + '/dash/soccerbot.php?action=' + action,
		type: 'GET',
		dataType: 'html',
		timeout: 5000
	}).success(function() {
		if (typeof(callback) == 'function') {
			callback(true);
		}
	}).fail(function() {
		if (typeof(callback) == 'function') {
			callback(false);
		}
		
		dash.dbg.log('- executing ' + action + ' failed');
	});
};

