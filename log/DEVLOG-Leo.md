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
#1/12/2026#      
Troubleshot SDL in lab with Mr. K. Upped ray density and tweaked rendering to make the world more visibile.
#1/13/2026#         
Got sprite and enemy drawing down, created a gun and enemy sprite. All collision features from the terminal version added back. enemies collide.    
#1/14/2026#    
Added death and destruction. You can now kill other clients, crashing their game. Patched a bug where players build tails of industructable walls behind them, like tron.      
#1/15/2026
Reworked death for more server involvment instead of just bricking everything when someone wins. Basic UI updates, with a hit/hurt flash, TTF text hp/id/gamemode indicators. The matchmaking parent server     
"Server Launcher" Works now, and can receive up to 4 clients an hold either a 1v1 or 4 player tournament match, passing clients along as needed. New game level added, and the 'ghosts' mode from the original terminal      game was re-added.