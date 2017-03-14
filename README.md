# OnionFS
OnionFS is a fork of layeredfs plugin made by cell9, that redirects romfs access to the SD card, useful for game mods. OnionFS is made to work with any game that was previously working with layeredfs, even if there is an update patch installed. Since it detects the correct addresses from the game code, it will work for any game.
The plugin will redirect the game romfs to **"SD:/OnionFS/\<titleID\>/romfs"** and the code to **"SD:/OnionFS/\<titleID\>/code.bin"** by default.

##Custom path
To change the redirection path, use the included python script:

```
OnionFS_pathchange.py \<plg file path\> \<redirection path\>
```
For example: SD:/folder/subfolder/
```
OnionFS_pathchange.py OnionFS.plg folder/subfolder
```
(In the avobe example, romfs folder will be SD:/folder/subfolder/romfs and code file will be SD:/folder/subfolder/code.bin)

##Credits
cell9: Cretor of layeredfs and NTRCFW.

Nanquitas: Helping me A LOT with my noob questions. xP
