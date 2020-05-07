# fitz
CSSE2310 Project Semester 2 2018

A game where you can place tiles and play against two types of AI, or another player. 


# How to play

**`Usage: fitz tilefile [p1type p2type [height width | filename]]`**

To begin a game enter a tilefile which stores the pieces that will be chosen sequentially for play, the types
of players you would like to use, and the size of the board. The following are valid options:

## Player Options
* `h`: Human player
* `1`: AI Player one: Starts filling after last player position
* `2`: AI Player two: Starts filling from a corner

## Tilefile
The tilefile is a file which stores 5x5 pairs of characters, with a `,` representing an empty space and a `!`
representing a filled space. Use this to make different types of shapes for use. Each new tile is separated by a newline
to differentiate. The tilefile will loop back to the start of the file once it runs out of tiles. Example tilefile:

```,,,,,
,,,,,
,!!!,
,!!!,
,!!!,

,,!!!
,,,,!
,,,,!
,,,,,
,,,,,
```

By typing `fitz tilefile` all tiles in the supplied file will be printed along with all of their rotations.

## Board Size
The board is constrained between 0 and 999 for both axes. Pick any value within this range (inclusive). Example:
`20 20`

## Saved games
Fitz supports saving your game and reloading for later play. To reload a game, pass a save game file into the parameters
as `filename` in place of the boardsize. To save the game during gameplay, type the word `save` followed immediately by 
the filename to save to. That is, no spaces between the word save and the save file name (i.e. `saveFileName`).

## Gameplay input

While playing, you will be shown your tile to place and the state of the board, Input is determined through a triple of ints:
`xPos yPos angle`.

To place a tile, tell fitz where you want to place the center of the tile (the centre of the 5x5 grid) and what rotation angle
you would like. If a non empty part of the tile would go off the board, or onto an occupied tile, it is invalid and you 
will be reprompted for a valid move. The game ends once there are no more valid moves for the given tile. 





