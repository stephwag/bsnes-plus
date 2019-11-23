## BSX RSOD/Time sync error

To trigger the error found in this [video](https://www.youtube.com/watch?v=tzfswtN0LBk)

- Check out the branch `bsx-rsod` and recompile.
- Using [SatellaWave](https://github.com/luigiblood/sat_wave), generate Satellaview Broadcast binary files that point to a BS game.
- Make sure bsnes-plus settings are pointing to the broadcast files.
- Go to Load Special -> Load BSX Cartridge and enter the BSX and BS ROMs.
- Attempt to "download" the game through BSX.

If you see a screen with the clock set to `??:??`, then the error will eventually appear.

The branch changes line 235 in `bsx_base.cpp` from

```
stream.data = get_time(stream);
```

to

```
stream.data = 0;
```
