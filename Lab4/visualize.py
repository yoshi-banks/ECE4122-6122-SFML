#!/usr/bin/env python3

# Visualize the 2D steady state heat distribution from finalTemperatures.csv

import numpy as np
import matplotlib
# Try to use TkAgg backend for WSLg
matplotlib.use('TkAgg')
interactive = True

    
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt('finalTemperatures.csv', delimiter=',')

# Plot
plt.figure(figsize=(10, 8))
plt.imshow(data, cmap='hot', interpolation='nearest', origin='upper')
plt.colorbar(label='Temperature (°C)')
plt.title('2D Steady State Heat Distribution')
plt.xlabel('X Position')
plt.ylabel('Y Position')

plt.show()
print("Interactive display opened")
