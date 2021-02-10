Known Issues: 

How to Use:
Open the .exe file under /Run.
The operation is same as the demo
 

Operations:

keyboard:
    Press Esc to exit.
    Press P to pause/start the game.
    Press 1 to only draw polygon
    Press 2 to no space partition.
    Press 3 with space partition.
    Press F8 to restart of the game.
    Press left mouse button to start the visible raycast, the raycast end position is the mouse position.
    Press right mouse button to end the visible raycast
Deep Learning:
	1. 1024 raycast vs 128 object: framerate 10
	   1024 raycast vs 512 object: framerate 7
	   1024 raycast vs 1024 object: framerate 3
	2. When enable partitioning scheme, the improvement is huge, which makes me feels there is some bug with it and I ma still working on debuging. 
	3. Debug mode takes about half way slower that release mode. For 1024 raycast vs 32 objects, the framerates is 5. In debug mode, only drawing 1024 polygon, 
	   the frame rate is 10. For release mode, drawing 1024 polygon the frame rate is 20.
	4. Rendering improvement. 
		First I use old way draw vertices with index. For drawing 1024 polygon, the framerate is about one.
		Then I tried with drawing with index, but with old function, which construct the non index vertices first and use that to construct the indexes vertices and indexes.
		Because it has a tempory middle nonindexed vertices, the speed did not improve much.
		Finally with correctly construct indexed vertices, the speed improves a lot. It goes from O(N cube) to O(N square).		