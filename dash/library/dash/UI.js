Dash.UI = function() {
	this.states = [];
	this.stateSlider = null;
	this.currentStateIndex = 0;
	this.currentStateIndexWrap = null;
	this.stateCountWrap = null;
};

Dash.UI.prototype.init = function() {
	var self = this;
	
	this.stateSlider = $('#state-slider');
	this.currentStateIndexWrap = $('#current-state-index');
	this.stateCountWrap = $('#state-count');
	
	this.stateSlider.slider({
		min: 1,
		max: 0,
		step: 1,
		value: 0
	});
	
	this.stateSlider.bind('slide', function(e, ui) {
		self.showState(ui.value);
	});
	
	this.currentStateIndexWrap.bind('change keyup click', function(e) {
		var newIndex = parseInt(self.currentStateIndexWrap.val());
		
		if (newIndex > self.states.length) {
			newIndex = self.states.length;
		} else if (newIndex < 1) {
			newIndex = 1;
		}
		
		self.showState(newIndex);
	});
	
	this.addState(new Dash.State(0.125, '#CC0', 0.125, 0.125, 0));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 1, 0.125 + 1, Math.PI / 4));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 1, Math.PI / 2));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 2, 0.125 + 2, Math.PI));
	this.addState(new Dash.State(0.125, '#CC0', 0.125 + 3, 0.125 + 1, Math.PI * 3.0 / 4.0));
};

Dash.UI.prototype.addState = function(state) {
	this.states.push(state);
	
	this.stateCountWrap.html(this.states.length);
	this.stateSlider.slider('option', 'max', this.states.length);

	if (this.states.length == 1 || this.currentStateIndex == this.states.length - 1) {
		this.showState(this.states.length);
	}
};

Dash.UI.prototype.showState = function(index) {
	this.currentStateIndexWrap.val(index);
	this.stateSlider.slider('option', 'value', index);
	
	this.currentStateIndex = index;
	
	dash.renderer.renderState(this.states[index - 1]);
};