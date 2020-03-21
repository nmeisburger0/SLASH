# SLASH
Sketching based distributed locally sensitive hashing algorithm for similarity search with ultra high dimensionality datasets.

## Instructions
1. Download and unzip either the `criteo_tb`, `webspam_wc_normalized_trigram.svm.bz2`, or `kdd12.bz2` from https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html
2. In the benchmarking.h file, uncomment which dataset you are using. For kdd12 and webspam also uncomment either FILE_OUTPUT or EVAL_SIM depending on if you want the program to output the results to a file, or evaluate the simarity and print the result. Note that EVAL_SIM requires being able to load the dataset into RAM. Also make sure to set the basefile to be the path to the downloaded dataset.
3. Compile the program. On NOTS or a similar HPC cluster running `$ source setup.sh` from the `src` directory will load the necessary MPI library and compiler. We compile and ran our system using OpenMPI/3.1.4 and the mpicxx compiler in GCC/8.3.0.
4. Run `$ make clean; make` to compile the program.
5. The *.slurm files contain the slurm scripts for running the system using the slurm job scheduler. You can edit the size of the cluster as well as some other properties here. Make sure to set the job name at the top and the email that slurm will send notifications to.
6. Run `$ sbatch myjob.slurm` for webspam or kdd12 or `$ sbatch criteo.slurm` for criteo.