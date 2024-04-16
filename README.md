# Content-Aware Resizing

Experimenting with seam-carving.

## Current limitations

- Only support vertical carves.
- Only support PNG files.
- Slow.

## Quick start

```console
$ git clone https://github.com/DeShrike/seam-carving.git
$ cd seam-carving
$ make
$ ./sc images/horses.png
```

## References

### Seam Carving

See https://en.wikipedia.org/wiki/Seam_carving
and https://dl.acm.org/doi/10.1145/1275808.1276390

### Sobel operator

See https://en.wikipedia.org/wiki/Sobel_operator

## TODO

- Horizontal carving
- JPEG support
- Make it faster
- More parameters

