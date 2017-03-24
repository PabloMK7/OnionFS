# OnionFS
OnionFS is a fork of layeredfs plugin made by cell9 that redirects romfs and code access to the SD card, useful for game mods. It works with **any game**, even if there is an update patch installed. For romfs redirection, you only need to put in your SD card the modified files, **not the whole romfs directory nor a romfs.bin/game.romfs file**.

Default redirection paths:
- Extracted romfs (only modified files): **"SD:/OnionFS/\<titleID\>/romfs/"** 
- code.bin: **"SD:/OnionFS/\<titleID\>/code.bin"**
- code.ips: **"SD:/OnionFS/\<titleID\>/code.ips"**

## Custom path
To change the redirection path, use the included python script:

```
OnionFS_pathchange.py <plg file path> <redirection path>
```
For example: SD:/folder/subfolder/
```
OnionFS_pathchange.py OnionFS.plg folder/subfolder
```
(In the above example, romfs folder will be SD:/folder/subfolder/romfs and code file will be SD:/folder/subfolder/code.(bin/ips))

## Credits
- cell9: Cretor of layeredfs and NTRCFW.
- Nanquitas: Helping me A LOT. xP
- AuroraWright: ips patching code. 
