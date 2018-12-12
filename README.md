# OnionFS
OnionFS is a fork of layeredfs plugin made by cell9 that redirects romfs and save access to the SD card, useful for game mods and multiple save slots. It is designed to work with any game, even if there is an update patch installed (however there may be untested games that are incompatible for whatever reason, if you find one, please create an issue with the problem).

Default redirection paths:
- Extracted romfs: **"SD:/OnionFS/\<lowtitleID\>/romfs/"**
- Save file: **"SD:/OnionFS/\<lowtitleID\>/save/"**

**\<lowtitleID\>:** The last 8 characters of the game title ID.

## ROMFS redirection
If ROMFS redirection is enabled, the game will first try to open ROMFS files from the SD card, and if they don't exist then the original ones will be used. This feature behaves the exact same way as layeredFS

## SAVE redirection
If SAVE redirection is enabled, all the save files will be stored directly in the SD as if they were extracted from Checkpoint or JKSV. Note that the plugin won't extract the existing save files, it will create new ones. If you want to use your existing save extract it first with Checkpoint.
In order to keep the original save file untouched and working, the secure value of games that use it will be removed each time the game starts.

## Entry manager
You can create multiple SAVE and ROMFS entries for the same game in the SD card. This allows to load different romhacks or have multiple save slots. To use this feature press the (Select) button to open the CTRPF ingame menu and access the OnionFS entry, from there you will be able to add more entries, configure existing ones and change which one will be used.

**NOTE:** Modifying the current loaded entry or changing to another one will force the console to reboot to prevent any save corruption.

Custom entry redirection paths:
- Extracted romfs: **"SD:/OnionFS/\<entryname\>/romfs/"**
- Save file: **"SD:/OnionFS/\<entryname\>/save/"**

Notice that if a new entry is created, the redirected path will no longer depend on the game titleID. This is done so **it is possible to share the save between compatible games if an entry with the same name is created for both games.**

## Debug mode
You can use debug mode to output the plugin log to a file: **"SD:/OnionFS/\<titleID\>debug.txt"**. This information will be useful to log all the accessed files by the game as well as the initial setup operations.
To enable debug mode, hold **DPAD UP** while the game is booting.

**NOTE:** The game will run very slow and may lag in debug mode as a lot of file writes will be happening very quickly.

## Youtube video

[![OnionFS 2.0 - ROMFS & SAVE redirection to SD card.](https://img.youtube.com/vi/T7G_sFwYdsY/0.jpg)](https://www.youtube.com/watch?v=T7G_sFwYdsY)

## Credits
- cell9: Developer of layeredfs and NTRCFW.
- Nanquitas: CTRPF developer and help. (A LOT of help actually :P) 
~~"私はlolisが好きです"~~
