# Final Project Proposal

## Group Members:
Leo Titievsky
       
# Intentions:
Design a 3D, tournament/arena style FPS game with support for local multiplayer and bots.

# Intended usage:

The player will navigate into tournaments/servers through a menu with keyboard inputs, and then play the game with keyboard inputs.      
The player can choose to duel in a 1v1, take part in a 4 person tournament, or fight endless waves of ghosts.      
Command line prompts will be used for some things, like setting your name in a tournament.     
  
# Technical Details:

A description of your technical design. This should include: 
   
How you will be using the topics covered in class in the project.
     
How you are breaking down the project and who is responsible for which parts.
The client side will use *memory allocation* and *signals* to handle IO, quitting, and graphics. It will also *network through sockets to a server*. The server will exec and fork subservers.    
The clients send position data to the server which uses a *non-blocking select* to check for positional and state updates each tick.      The server then redistributes the position data to players so they can draw them on screen     the server can also exit and be reaped by its parent matchmaking/tournament server to start the next round of a tournament      The tournament server works by mapping each client to a descriptor and having its children exit with the index of the descriptor who won, allowing a tournament to be made by making contestants wait for the server to give the OK and then forking a new subserver and passing clients from the lobby into the game.

# Intended pacing:
FPS Core already completed
1/8/26: seperate and revamp core FPS code, preparing game logic for feeding from a server process and graphics for using SDL or a frame buffer lib
1/10/26: Server setup
1/12/26: Bot revamp, based in server. Replace 'ghosts' with A* guided bots. (maybe) use bot-subprocesses to have complex behaviours.
1/14/26: Replace bot slots with other player position feeds, design multiplayer lobby/matchmaking/menu
1/16/26: Playtest and debug.
