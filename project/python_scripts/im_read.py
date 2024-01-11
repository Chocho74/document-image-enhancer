import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

filename = "output.csv"

df = pd.read_csv(filename)
array = df.values

plt.figure()
plt.imshow(array)
plt.show()

