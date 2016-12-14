# OnionFS
OnionFS is a fork of layeredfs plugin made by cell9, that redirects romfs access to the SD card, useful for game mods. OnionFS is made to work with any game that was previously working with layeredfs, even if there is an update patch installed. Since it detects the correct addresses from the game code, it will work for any game.
The plugin will redirect the game romfs to **"SD:/OnionFS/\<titleID\>"** by default.

##Custom path
Add your custom path by changing the OnionFS.plg at address **0x3058**.
For example: "SD:/myfolder/hello" must be written at **0x3058** as **"ram:/myfolder/hello/"**. Clear any "yourpath" string you see.
(*A python script is being made to make this process easier.*)

##Credits

cell9: Cretor of layeredfs and NTRCFW.

Nanquitas: Helping me A LOT with my noob questions. xP
