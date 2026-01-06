# Final Project Proposal

## Group Members:
Leo Titievsky
       
# Intentions:
Design a 3D, tournament/arena style FPS game with support for local multiplayer and bots.

# Intended usage:

The player will navigate into tournaments/servers through a menu with keyboard inputs, and then play the game with keyboard inputs.
  
# Technical Details:

A description of your technical design. This should include: 
   
How you will be using the topics covered in class in the project.
     
How you are breaking down the project and who is responsible for which parts.
The client side will use *memory allocation* and *signals* to handle IO, quitting, and graphics. It will also *network through sockets to a server*. The server will exec and fork subservers, and generate bot processes, linked through *Interprocess communication* 
    
# Intended pacing:
FPS Core already completed
1/8/26: seperate and revamp core FPS code, preparing game logic for feeding from a server process and graphics for using SDL or a frame buffer lib
1/10/26: Server setup
1/12/26: Bot revamp, based in server. Replace 'ghosts' with A* guided bots. (maybe) use bot-subprocesses to have complex behaviours.
1/14/26: Replace bot slots with other player position feeds, design multiplayer lobby/matchmaking/menu
1/16/26: Playtest and debug.
