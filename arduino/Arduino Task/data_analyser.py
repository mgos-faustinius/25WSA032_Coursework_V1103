import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('temp_data.csv')
print(df.head())

# PLOT 1: plotting temp vs time
plt.figure()
plt.plot(df['Time'], df['Temperature'])
plt.xlabel('Time (ms)')
plt.ylabel('Temperature (°C)')
plt.title('Temperature vs Time')
plt.savefig('temp_vs_time.png') # save the plot as an image file
plt.show()

# PLOT 2: plotting magnitude vs frequency
plt.figure()
plt.plot(df['Frequency'], df['Magnitude'])
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.title('Magnitude vs Frequency')
plt.savefig('magnitude_vs_frequency.png') 
plt.show()

#Plot 3: smoothed temp and temp vs time 
df['Smoothed'] = df['Temperature'].rolling(window=5).mean() # using rolling average to smoothen temp data

plt.figure()
plt.plot(df['Time'], df['Temperature'], label='Original')
plt.plot(df['Time'], df['Smoothed'], label='Smoothed')
plt.legend()
plt.xlabel('Time (ms)')
plt.ylabel('Temperature (°C)')
plt.title('Smoothed Temperature vs Time')
plt.savefig('smoothed_temp_vs_time.png') 
plt.show()

#plot 4: histogram of temperature values
plt.figure()
plt.hist(df['Temperature'], bins=20) # number of bars (bins) set to 20
plt.xlabel('Temperature (°C)')
plt.ylabel('Frequency')
plt.title('Histogram of Temperature Values')
plt.savefig('temp_histogram.png') 
plt.show()

#plot 5: temperature change rate vs time
df['Temp_Change_Rate'] = df['Temperature'].diff() / df['Time'].diff() 
plt.figure()
plt.plot(df['Time'], df['Temp_Change_Rate'])
plt.xlabel('Time (ms)')
plt.ylabel('Temperature Change Rate (°C/ms)')
plt.title('Temperature Change Rate vs Time')
plt.savefig('temp_change_rate_vs_time.png') 
plt.show()