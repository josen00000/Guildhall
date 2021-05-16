How to Use:
Open the .exe file under /Run/
Operations:
	press P to create player.
	press I to create item.
	press E to create enemy
    	W, A, S, D to move the player
	press ESC to quit (not X)
	Controller:
		Right trigger: shoot
		Left JoyStick: move
		Right Joystick: aim
	F1 to enter debug mode
		blue box: camera window
		red circle: camera position (smoothed)
		black circle: goal camera position (not smoothed)
Console command:
	delete_player:<index> delete player with index
Settings:
	The first player is always controlled by mouse and keyboard.
	If the controller is connect, the other player is controlled by controller. Otherwise, they are controlled by AI.
	Enemy and Player share same AI stratergy:
		Patrol: No player/Enemy close enough, randomly find movable goal pos and move.
		Shoot:	player/enemy is close enough, stable and shoot towards target with cooldown.

 