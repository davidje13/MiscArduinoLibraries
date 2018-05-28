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
