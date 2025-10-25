import numpy as np
import matplotlib
# Try to use TkAgg backend for WSLg
try:
    matplotlib.use('TkAgg')
    interactive = True
except:
    matplotlib.use('Agg')
    interactive = False
    
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

if interactive:
    plt.show()
    print("Interactive display opened")
else:
    plt.savefig('heat_distribution.png', dpi=300, bbox_inches='tight')
    print("✓ Saved to heat_distribution.png (GUI not available)")