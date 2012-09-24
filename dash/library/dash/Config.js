// Define the namespace
window.Dash = {};

Dash.Config = {
	socket: {
		//host: 'rx',
		host: '192.168.1.120',
		port: 8000
	},
	robot: {
		color: '#DD0',
		radius: 0.125
	},
	keyboard: {
		speed: 0.5,
		turnRate: Math.PI
	},
	joystick: {
		speed: 1,
		turnRate: Math.PI * 2
	}
};