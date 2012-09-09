window.Dash = {};

$(document).ready(function() {
	window.dash = {
		renderer: new Dash.Renderer(),
		ui: new Dash.UI()
	};
	
	dash.renderer.init();
	dash.ui.init();
});