#!python3
from math import pi, sin


A = int(2**16-1) >> 5
sample_frequency = 48000
sine_frequency = 1000


print("int16_t samples[] = {")
# print("int16_t samples[" + str(sample_frequency // sine_frequency) + "] = {")


for i in range(sample_frequency // sine_frequency):
    sample = A*sin(2*pi*i/(sample_frequency // sine_frequency))
    print("    " + str(int(sample)) + ",")

print("};")
