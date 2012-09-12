Dash.UI = function() {
	this.states = [];
	this.stateSlider = null;
	this.currentStateIndex = 0;
	this.currentStateIndexWrap = null;
	this.stateCountWrap = null;
};

Dash.UI.prototype.init = function() {
	this.initSlider();
	this.initSocket();
	
	this.addState(new Dash.State(0.125, '#CC0', 0.125, 0.125, 0));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 1, 0.125 + 1, Math.PI / 4));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 1, Math.PI / 2));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 2, Math.PI));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 3, 0.125 + 1, Math.PI * 3.0 / 4.0));
};

Dash.UI.prototype.initSlider = function() {
	var self = this;
	
	this.stateSlider = $('#state-slider');
	this.currentStateIndexWrap = $('#current-state-index');
	this.stateCountWrap = $('#state-count');
	
	$("#state-slider").empty().noUiSlider('init', {
		//step: 1,
		knobs: 1,
		connect: 'lower',
		scale: [1, 1],
		change: function(){
			var value = $(this).noUiSlider('value')[1];
			
			self.showState(value);
			
			console.log('value', value);

			/*$(this).find('.noUi-lowerHandle .infoBox').text(values[0]);
			$(this).find('.noUi-upperHandle .infoBox').text(values[1]);*/

		},
		end: function(type){
			console.log('end', type);
			
			/*var message = "The slider was last changed by ";

			if ( type=="click" ){ message=message+"clicking the bar."; }
			else if ( type=="slide" ){ message=message+"sliding a knob."; }
			else if ( type=="move" ){ message=message+"calling the move function."; }

			$("#slideType").fadeOut(function(){

				$(this).text(message).fadeIn();

			});*/
		}
	});
	
	/*this.stateSlider.slider({
		min: 1,
		max: 0,
		step: 1,
		value: 0
	});
	
	this.stateSlider.bind('slide', function(e, ui) {
		self.showState(ui.value);
	});*/
	
	this.currentStateIndexWrap.bind('change keyup click', function(e) {
		var newIndex = parseInt(self.currentStateIndexWrap.val());
		
		if (newIndex > self.states.length) {
			newIndex = self.states.length;
		} else if (newIndex < 1) {
			newIndex = 1;
		}
		
		self.showState(newIndex);
	});
};

Dash.UI.prototype.initSocket = function() {
	dash.socket.bind(Dash.Socket.Event.OPEN, function(e) {
		dash.dbg.log(
			'Socket server opened on ' + e.socket.host + ':' + e.socket.port
		);
	});
	
	dash.socket.open('rx', 8080);
};

Dash.UI.prototype.addState = function(state) {
	this.states.push(state);
	
	this.stateCountWrap.html(this.states.length);
	this.stateSlider.noUiSlider('move', {
		knob: 0,
		scale: [1, this.states.length]
	});

	if (this.states.length == 1 || this.currentStateIndex == this.states.length - 1) {
		this.showState(this.states.length);
	}
};

Dash.UI.prototype.showState = function(index) {
	this.currentStateIndexWrap.val(index);
	this.stateSlider.noUiSlider('move', {
		knob: 0,
		to: index
	});
	//this.stateSlider.noUiSlider('init', 'scale', [1, this.states.length]);
	
	this.currentStateIndex = index;
	
	dash.renderer.renderState(this.states[index - 1]);
};