# Abducted

## Created for the GB Compo 2023
This project was created for the [GB Compo 2023](https://itch.io/jam/gbcompo23). It was fun!

There's a few bits to this. This was made using the incredible [GB Studio 3.1](https://www.gbstudio.dev/). The first person engine is a rewrite of the [Point and Click scene](plugins/crawler/engine/src/states/pointnclick.c), and most of the events that are not stock GB Studio are native calls to the [hero.c](plugins/crawler/engine/src/hero.c) file.

The implementation is a bit clumsy. I didn't really understand how I could look up actors or variables set up in GB Studio from c. My solution was to "extern" a bunch of variables in c and then use [GBVM](https://www.gbstudio.dev/docs/scripting/gbvm/) commands like `VM_GET_UINT8`, etc., to grab them from the engine. Late into the jam I was able to use GBVM a bit better, and I better understood the engine behind GB Studio. There's a lot of things I want to tidy up (beyond the bugs in the game itself) but it works pretty well.

The first person engine is a simple grid with walls, doors, and open spaces. First, the viewing area is solved by rotating 3x5 spaces from the map based on the player's position and rotation. Then, using a "painter's agorithm", the view is drawn from back to front. Each layer is series of copies from the "pieces" on the right hand side of the background to a buffer area below the screen space. Once the drawing is done, the whole scene is copied from the buffer area to the viewport.

I'll eventually include some better instructions on getting it up and running yourself, but in the meantime feel free to open up the GB Studio project and see how it all works!
