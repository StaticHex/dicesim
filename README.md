================================================================================
--------------------------------------------------------------------------------
<> Dice Roll Simulator v1.1                                                   <>
--------------------------------------------------------------------------------
================================================================================
Programmed By: Joseph Bourque
Artwork By: Joseph Bourque
================================================================================
<> CONTENTS                                                                   <>
================================================================================
1. Controls
2. Program Overview
3. Known Bugs
4. List of Assumptions
5. FAQ
6. Changelog

================================================================================
1. Controls
--------------------------------------------------------------------------------
LEFT MOUSE BUTTON - Move center of gravity
RIGHT MOUSE BUTTON - Rotate die
Z ------------------ Shift between (y-active) and (z-active)
W ------------------ Move center of gravity up (y-active) or forward (z-active)
A ------------------ Move center of gravity left
S ------------------ Move center of gravity down (y-active) or back (z-active)
D ------------------ Move center of gravity right
UP ----------------- Rotate die up
DOWN --------------- Rotate die down
LEFT --------------- Rotate die Left
RIGHT -------------- Rotate die right

--------------------------------------------------------------------------------
2. Program Overview
--------------------------------------------------------------------------------
* The center of gravity for the die is initially set in the center of the die
  - Entering values in the x-pos, y-pos, or z-pos boxes will move the center
    of gravity for the die; as will using the GUI controls outlined above
* The [start sim] and [reset sim] buttons function exactly the same as their
  menu counterparts and therefore, to review functionality for these buttons
  please refer to the menu overview below.

--------------------------------------------------------------------------------
<> Menu Overview                                                              <>
--------------------------------------------------------------------------------
* FILE
  - SAVE RESULTS - saves the contents of the "simulation results" window to a 
                   file of your choice.
  - RESET SETTINGS - Resets the position of the center of gravity, the number of
                     rolls to perform, and the significant digits to show to 
                     their original values
  - CLOSE PROGRAM - Exits the program, same functionality as the exit button for
                    the window
* SIMULATION
  - SIGNIFICANT DIGITS - Sets the number of digits to round the center of 
                         gravity to. 
  - START SIM - Runs the simulation and outputs th results in the simulation 
                results setting
  - RESET SIM - Prepares the program for the next simulation, choosing "Yes" 
                when asked if you want to clear settings will reset all settings
                and graphics to their default values
* GRAPHICS
  - Reset View - Resets the dice to the initial front view
* HELP
  - Displays this manual, nothing more.

--------------------------------------------------------------------------------
3. Known Bugs
--------------------------------------------------------------------------------
No bugs have been reported yet. To report a bug please contact the developer
at indegon1@gmail.com or leave a comment on the program's git repository at
##GIT ADDRESS HERE##

--------------------------------------------------------------------------------
4. List of Assumptions
--------------------------------------------------------------------------------
- Center of gravity assumed to always be on or inside the die; cannot move
  center of gravity outside the die

- No air resistance

- Dice model has sharp corners even though actual die have a 1% bevel on
  the edges

- Die assumed to be made out of Poly(methyl methacrylate) (PPMA) and the 
  restitution and coefficient of friction used are based on interactions between 
  PPMA and felt on wood

- Die assumed to only have ONE center of gravity vs. multiple

--------------------------------------------------------------------------------
5. FAQ
--------------------------------------------------------------------------------
This is a new program so no FAQ yet, to submit a question please contact the
developer at indegon1@gmail.com or leave a comment on the program's git 
repository at ##GIT ADDRESS HERE##

--------------------------------------------------------------------------------
6. Changelog
--------------------------------------------------------------------------------
++ Version 1.1 ++
--------------------------------------------------------------------------------
- Fixed a bug involving updating the output area which caused the program to 
  crash when running large numbers of simulations

- Implemented smoother update tracking, no longer have to move the mouse to
  get the GUI to update

- Tested working with up to 1,000,000 simulations

- Ran theoretical models to determine maximum number of simulations program
  can handle: 1,000,000,000 simulations which would take approximately 
  148 days to run to completion

- Implemented upper bound on num sim box to reflect theoretical bound
