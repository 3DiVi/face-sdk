from sklearn.metrics import auc
import matplotlib.pyplot as plt
import numpy as np
import sys

def find_nearest(array, value):
    array = np.asarray(array)
    return (np.abs(array - value)).argmin()


def create_figure(total_subplots, subplot_instance):
    if subplot_instance is None:
        if total_subplots == (1, 1) or total_subplots == 1:
            f, axes = plt.subplots(figsize=(12, 12))
            subplot_instance = axes
        else:
            f, axes = plt.subplots(total_subplots[0], total_subplots[1],
                                   figsize=(8 * total_subplots[1], 7 * total_subplots[0]))
            if axes.ndim == 1:
                subplot_instance = axes[0]
            else:
                subplot_instance = axes[0, 0]
    else:
        axes = subplot_instance
    return axes, subplot_instance



def get_tpr_by_fpr(tpr_arr, fpr_arr, fpr_value):
    for i in range(0, len(fpr_arr) - 1):
        if fpr_arr[i] <= fpr_value <= fpr_arr[i+1]:
            x = fpr_arr
            y = tpr_arr
            return y[i] + (fpr_value - x[i])*(y[i+1] - y[i])/(x[i+1] - x[i])
    raise "Not found!!!"


def plot_roc_curve(fpr, tpr, thresholds=None, title_additional='', label_additional='', save_plot_path=None, color='b',
                   subplot_instance=None, total_subplots=(1, 1), close_fig=False, legend_size=None, linestyle=None):
    roc_auc = auc(fpr, tpr)
    fnr = 1 - tpr
    eer = tpr[np.nanargmin(np.absolute((fnr - fpr)))]
    if legend_size is None:
        if thresholds is None:
            legend_size = 5
        else:
            legend_size = 3
    axes, subplot_instance = create_figure(total_subplots, subplot_instance)
    x_labels_str = ['1e-08', '1e-07', '1e-06', '1e-05', '0.0001', '0.001', '0.01', '0.1', '1']
    x_labels = [1e-08, 1e-07, 1e-06, 1e-05, 0.0001, 0.001, 0.01, 0.1, 1]

    fpr_index = [find_nearest(fpr, val) for val in [1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2]]
    tpr_values = f" TPR:({tpr[fpr_index[0]]:.4}(1e-8), {tpr[fpr_index[1]]:.4}(1e-7), {tpr[fpr_index[2]]:.4}(1e-6), {tpr[fpr_index[3]]:.4}(1e-5)," \
                 f" {tpr[fpr_index[4]]:.4}(1e-4), {tpr[fpr_index[5]]:.4}(1e-3), {tpr[fpr_index[6]]:.4}(1e-2))"
    
    _labels = [1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2]
    _labels = [1e-8, 1e-7, 2e-7, 3e-7, 4e-7, 5e-7, 6e-7, 7e-7, 8e-7, 9e-7, 1e-6, 2e-6, 3e-6, 4e-6, 5e-6, 6e-6, 7e-6, 8e-6, 9e-6, 1e-5, 2e-5, 3e-5, 4e-5, 5e-5, 6e-5, 7e-5, 8e-5, 9e-5, 1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 1e-3, 2e-3, 3e-3, 4e-3, 5e-3, 6e-3, 7e-3, 8e-3, 9e-3, 1e-2]
    _tpr_values = ""
    for l in _labels[::-1]:
        _tpr_values += f"{l:.0e} = {get_tpr_by_fpr(tpr, fpr, l):.3}, "
    print(label_additional, _tpr_values)
    
    metrics = tpr_values
    if thresholds is not None:
        threshold_values = f" threshold:({thresholds[fpr_index[0]]:.4}(1e-8), {thresholds[fpr_index[0]]:.4}(1e-7)," \
                           f"{thresholds[fpr_index[1]]:.4}(1e-6), " \
                           f"{thresholds[fpr_index[2]]:.4}(1e-5), {thresholds[fpr_index[3]]:.4}(1e-4), " \
                           f"{thresholds[fpr_index[4]]:.4}(1e-3), {thresholds[fpr_index[5]]:.4}(1e-2))"
        metrics += threshold_values
    subplot_instance.plot(fpr, tpr, color=color, lw=1, linestyle=linestyle,
                          label=f'{label_additional}')
    subplot_instance.set_xlim([1e-8, 1])
    subplot_instance.set_ylim([0.9, 1])
    subplot_instance.grid(linestyle='--', linewidth=1)
    subplot_instance.set_xscale('log')
    subplot_instance.set_xlabel('False Accept Rate', fontsize=18)
    subplot_instance.set_ylabel('True Accept Rate', fontsize=18)
    subplot_instance.set_title('ROC Curve Test', fontsize=20)
    subplot_instance.legend(loc="lower right", fontsize=legend_size+2)
    subplot_instance.set_xticks(x_labels)
    subplot_instance.set_xticklabels(x_labels_str)
    subplot_instance.set_yticks(np.arange(0.750, 1.025, 0.025))
    subplot_instance.get_figure().tight_layout()
    if save_plot_path:
        subplot_instance.get_figure().savefig(save_plot_path)
        print(f"The plot is saved in {save_plot_path}")
    if close_fig:
        plt.close(subplot_instance.get_figure())
    return fpr, tpr, roc_auc, eer, axes




