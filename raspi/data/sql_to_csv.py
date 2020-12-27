import mysql.connector
import numpy as np
import time
import datetime
import pandas as pd

############################
# get data out of database #
############################

database = mysql.connector.connect(
    host="localhost",
    user="",
    passwd="",
    database=""
)

cursor = database.cursor()
query = "SELECT * FROM dual_zone_temp_hum"
cursor.execute(query)
data = cursor.fetchall()
# data is a now a list of tuples
# each list item is a row from database
# list item comprised of tuples for each col from database
cursor.close()
database.close()


#############################
# put data into numpy array #
#############################

# U19 is 19 char string
# for specific formatting from database, timestamp takes form (item 11 (index [10])
# '2019-11-28 15:35:03.632407'
# i.e. 'yyyy-mm-dd hh:mm:ss.mmmmmm'
# we ust U19 to trim this into:
# '2019-11-28 15:35:03'
# removing the floating milliseconds

#   z1       z2
# temp hum temp hum

dt = np.dtype('int,U19,float,float,float,float')
data_np = np.array(data, dtype=dt)
# np.savetxt("dual_zone_t_h_testing.csv", data_np, delimiter=",")

data_pd = pd.DataFrame(data_np)

now = datetime.datetime.now()
timestamp = str(now.strftime("%Y-%m-%d"))
fname = timestamp + "_dual_zone_t_h_testing.csv"

data_pd.to_csv(fname, header=["idx","timestamp","z1_tempF","z1_hum","z2_tempF","z2_hum"])
