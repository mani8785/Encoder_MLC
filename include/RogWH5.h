#ifndef ROGWH5_H_
#define ROGWH5_H_

#include <iostream>
#include <sstream>
#include <itpp/itcomm.h>
#include <itpp/stat/misc_stat.h>
#include <itpp/itbase.h>

#include "H5Cpp.h"

using namespace std;
using namespace itpp;
using namespace H5;

/*
    read and write from/to files frame by frame for 2D dataset
*/
void add_dataset(H5File &hdf5file, string DATASET_NAME, string dtype, hsize_t FSPACE_RANK, hsize_t Dims[], DataSpace &fspace, DataSet &dataset);
void add_dataset(H5File &hdf5file, string DATASET_NAME, PredType pre_def_type, hsize_t FSPACE_RANK, hsize_t Dims[], DataSpace &fspace, DataSet &dataset);

void get_dataset_info(string Fname, string Dname, H5T_class_t &Ctype, int &Rank, hsize_t Dims[], size_t &type_size, bool Disp_flag);
void get_dataset_info(string Fname, string Dname, int &Rank, hsize_t Dims[], H5std_string &my_dtype, bool Disp_flag);
void reshape_h5(string FnameInp, string Dnames[], int numdatasets, string FnameOut);
void reshape_h5_2D_to_1D(string FnameInp, string Dnames[], int numdatasets, string FnameOut);
void reshape_h5_1D_to_2D(string FnameInp, string Dnames[], int numdatasets, string FnameOut, hsize_t CFL);
void combine_datasets(string FnameInp, string Dnames[], int numdatasets, string FnameOut, string Dnames_out[]);




/*
    2D read and write functions
    File:
        Dname:
               0,0     0,1    ...   0, Ncol
               1,0     1,1    ...   1, Ncol
                .       .     ...     .
                .       .     ...     .
                .       .     ...     .
            Nrow,0   Nrow,1   ... Nrow, Ncol
*/
void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, vec &data_read); 
void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, ivec &data_read);
void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, bvec &data_read);

void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, vec &data_read); 
void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, ivec &data_read);
void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, bvec &data_read);

void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, vec &ith_col_buffer);
void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, ivec &ith_col_buffer);
void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, bvec &ith_col_buffer);

void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t row_j, vec &row_j_buffer);
void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t row_j, ivec &row_j_buffer);
void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t row_j, bvec &row_j_buffer);

/*
        Read and write for 1D data set
        File:
        Dname:
               0,0     0,1    ...   0, Ncol
*/

void read1D_subset(string Fname, string Dname, hsize_t Steps, hsize_t index_i, vec &data_read);  // new
void read1D_subset(string Fname, string Dname, hsize_t Steps, hsize_t index_i, ivec &data_read); // new
void read1D_subset(string Fname, string Dname, hsize_t Steps, hsize_t index_i, bvec &data_read); // new

void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, vec &buffer_data);
void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, ivec &buffer_data);
void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, bvec &buffer_data);


void read3D_index_z(string Fname, string Dname, hsize_t Count_Col, hsize_t Count_Row, hsize_t z_index, mat &buffer_mat); 
void write3D_index_z(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count_Col, hsize_t Count_row, hsize_t zth_frame, mat &buffer_mat);


#endif /* class ROGWH5_H_ */
