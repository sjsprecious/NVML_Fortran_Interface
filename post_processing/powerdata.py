import pandas as pd
import glob

# Get a list of all txt files in the current directory
files = glob.glob('*txt')

for f in files:
    # Read the text file into a Pandas DataFrame
    df = pd.read_csv(f, delimiter=',', header=None, names=['timestamp','Power(Watts)'])

    # Get the number of rows and columns
    num_rows, num_columns = df.shape

    # Calculate the total energy consumption (unit: J):
    #   - subtract the time stamps from two rows to get actual time interval (unit: second)
    #   - take the average of two rows to get the average GPU power usage (unit: Watts)
    energy = 0.0
    for r in range(num_rows-1):
        interval = (df['timestamp'][r+1] - df['timestamp'][r]) / 1000.0
        power = (df['Power(Watts)'][r] + df['Power(Watts)'][r+1]) / 2.0
        energy = energy + power * interval

    # Print out results:
    fstring = "File: {}, energy consumption = {:.6f} J or {:.6f} KWH".format(f,energy,energy/3.6e6)
    print(fstring)
