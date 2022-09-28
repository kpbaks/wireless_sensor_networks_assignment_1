set terminal jpeg 
set output "output.jpg"

plot "./test_signal.dat" with lines
plot "./compressed_signal.dat" with lines
plot "./decompressed_signal.dat" with lines
