<?php

function rebuild() {
	echo exec('bash /home/rx/projects/soccerbot/pull-make-release.sh');
}

function kill() {
	echo exec('bash /home/rx/projects/soccerbot/kill.sh');
}

function shutdown() {
	echo exec('echo "qwerty" | sudo -S shutdown -hP now > /home/rx/projects/soccerbot/test.log');
}

if (!isset($_GET['action'])) {
	echo 'Missing action';
	
	return;
}

switch ($_GET['action']) {
	case 'rebuild':
		rebuild();
	break;

	case 'kill':
		kill();
	break;

	case 'shutdown':
		shutdown();
	break;

	default:
		echo 'Unknown action: '.$_GET['action'];
	break;
}