plot_colors = ['xkcd:purple', 'xkcd:green', 'xkcd:blue', 'xkcd:pink', 'xkcd:brown', 'xkcd:red', 'xkcd:light blue',
               'xkcd:dark blue', 'xkcd:turquoise', 'xkcd:grey', 'xkcd:dark green', 'xkcd:yellow', 'xkcd:orange', 'xkcd:teal',
               'xkcd:maroon', 'xkcd:black']

def read_file(fp):
    with open(fp) as f:
        ROC_data = f.readlines()

    ROC_data_splitted = [t.split() for t in ROC_data]
    ROC_data_narray = np.array(ROC_data_splitted, dtype=float)
    return ROC_data_narray


def plot(rocs, last_rep=True, pairs=False):
    ax = None

    for i, r_name in enumerate(rocs):
        ROC_data_narray = read_file(base_path+r_name)
        print(r_name)
        if pairs:
            *_, ax = plot_roc_curve(ROC_data_narray[..., 0], ROC_data_narray[..., 1], legend_size=15,
                                label_additional=r_name, title_additional=' lfw',
                                subplot_instance=ax, color=plot_colors[1 + (i//2) % (len(plot_colors) - 1)],
                                linestyle=('dashed' if i%2 else None))
        else:
            *_, ax = plot_roc_curve(ROC_data_narray[..., 0], ROC_data_narray[..., 1], legend_size=18,
                                label_additional=r_name, title_additional=' lfw',
                                subplot_instance=ax, color=plot_colors[1 + i % (len(plot_colors) - 1)])

    plt.show()

def calc_diff(rocs_path, base_idx=-1):
    roc_base = read_file(base_path+rocs[-1])
    fpr = roc_base[..., 0]
    tpr = roc_base[..., 1]
    values = [1e-6, 8e-5, 5e-5, 3e-5, 1e-5, 8e-4, 5e-4, 3e-4, 1e-4, 8e-3, 5e-3, 3e-3, 1e-3, 1e-2]
    fpr_values_base = np.array([tpr[find_nearest(fpr, val)] for val in values])
    res = []
    for i, r_name in enumerate(rocs):
        ROC_data_narray = read_file(base_path+r_name)
        fpr = ROC_data_narray[..., 0]
        tpr = ROC_data_narray[..., 1]
        fpr_values = np.array([tpr[find_nearest(fpr, val)] for val in values])
        res.append(sum(np.abs(fpr_values_base - fpr_values)))
    return res

def help_message():
    print("Bad arguments...\n")
    print("Usage params:")
    print("python ./draw_roc_curves.py <rocs_folder> <roc_points_files> \n")
    print("Example:")
    print("python ./draw_roc_curves.py \ \n\t./base_roc \ \n\t11.1000 \ \n\t10.100 \ \n\t10.30 \ \n\t9.300")
    sys.exit()

base_path = './base_roc/'
rocs = []

if(len(sys.argv) >= 2):
    base_path = sys.argv[1]
    for i in range(2, len(sys.argv)):
        rocs += [sys.argv[i]]
else:
    help_message()

diff = np.array(calc_diff(rocs))
rocs = np.array(rocs)

plot(rocs[np.argsort(diff)])