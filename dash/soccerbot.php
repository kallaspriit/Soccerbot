<?php

function rebuild() {
	echo exec('bash /home/rx/projects/soccerbot/pull-make-release.sh');
}

function kill() {
	echo exec('bash /home/rx/projects/soccerbot/kill.sh');
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

	default:
		echo 'Unknown action: '.$_GET['action'];
	break;
}