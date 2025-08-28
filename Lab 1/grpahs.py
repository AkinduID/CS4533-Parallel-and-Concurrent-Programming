import matplotlib.pyplot as plt
import numpy as np

# Data
cases = {
    "Case 1": {
        "Serial": [7053, 949],
        "One mutex": [8135, 702, 13112, 1210, 14710, 896, 14597, 490],
        "RWLock": [8848, 656, 7598, 421, 6286, 847, 7223, 727]
    },
    "Case 2": {
        "Serial": [10648, 878],
        "One mutex": [11257, 969, 19073, 1716, 20240, 1017, 21313, 1191],
        "RWLock": [13350, 3383, 17502, 707, 19358, 1188, 19884, 1026]
    },
    "Case 3": {
        "Serial": [44699, 1511],
        "One mutex": [45773, 1797, 61931, 3610, 63750, 1778, 65038, 1551],
        "RWLock": [46052, 1562, 70865, 2821, 73178, 1234, 72717, 1319]
    }
}

# Function to plot and save each case
def plot_case(case_name, data):
    threads = ['1', '2', '4', '8']
    plt.figure(figsize=(10,6))
    
    # Serial (single-thread bar)
    plt.bar('1 Thread', data['Serial'][0], yerr=data['Serial'][1], capsize=5, label='Serial', color='lightblue')
    
    # One mutex and RWLock
    one_mutex_avgs = data['One mutex'][::2]
    one_mutex_std = data['One mutex'][1::2]
    
    rwlock_avgs = data['RWLock'][::2]
    rwlock_std = data['RWLock'][1::2]
    
    x = np.arange(len(threads))
    width = 0.35
    
    plt.bar(x - width/2, one_mutex_avgs, width, yerr=one_mutex_std, capsize=5, label='One mutex', color='orange')
    plt.bar(x + width/2, rwlock_avgs, width, yerr=rwlock_std, capsize=5, label='Read-Write lock', color='green')
    
    plt.xticks(x, threads)
    plt.xlabel("Number of Threads")
    plt.ylabel("Time (microseconds)")
    plt.title(f"{case_name} Performance")
    plt.legend()
    plt.tight_layout()
    
    # Save figure as PNG
    plt.savefig(f"{case_name.replace(' ', '_')}_performance.png")
    plt.close()  # Close the figure to avoid displaying it

# Plot and save all cases
for case_name, data in cases.items():
    plot_case(case_name, data)

print("All graphs saved successfully!")
