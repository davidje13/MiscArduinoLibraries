# Decompressor

This is a decompression algorithm based on DEFLATE but optimised for
low-RAM requirements when decompressing. It works with the Compressor
utility.

Specifically, this compresses repeated phrases in the input data, and
applies huffman encoding on the result.

The window size is configurable; larger values may allow better
compression but require more RAM at runtime. Even window sizes of 1
can often be sufficient to get good compression levels on moderate
amounts of data.

## Program size savings

As a very vague guide, using this code may add ~2KB to the executable
size, so if your resources end up compressed by at least that much, it
may be worth using this utility.

## Security

This tool is designed for reading trusted data. See the TRUSTED_INPUT
definition for details on how to use it with untrusted input. Note that
the intended use-case is for compressing static resources, not user
data.

## RAM usage

The basic decompressor uses 38 bytes plus the chosen window size, plus
a few more bytes while its various functions are in operation.

Using DecompressorPointer will add about 4 extra bytes per pointer.

## Speed

Note that reading compressed data is much slower than reading
uncompressed data! This library is quite fast, but it is tuned to
minimal RAM usage, not maximum speed.

It is important to ensure you only read data from within the most
recent [window-size] bytes of the highest-index byte read so far.
If you try to read a byte from earlier in the stream, it will need to
reset and calculate all bytes from scratch.

In other words, this is good for any window size:

```
for(int y = 0; y < 10; y++) {
  for(int x = 0; x < 10; x++) {
    uint8_t value = decompressor.get(y * 10 + x);
    // ...
  }
}
```

but this is bad unless the window size is at least 100:

```
for(int x = 0; x < 10; x++) {
  for(int y = 0; y < 10; y++) {
    uint8_t value = decompressor.get(y * 10 + x);
    // ...
  }
}
```
