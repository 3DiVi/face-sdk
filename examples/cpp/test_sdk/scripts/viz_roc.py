import os
import sys
import pandas as pd
import matplotlib.pyplot as plt

def viz_roc(csv_files, plot_result_file=None):

	plt.title("ROC")
	plt.grid(True)

	plt.xscale("log")
	plt.xlim(1e-7, 1)
	plt.xlabel("false positive rate")

	plt.ylabel("true positive rate")

	for csv_file in csv_files:
		roc_data = pd.read_csv(csv_file, sep=' ', names=['far', 'tar', 'dist'])
		plt.plot(roc_data.far.values, roc_data.tar.values, label=csv_file)

	plt.legend(loc='lower right')

	if plot_result_file == None:
		plt.show()
	else:
		plt.savefig(plot_result_file)

if __name__ == '__main__':
	if len(sys.argv) == 1:
		print('Run example:\n\tpython3 viz_roc.py [-s res_file.jpg] roc_file1.txt [roc_file2.txt [...]]')
		print('\nOptions:\n\t-s result_file.jpg - save image in result_file.jpg without visualization')
	elif sys.argv[1] == '-s':
		assert(len(sys.argv) >= 4)
		viz_roc(sys.argv[3:], sys.argv[2])
	else:
		viz_roc(sys.argv[1:])
