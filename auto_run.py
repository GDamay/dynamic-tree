import os
import pandas as pd
import matplotlib.pyplot as plt

def print_and_perform(command):
    print('Performing "' + command +'"')
    os.system(command)

seed_min = 0
seed_max = 9
eps_min = 0.01
eps_max = 1
eps_step = 0.01
path_to_executable = './Build/dynamic_tree'
data_folder = 'Example'
config_name = 'parameters'
config_extension = '.csv'
general_results_folder = 'Results'
partial_temp_folder = 'temp'
partial_final_folder = 'final'
partial_images_folder='images'
path_to_params_file = data_folder + '/' + config_name + config_extension;
font_size = 20

temp_results_folder = general_results_folder + '/' + partial_temp_folder + '/' + config_name
final_results_folder = general_results_folder + '/' + partial_final_folder
images_folder = general_results_folder + '/' + partial_images_folder  + '/' + config_name
print_and_perform('mkdir -p ' + temp_results_folder)
print_and_perform('mkdir -p ' + final_results_folder)
print_and_perform('mkdir -p ' + images_folder)

# --- Running algorithm
list_of_df = []
for i in range(seed_max-seed_min+1):
    print_and_perform(path_to_executable + ' --param_file ' + path_to_params_file + ' -a 1 -c -r ' + str(i) + ' -e ' + str(eps_min) + ' -f ' + str(eps_max) + ' -j ' + str(eps_step) + ' > ' + temp_results_folder + '/' + str(i) + '.csv')
    list_of_df += [pd.read_csv(temp_results_folder + '/' + str(i) + '.csv', 
												sep=';',
												names=['seed', 'epsilon', 'TP', 'TN', 'FP', 'FN', 'init_time', 'iter_time', 'nb_build', 'MTE'])]

# --- Computing results
def harmonic_mean(var1, var2):
	return 2*var1*var2/(var1+var2)

df_concat = pd.concat(list_of_df)
df_mean = df_concat.groupby(df_concat.epsilon).mean()

df_mean.to_csv(final_results_folder + '/' + config_name + '.csv',
							sep=';', header=False)

# Specific to sliding window
event_number=2*(df_mean['TP'] + df_mean['TN'] + df_mean['FP'] + df_mean['FN'])
df_mean['Tprec']=df_mean['TP']/(df_mean['TP']+df_mean['FP'])
df_mean['Trec']=df_mean['TP']/(df_mean['TP']+df_mean['FN'])
df_mean['Fprec']=df_mean['TN']/(df_mean['TN']+df_mean['FN'])
df_mean['Frec']=df_mean['TN']/(df_mean['TN']+df_mean['FP'])
df_mean['TF1'] = harmonic_mean(df_mean['Tprec'], df_mean['Trec'])
df_mean['FF1'] = harmonic_mean(df_mean['Fprec'], df_mean['Frec'])
df_mean['TPE']=df_mean['iter_time']/event_number

plt.figure();
plt.rc('font', size=font_size)
df_mean.plot(y=['TF1'], ax=plt.gca(), label=['F1-score'])
plt.ylabel('F1-score')
plt.xlabel('ε')
plt.xlim(0,df_mean.index.max())
plt.savefig(images_folder + '/' + config_name + '_F1_score.png', bbox_inches='tight')

plt.figure()
plt.rc('font', size=font_size)
df_mean.plot(y='TPE', label='Time per event', ax=plt.gca(), logy=True)
plt.ylabel('Time (ms)')
plt.xlabel('ε')
plt.xlim(0,df_mean.index.max())
plt.savefig(images_folder + '/' + config_name + '_iter_time.png', bbox_inches='tight')
