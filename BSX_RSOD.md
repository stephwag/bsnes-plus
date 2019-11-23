## BSX RSOD/Time sync error

To trigger the error found in this [video](https://www.youtube.com/watch?v=tzfswtN0LBk), check out the branch `bsx-rsod` and recompile.

The branch changes line 235 in `bsx_base.cpp` from

```
stream.data = get_time(stream);
```

to

```
stream.data = 0;
```
