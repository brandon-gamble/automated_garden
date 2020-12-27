import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

data_pd = pd.read_csv("2020-11-30_dual_zone_t_h_testing.csv", delimiter=",", index_col = "timestamp")


# data_pd.plot()
# data_pd["z1_tempF"].plot()

########################################################
# Temperature
########################################################
fig, ax = plt.subplots()

data_pd["z1_tempF"].plot()
data_pd["z2_tempF"].plot()

ax.set(xlabel='time', ylabel='Temp (F)',
       title='Temperature Experiment')
ax.grid()

plt.yticks(np.arange(70,86,1))
plt.xticks(rotation = 30)
plt.ylim([70, 85])

plt.show()


########################################################
# Humidity
########################################################
fig, ax = plt.subplots()

data_pd["z1_hum"].plot()
data_pd["z2_hum"].plot()

ax.set(xlabel='time', ylabel='Humidity (%)',
       title='Humidity Experiment')
ax.grid()

plt.yticks(np.arange(40,105,5))
plt.xticks(rotation = 30)
plt.ylim([45, 100])

plt.show()
