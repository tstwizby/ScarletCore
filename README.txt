This is the source code for the ScarletCore engine, currently in development alongside the game 'Rules and Instructions' (https://scarlettwizby.itch.io/rules-and-instructions)

This engine is being developed by a single person with a day job, so progress is likely to be erratic and unpredictable. Yes, I know, my coding style is cursed.


The purpose of this engine is to support building adventure games/visual novels, in a way that is portable and efficient, using SDL. The full scope of the project is as of yet undefined, and is tied to the development of the game 'Rules and Instructions', though it is intended to be general-purpose and not tied tightly to that game alone.

Notable features distinguishing it from similar engines are built-in systems for tracking and modifying environmental details such as time of day, weather, or distance between locations, allowing for example dynamically constructed backgrounds taking such details into account.

It includes a compiled scripting language and tools for making custom fonts.



At the moment this readme is a little bare-bones, as I am prioritizing development of the game over the engine (as a product) at this time, but I intend to leave the engine as fully open-source and to create tools for working with it effectively over time.



List of code files and their purposes:

main.c			The main file of the project, includes other files.
error.c			Includes macros for configurable error handling/debug message logging.
buffer.c		Defines a 'buffer' struct used in many places in the engine.
paths.c			Defines the format for file paths and provides a number of helper macros for file IO.
game.c			Defines, ennumerates, and implements game data types.
script.c		Contains information directly relevant to writing/reading script files.
SDLWrapper.c	Wrapper for SDL libraries to separate rendering logic from game logic.
sound.c			Wrapper for SDL_mixer to separate audio logic from game logic.
graphics.c		Defines structures used to render graphical objects.
textbox.c		Defines a 'textbox' struct used by the engine.
text.c			Contains information about the static text boxes/script buffers used by the game.
compiler.c		A simple compiler for script files.
fontbuilder.c	An example for building font data files.