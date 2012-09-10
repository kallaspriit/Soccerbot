Dash.Socket = function() {
	this.host = null;
	this.port = null;
	this.ws = null;
};

Dash.Socket.prototype = new Dash.Bindable();

Dash.Socket.Event = {
	OPEN: 'open',
	CLOSE: 'close',
	MESSAGE: 'message',
	ERROR: 'error'
};

Dash.Socket.State = {
	READY: 0,
	OPEN: 1,
	CLOSING: 2,
	CLOSED: 3
};

Dash.Socket.prototype.open = function(host, port) {
	var self = this;
	
	this.host = host;
	this.port = port;
	this.ws = new WebSocket('ws://' + this.host + ':' + this.port);
	
	this.ws.onopen = function() {
		self.fire({
			type: Dash.Socket.Event.OPEN,
			socket: this
		});
	};
	
	this.ws.onclose = function() {
		self.fire({
			type: Dash.Socket.Event.CLOSE,
			socket: this
		});
	};
	
	this.ws.onerror = function(error) {
		self.fire({
			type: Dash.Socket.Event.ERROR,
			error: error,
			socket: this
		});
	};
	
	this.ws.onmessage = function(message) {
		self.fire({
			type: Dash.Socket.Event.ERROR,
			message: message
		});
	};
};

Dash.Socket.prototype.getState = function() {
	if (this.ws == null) {
		return Dash.Socket.State.CLOSED;
	} else {
		return this.ws.readyState;
	}
};

Dash.Socket.prototype.getBufferedAmount = function() {
	if (this.ws == null) {
		return 0;
	} else {
		return this.ws.bufferedAmount;
	}
};

Dash.Socket.prototype.close = function() {
	if (this.ws != null) {
		this.ws.close();
		this.ws = null;
	}
};