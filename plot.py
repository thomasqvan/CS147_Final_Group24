import matplotlib.pyplot as plt

if __name__ == '__main__':

    # Data extracted from Amazon AWS
    bpms = [78.626, 78.567, 77.490, 78.371, 76.522, 76.491, 74.430, 75.438, 75.096, 75.237,
            77.255, 77.437, 77.581, 78.217, 77.368, 77.543, 77.402, 78.119, 77.603, 76.732]

    plt.plot(bpms)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Heartbeat (BPM)")
    plt.title("HeartBeat Visualization")
    plt.show()
    
