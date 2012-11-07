<?php

function rebuild() {
	//echo exec('call "C:/soccerbot/kill.bat"')."\n";
	//echo exec('call "C:/soccerbot/update.bat"')."\n";
	echo exec('call "C:/soccerbot/build.bat"')."\n";
	//echo exec('C:/soccerbot/run.bat')."\n";
	//echo exec('start /B "test" C:/soccerbot/soccerbot.exe')."\n";
	//pclose(popen('start "bla" "C:/soccerbot/soccerbot.exe"', "r"));
}

function kill() {
	echo exec('C:/soccerbot/kill.bat');
}

function shutdown() {
	echo exec('C:/soccerbot/shutdown.bat');
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