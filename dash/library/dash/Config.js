// Define the namespace
window.Dash = {};

Dash.Config = {
	socket: {
		host: 'localhost',
		port: 8000
	},
	robot: {
		radius: 0.125
	},
	keyboard: {
		speed: 0.5,
		turnRate: Math.PI
	},
	joystick: {
		speed: 1.5,
		turnRate: Math.PI * 2
	}
};