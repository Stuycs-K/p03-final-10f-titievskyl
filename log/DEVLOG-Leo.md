#1/6/2026 start#     
Added my utilities and reworked my animation demo from the original version to prepare for a cleaner game rework on top of my engine          
#1/7/2026 start#     
Began adding SDL functionality, achieving a succesful basic version of the column drawing behind the term gfx tools I wrote          
#1/8/2026 start#     
Converted SDL code to SDL2, and began adapting forking server to new use case. Decided on using select and figured out client->server messaging format : ID|HP|STATE|X|Y          
#1/9/2026#       
New server architecture, focused on select        
#1/10-11 weekend/2026#      
Basic SDL port finished, combatless MVP (two players walk around a blank server) done.      
Finishing requires adding back shooting, storing a local pillar copy of the recv'd player data, adding a basic ui, making map, basic A* bot system (client process running on server)            
