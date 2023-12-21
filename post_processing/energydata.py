import pandas as pd

files    = ['gpu_usage_rank0_gpu0.txt', 'gpu_usage_rank1_gpu1.txt',
            'gpu_usage_rank2_gpu2.txt', 'gpu_usage_rank3_gpu3.txt']

for f in files:
    # Read the text file into a Pandas DataFrame
    df = pd.read_csv(f, delimiter=',', header=None, names=['timestamp','Energy(J)'])

    # Calculate the total energy consumption (unit: J):
    energy = df['Energy(J)'][1] - df['Energy(J)'][0]

    # Print out results:
    fstring = "File: {}, energy consumption = {:.6f} J or {:.6f} KWH".format(f,energy,energy/3.6e6)
    print(fstring)
