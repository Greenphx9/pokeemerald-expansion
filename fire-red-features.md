This document will describe how to port over Fire Red features to Emerald/list commits where I ported.

## NPC Text Color
NPCs have text colors based off their gender in Fire Red / Leaf Green.

https://github.com/Greenphx9/pokeemerald-expansion/commit/050cca4093c88be8e97d893047fd0c59f83e4662

https://github.com/Greenphx9/pokeemerald-expansion/commit/15ed161531a2f08bb9db231ac54961651986be8c

## Battle BGs
The same format for battle BGs are used in all Gen 3 games, meaning you can simply copy + paste the tiles, tilemap, and palettes.

## Tilesets
Porting tilesets can be annoying, but luckily SBird & Ghoulslash have scripts that simplify this. 

Firstly, ensure you are using the triple layer metatile system [found here](https://github.com/pret/pokeemerald/wiki/Triple-layer-metatiles).

When you get to running the script, use the modified script from [pokefirered's wiki](https://gist.github.com/Snaid1/634847353fd320ea24aa71271afb9cde).

This should convert all of the **metatiles.bin** files, and you can copy them to your pokeemerald tileset folder. We aren't done yet however, as the metatile attributes will still be broken.

Next, run ghoulslash's [convert_metatile_attributes](https://github.com/ghoulslash/tools/blob/master/decomps/convert_metatile_attributes.py) script on each **metatile_attributes.bin** file, and then copy it over. 

Now your tilesets should be fully working with Emerald. Of course, metatile behaviours will be broken unless you align yours with Emerald, but its still a good start
