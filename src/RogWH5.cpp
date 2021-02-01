#include <iostream>
#include "md.h"
#include "RogWH5.h"

void add_dataset(H5File &hdf5file, string DATASET_NAME, PredType pre_def_type, hsize_t FSPACE_RANK, hsize_t Dims[], DataSpace &fspace, DataSet &dataset)
{

    Dims[0];
    if (pre_def_type == PredType::STD_I8LE)
    {
        /*
            * Create property list for a dataset and set up fill values.
        */
        int8_t fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::STD_I8LE, &fillvalue);
        /*
    * Create dataspace for the syndrome dataset in the file.
    */
        fspace = DataSpace(FSPACE_RANK, Dims);
        /*
        *    Create dataset and write it into the file.
        */
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::STD_I8LE, fspace, plist);
    }
    else if (pre_def_type == PredType::IEEE_F32LE)
    {
        /*
            * Create property list for a dataset and set up fill values.
        */
        float fillvalue = 0.0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::IEEE_F32LE, &fillvalue); // fill value type
        /*
    * Create dataspace for the syndrome dataset in the file.
    */
        fspace = DataSpace(FSPACE_RANK, Dims);
        /*
        *    Create dataset and write it into the file.
        */
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::IEEE_F32LE, fspace, plist);
    }
    else if (pre_def_type == PredType::STD_I16LE)
    {

        int16_t fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::STD_I16LE, &fillvalue);

        fspace = DataSpace(FSPACE_RANK, Dims);

        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::STD_I16LE, fspace, plist);
    }
    else if (pre_def_type == PredType::IEEE_F64LE)
    {
        double fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::IEEE_F64LE, &fillvalue);

        fspace = DataSpace(FSPACE_RANK, Dims);

        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::IEEE_F64LE, fspace, plist);
    }
    else if (pre_def_type == PredType::NATIVE_HBOOL)
    {
        bool fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::NATIVE_HBOOL, &fillvalue);

        fspace = DataSpace(FSPACE_RANK, Dims);

        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::NATIVE_HBOOL, fspace, plist);
    }
    else
    {
        it_error("Do not support this data type");
    }
};

void add_dataset(H5File &hdf5file, string DATASET_NAME, string dtype, hsize_t FSPACE_RANK, hsize_t Dims[], DataSpace &fspace, DataSet &dataset)
{
    /*
        INPUT:
            File address and name
            Dataset name 
            type of the data
            Rank of the dataset
            the dimensions of the dataset
        OUTPUT:
            Filespace
            Dataset

    */
    /* Check the type of the codec input file
        0: bool/boolean
        1: int8
        2: int16
        3: float/IEEE32
        4: double/IEEE64
  */
    int type_ID = 5;
    size_t found_type = dtype.find("bool");
    if (found_type != std::string::npos)
        type_ID = 0;
    found_type = dtype.find("int8");
    if (found_type != std::string::npos)
        type_ID = 1;
    found_type = dtype.find("int16");
    if (found_type != std::string::npos)
        type_ID = 2;
    found_type = dtype.find("float");
    if (found_type != std::string::npos)
        type_ID = 3;
    found_type = dtype.find("double");
    if (found_type != std::string::npos)
        type_ID = 4;

    /*
        Common variables 
    */
    DSetCreatPropList plist;
    /*
    * Create dataspace for the syndrome dataset in the file.
    */
    fspace = DataSpace(FSPACE_RANK, Dims);
    bool fillvalue = 0;     /* Fill value for the dataset */
    int8_t fillvalue1 = 0;  /* Fill value for the dataset */
    int16_t fillvalue2 = 0; /* Fill value for the dataset syndrome */
    float fillvalue3 = 0.0; /* Fill value for the dataset syndrome */
    double fillvalue4 = 0;  /* Fill value for the dataset syndrome */

    switch (type_ID)
    {
    case 0:
        plist.setFillValue(PredType::NATIVE_HBOOL, &fillvalue);
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::NATIVE_HBOOL, fspace, plist);
        break;
    case 1:
        plist.setFillValue(PredType::STD_I8LE, &fillvalue1);
        /*
        *    Create dataset and write it into the file.
        */
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::STD_I8LE, fspace, plist);
        break;
    case 2:
        plist.setFillValue(PredType::STD_I16LE, &fillvalue2);
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::STD_I16LE, fspace, plist);
        break;
    case 3:
        plist.setFillValue(PredType::IEEE_F32LE, &fillvalue3); // fill value type
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::IEEE_F32LE, fspace, plist);
        break;
    case 4:
        plist.setFillValue(PredType::IEEE_F64LE, &fillvalue4);
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::IEEE_F64LE, fspace, plist);
        break;
    case 5:
        it_error("wrong type identifier.");
        break;
    default:
        break;
    }
};

/*
    read and write from/to files frame by frame
*/

void get_dataset_info(string Fname, string Dname, H5T_class_t &Ctype, int &Rank, hsize_t Dims[], size_t &type_size, bool DispF)
{
    /*
        Fname: File name: It could be the full address of the xxx.h5 
        Dname: Dataset name: It should be a valid name
        Ctype: H5 Class type: H5T_INTEGER, H5T_FLOAT, ... 
        Dtype: Stored data type: NATIVE_INT, NATIVE_FLOAT, NATIVE_DOUBLE, .... 
        Dims : Dimsnsions of the 2D data set
        DispF: Display Flag
    */
    // if (DispF)
    // {
    //     printf("\e[1m");
    //     printf("# %-58s \n", "================= Load Transmitter Data ");
    //     printf("# %68s \n", "---------------------------------------------------------------------- ");
    //     printf("# %16s | %16s | %16s | %16s \n", " H5 Class Type ", "Data Size (Byte)  ", "Rank  ", " Dimension  ");
    //     printf("# %68s \n", "---------------------------------------------------------------------- ");
    //     printf("\e[0m");
    // }
    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();
        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *dataset = new DataSet(file->openDataSet(Dname)); // our standard

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = dataset->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();

        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        int ndimsI = fspaceI.getSimpleExtentDims(Dims, NULL);
        Rank = ndimsI;

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        Ctype = dataset->getTypeClass();
        H5std_string order_string;
        H5T_order_t order;
        size_t size;

        /*
            * Get class of datatype and print message if it's an integer.
        */
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            /*
                * Get the float datatype
            */
            FloatType floatype = dataset->getFloatType();
            /*
                * Get order of datatype and print message if it's a little endian.
            */
            order = floatype.getOrder(order_string);
            /*
                * Get size of the data element stored in file and print it.
            */
            type_size = floatype.getSize();
            // cout << "Data size is " << size << endl;
            // if (DispF)
            //     printf("# %-16s | %-16d \n", " FLOAT", (int)type_size);

            if (DispF)
            {
                printf("\e[1m");
                printf("# --------------------------------------------------------\n");
                printf("# -------------- %8s", "Input data information \n");
                printf("# --------------------------------------------------------\n");
                printf("# * Dataset info: \n");
                printf("# \t ** %8s\t= %8s\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n",
                       "H5 Class Type", "FLOAT",
                       "Data Size (Byte)", (int)type_size,
                       "Rank", Rank);
                printf("# \n");
                printf("\e[0m");
            }
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {
            IntType inttype = dataset->getIntType();
            order = inttype.getOrder(order_string);
            type_size = inttype.getSize();
            // if (DispF)
            //     printf("# %-16s | %-16d \n", " INTEGER", (int)type_size);

            if (DispF)
            {
                printf("\e[1m");
                printf("# --------------------------------------------------------\n");
                printf("# -------------- %8s", "Tx data information \n");
                printf("# --------------------------------------------------------\n");
                printf("# * Dataset info: \n");
                printf("# \t ** %8s\t= %8s\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n",
                       "H5 Class Type", "INTEGER",
                       "Data Size (Byte)", (int)type_size,
                       "Rank", Rank);
                printf("# \n");
                printf("\e[0m");
            }
        }

        delete dataset;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void get_dataset_info(string Fname, string Dname, int &Rank, hsize_t Dims[], H5std_string &my_dtype, bool DispF)
{
    /*
        INPUT:
            Fname: File name: It could be the full address of the xxx.h5 
            Dname: Dataset name: It should be a valid name
        
        OUTPUT:
            Rank : 1D, 2D or 3D
            Dims : Dimsnsions of the 1D/2D/3D data set
            dtype: our supported datatypes
            DispF: Display Flag
    */
    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();
        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *dataset = new DataSet(file->openDataSet(Dname)); // our standard
        /*
            * Get dataspace of the dataset.
        */
        DataSpace fspaceI = dataset->getSpace();
        /*
            * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
            * Get the dimension size of each dimension in the dataspace and
            * display them.
        */
        int ndimsI = fspaceI.getSimpleExtentDims(Dims, NULL);
        Rank = ndimsI;
        /*
            * Get the class of the datatype that is used by the dataset.
        */
        size_t type_size;
        H5T_class_t Ctype = dataset->getTypeClass();
        H5std_string order_string;
        H5T_order_t order;

        /*
            * Get class of datatype and print message if it's an integer.
        */
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            /*
                * Get the float datatype
            */
            FloatType floatype = dataset->getFloatType();
            /*
                * Get order of datatype and print message if it's a little endian.
            */
            order = floatype.getOrder(order_string);
            /*
                * Get size of the data element stored in file and print it.
            */
            type_size = floatype.getSize();
            // cout << "Data size is " << size << endl;
            if (type_size == 4)
            {
                my_dtype = "float";
            }
            else if (type_size == 8)
            {
                my_dtype = "double";
            }

            if (DispF)
            {
                if (Rank == 1)
                {
                    Dims[1] = 1;
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Input data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "FLOAT/DOUBLE",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else if (Rank == 2)
                {
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "FLOAT/DOUBLE",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else if (Rank == 3)
                {
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-4d, %8d, %8d)\n",
                           "H5 Class Type", "FLOAT/DOUBLE",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1], (int)Dims[2]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else
                {
                    it_error("only support 1D or 2D datasets");
                }
            }
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {
            IntType inttype = dataset->getIntType();
            order = inttype.getOrder(order_string);
            type_size = inttype.getSize();
            if (type_size == 1)
            {
                my_dtype = "int8";
            }
            else if (type_size == 2)
            {
                my_dtype = "int16";
            }

            if (DispF)
            {
                if (Rank == 1)
                {
                    Dims[1] = 1;
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Input data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "INTEGER",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else if (Rank == 2)
                {
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "INTEGER",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else if (Rank == 3)
                {
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-4d, %8d, %8d)\n",
                           "H5 Class Type", "INTEGER",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1], (int)Dims[2]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else
                {
                    it_error("only support 1D or 2D datasets");
                }
            }
        }

        delete dataset;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void reshape_h5(string FnameInp, string Dnames[], int numdatasets, string FnameOut)
{
    H5T_class_t Ctype; // class type
    size_t size_type;  // size of the data in byte
    hsize_t Dims[2];   // dataset dimensions
    int Rank;
    get_dataset_info(FnameInp, Dnames[0], Ctype, Rank, Dims, size_type, false);

    // ToDo check that staructure of the two datasets are the same: Size and Format
    if (Dims[1] >= 2)
    {
        /*
        Create a *.h5 file to wtite the reshaped data in it
        */
        H5File file(FnameOut, H5F_ACC_TRUNC);
        DataSpace fspace;
        DataSet dataset;
        DataSpace fspaceQ;
        DataSet datasetQ;
        hsize_t Total_lements[1];
        Total_lements[0] = Dims[0] * Dims[1];

        if (Ctype == H5T_FLOAT)
        {
            vec temp_col_vals;
            temp_col_vals.set_length(Dims[0], false);
            if (size_type == 4)
            {
                add_dataset(file, Dnames[0], PredType::IEEE_F32LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[0], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(dataset, fspace, "float", Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F32LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[1], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(datasetQ, fspaceQ, "float", Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ
            }
            else if (size_type == 8)
            {
                add_dataset(file, Dnames[0], PredType::IEEE_F64LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[0], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(dataset, fspace, "double", Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F64LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[1], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(datasetQ, fspaceQ, "double", Dims[0], ifc, temp_col_vals);
                } // end of frames forn TxQ
            }
            else
            {
                it_error("Do not support float with this size");
            }
        }
        else if (Ctype == H5T_INTEGER)
        {
            if (size_type == 1)
            {
                add_dataset(file, Dnames[0], PredType::STD_I8LE, 2, Total_lements, fspace, dataset);
                add_dataset(file, Dnames[1], PredType::STD_I8LE, 2, Total_lements, fspaceQ, datasetQ);
            }
            else if (size_type == 2)
            {
                add_dataset(file, Dnames[0], PredType::STD_I16LE, 2, Total_lements, fspace, dataset);
                add_dataset(file, Dnames[1], PredType::STD_I16LE, 2, Total_lements, fspaceQ, datasetQ);
            }
            else
            {
                it_error("Do not support integer with this size");
            }
        }
        else
        {
            it_error("Do not support other formats (reshape func)");
        }

        /*
     * Reset the selection for the file dataspace fid.
     */
        fspace.selectNone();
    }
    else
    {
        it_error("create a 1D copy is not possible.");
    }
};

void reshape_h5_2D_to_1D(string FnameInp, string Dnames[], int numdatasets, string FnameOut)
{
    H5T_class_t Ctype; // class type
    size_t size_type;  // size of the data in byte
    hsize_t Dims[2];   // dataset dimensions
    int Rank;
    string dtype;
    // get_dataset_info(FnameInp, Dnames[0], Ctype, Rank, Dims, size_type, false);
    get_dataset_info(FnameInp, Dnames[0], Rank, Dims, dtype, false);
    if (Rank == 1)
    {
        it_error("The dataset is 1D");
    }
    else if (Rank == 2)
    {
        // ToDo check that staructure of the two datasets are the same: Size and Format
        if (Dims[1] >= 2)
        {
            /*
                Create a *.h5 file to wtite the reshaped data in it
            */
            H5File file(FnameOut, H5F_ACC_TRUNC);
            DataSpace fspace;
            DataSet dataset;
            DataSpace fspaceQ;
            DataSet datasetQ;
            hsize_t Total_lements[1];
            Total_lements[0] = Dims[0] * Dims[1];
            if (dtype == "float" || dtype == "double")
            {
                vec temp_col_vals;
                temp_col_vals.set_length(Dims[0], false);
                add_dataset(file, Dnames[0], dtype, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[0], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(dataset, fspace, dtype, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI
                add_dataset(file, Dnames[1], dtype, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[1], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(datasetQ, fspaceQ, dtype, Dims[0], ifc, temp_col_vals);
                } // end of frames forn TxQ
            } else if (dtype == "int8" || dtype == "int16")
            {
                ivec temp_col_vals;
                temp_col_vals.set_length(Dims[0], false);
                add_dataset(file, Dnames[0], dtype, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[0], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(dataset, fspace, dtype, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], dtype, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read2D_from_col_i(FnameInp, Dnames[1], Dims[0], ifc, temp_col_vals);
                    write1D_dataset_subset(datasetQ, fspaceQ, dtype, Dims[0], ifc, temp_col_vals);
                } // end of frames forn TxQ
            }

            /*
                * Reset the selection for the file dataspace fid.
            */
            fspace.selectNone();
            fspaceQ.selectNone();
        }
        else
        {
            it_error("create a 1D copy is not possible.");
        }
    }
};

void reshape_h5_1D_to_2D(string FnameInp, string Dnames[], int numdatasets, string FnameOut, hsize_t CFL)
{
    int Rank;
    hsize_t Dims[2]; // dataset dimensions
    string dtype;
    get_dataset_info(FnameInp, Dnames[0], Rank, Dims, dtype, false);

    if (Rank == 2) // ToDo 1D in 2D
    {
        it_error("Input data set has rank larger than 1");
    }
    else if (Rank == 1)
    {
        H5File file(FnameOut, H5F_ACC_TRUNC);
        DataSpace fspace;
        DataSet dataset;
        DataSpace fspaceQ;
        DataSet datasetQ;
        hsize_t Total_lements;
        Total_lements = Dims[0];
        hsize_t TFN = Total_lements / CFL;
        hsize_t newDims[2] = {TFN, CFL};

        if (dtype == "float" || dtype == "double")
        {
            vec temp_col_vals;
            temp_col_vals.set_length(CFL, false);
            add_dataset(file, Dnames[0], dtype, 2, newDims, fspace, dataset);
            for (size_t ifc = 0; ifc < TFN; ifc++)
            {
                read1D_subset(FnameInp, Dnames[0], CFL, ifc, temp_col_vals);
                write2D_to_dataset_row_j(dataset, fspace, dtype, CFL, ifc, temp_col_vals);

            } // end of frames for TxI

            add_dataset(file, Dnames[1], dtype, 2, newDims, fspaceQ, datasetQ);
            for (size_t ifc = 0; ifc < TFN; ifc++)
            {
                read1D_subset(FnameInp, Dnames[1], CFL, ifc, temp_col_vals);
                write2D_to_dataset_row_j(datasetQ, fspaceQ, dtype, CFL, ifc, temp_col_vals);
            } // end of frames forn TxQ
        }
        else if (dtype == "int8" || dtype == "int16")
        {
            ivec temp_col_vals;
            temp_col_vals.set_length(CFL, false);
            add_dataset(file, Dnames[0], dtype, 2, newDims, fspace, dataset);
            for (size_t ifc = 0; ifc < TFN; ifc++)
            {
                read1D_subset(FnameInp, Dnames[0], CFL, ifc, temp_col_vals);
                write2D_to_dataset_row_j(dataset, fspace, dtype, CFL, ifc, temp_col_vals);

            } // end of frames for TxI

            add_dataset(file, Dnames[1], dtype, 2, newDims, fspaceQ, datasetQ);
            for (size_t ifc = 0; ifc < TFN; ifc++)
            {
                read1D_subset(FnameInp, Dnames[1], CFL, ifc, temp_col_vals);
                write2D_to_dataset_row_j(datasetQ, fspaceQ, dtype, CFL, ifc, temp_col_vals);
            } // end of frames forn TxQ
        }

        fspace.selectNone();
        fspaceQ.selectNone();
    }
};





void combine_datasets(string FnameInp, string Dnames[], int numdatasets, string FnameOut, string Dnames_out[])
{
    /*
        The input file dataset is like this
            TxI0_______ 1D dataset with length L
            TxI1_______ 1D dataset with length L
            .
            .
            .
            TxIn_______ 1D dataset with length L
        
        The output dataset is then
            TxI_______ 1D dataset with length L*(n+1)
    */
    hsize_t Dims[2];   // dataset dimensions
    int Rank;
    string dtype;
    get_dataset_info(FnameInp, Dnames[0], Rank, Dims, dtype, false);
    if (Rank > 1)
    {
        it_error("Each group should be a 1D vector");
    }else if (Rank == 1)
    {
        /*
        Create a *.h5 file to wtite the combined data in it
        */
        H5File file(FnameOut, H5F_ACC_TRUNC);
        DataSpace fspace;
        DataSet dataset;
        DataSpace fspaceQ;
        DataSet datasetQ;
        hsize_t Total_lements[1];
        Dims[1] = (hsize_t) numdatasets;
        Total_lements[0] = Dims[0] * Dims[1];

        if (dtype == "float" || dtype == "double")
        {
            vec temp_col_vals;
            temp_col_vals.set_length(Dims[0], false);
            add_dataset(file, Dnames_out[0], dtype, 1, Total_lements, fspace, dataset);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[0]+to_str(ifc), Dims[0], 0, temp_col_vals); // ToDo check the correct name
                // cout << temp_col_vals.left(10) << endl;
                write1D_dataset_subset(dataset, fspace, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
            
            add_dataset(file, Dnames_out[1], dtype, 1, Total_lements, fspaceQ, datasetQ);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[1]+to_str(ifc), Dims[0], 0, temp_col_vals); // ToDo check the correct name
                write1D_dataset_subset(datasetQ, fspaceQ, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
        }
        else if (dtype == "int8" || dtype == "int16")
        {
            ivec temp_col_vals;
            temp_col_vals.set_length(Dims[0], false);
            add_dataset(file, Dnames_out[0], dtype, 1, Total_lements, fspace, dataset);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[0]+to_str(ifc), Dims[0], 0, temp_col_vals);// ToDo check the correct name
                // cout << temp_col_vals.left(10) << endl;
                write1D_dataset_subset(dataset, fspace, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
            
            add_dataset(file, Dnames_out[1], dtype, 1, Total_lements, fspaceQ, datasetQ);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[1]+to_str(ifc), Dims[0], 0, temp_col_vals); // ToDo check the correct name
                write1D_dataset_subset(datasetQ, fspaceQ, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
            
        }
        else if (dtype == "bool")
        {
            bvec temp_col_vals;
            temp_col_vals.set_length(Dims[0], false);
            add_dataset(file, Dnames_out[0], dtype, 1, Total_lements, fspace, dataset);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[0]+to_str(ifc), Dims[0], 0, temp_col_vals);
                write1D_dataset_subset(dataset, fspace, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
            
            add_dataset(file, Dnames_out[1], dtype, 1, Total_lements, fspaceQ, datasetQ);
            for (size_t ifc = 0; ifc < Dims[1]; ifc++)
            {
                read1D_subset(FnameInp, Dnames_out[1]+to_str(ifc), Dims[0], 0, temp_col_vals);
                write1D_dataset_subset(datasetQ, fspaceQ, dtype, Dims[0], ifc, temp_col_vals);
            } // end of frames for TxI
        }
        else
        {
            it_error("Data type is not valid.");
        }
        
        /*
            *   Reset the selection for the file dataspace fid.
            */
        fspace.selectNone();
        fspaceQ.selectNone();
        
        
    }
};


/*
    READ AND WRITE 1D
*/

void read1D_subset(string Fname, string Dname, hsize_t Step, hsize_t index_i, vec &data_read)
{
    /*
        L is the half of the cw length
    */
    int i, j;
    double TxI_buffer[Step]; /* output buffer */
    for (j = 0; j < Step; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        if (rankI > 1)
        {
            it_error("This is a 2D data set!");
        }

        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[1];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
    */
        hsize_t mdim[] = {Step}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(1, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        blockm[0] = 1;
        stridem[0] = 1;
        countm[0] = Step;

        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = Step * index_i;
        block[0] = 1;
        stride[0] = 1;
        count[0] = Step; // dims_outI[0];
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */

        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);
        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::IEEE_F64LE, mspaceI, fspaceI);
        data_read.set_length(Step, false);
        for (j = 0; j < Step; j++)
        {
            // cout << "out_j = " << TxI_buffer[j] << endl;
            data_read(j) = TxI_buffer[j];
        }
        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
}; // new

void read1D_subset(string Fname, string Dname, hsize_t Step, hsize_t index_i, ivec &data_read)
{
    /*
        L is the half of the cw length
    */
    int i, j;
    int16_t TxI_buffer[Step]; /* output buffer */
    for (j = 0; j < Step; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        if (rankI > 1)
        {
            it_error("This is a 2D data set!");
        }

        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[1];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
    */
        hsize_t mdim[] = {Step}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(1, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        blockm[0] = 1;
        stridem[0] = 1;
        countm[0] = Step;

        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = Step * index_i;
        block[0] = 1;
        stride[0] = 1;
        count[0] = Step; // dims_outI[0];
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */

        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);
        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::STD_I16LE, mspaceI, fspaceI);
        data_read.set_length(Step, false);
        for (j = 0; j < Step; j++)
        {
            // cout << "out_j = " << TxI_buffer[j] << endl;
            data_read(j) = (int) TxI_buffer[j];
        }
        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read1D_subset(string Fname, string Dname, hsize_t Step, hsize_t index_i, bvec &data_read)
{
    /*
        L is the half of the cw length
    */
    int i, j;
    bool TxI_buffer[Step]; /* output buffer */
    for (j = 0; j < Step; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        if (rankI > 1)
        {
            it_error("This is a 2D data set!");
        }

        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[1];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
    */
        hsize_t mdim[] = {Step}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(1, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        blockm[0] = 1;
        stridem[0] = 1;
        countm[0] = Step;

        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = Step * index_i;
        block[0] = 1;
        stride[0] = 1;
        count[0] = Step; // dims_outI[0];
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */

        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);
        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::NATIVE_HBOOL, mspaceI, fspaceI);
        data_read.set_length(Step, false);
        for (j = 0; j < Step; j++)
        {
            // cout << "out_j = " << TxI_buffer[j] << endl;
            data_read(j) = TxI_buffer[j];
        }
        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
}; // new

void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, vec &buffer_data)
{
    const int MSPACE1_RANK = 1; // Rank of the first dataset in memory
    int i, j;                   // loop indices */

    try
    {
        /*
        * Turn off the auto-printing when failure occurs so that we can
        * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
        * Select hyperslab for the dataset in the file, using 1x1 blocks,
        * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = 0;
        stride[0] = 1;
        count[0] = 1;
        block[0] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
        * Select hyperslab.
        * We will use L elements of the vector buffer starting at the
        * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        stridem[0] = 1;
        countm[0] = 1;
        blockm[0] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            * Write selection from the vector buffer to the dataset in the file.
        */
        double buff_vector[Count]; // vector buffer for dset

        start[0] = index_i * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
            * Buffer update.
        */
        // for (i = 0; i < Count; i++)
        //     buff_vector[i] = buffer_data(i);

        // dataset.write(buff_vector, pre_def_type, mspace1, fspace);

        if (dtype == "float")
        {
            float buff_vector[Count]; // vector buffer for dset
            for (i = 0; i < Count; i++)
                buff_vector[i] = (float)buffer_data(i);

            dataset.write(buff_vector, PredType::IEEE_F32LE, mspace1, fspace);
        }
        else if (dtype == "double")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            double buff_vector[Count]; // vector buffer for dset
            /*
            * Buffer update.
            */
            for (i = 0; i < Count; i++)
                buff_vector[i] = buffer_data(i);

            dataset.write(buff_vector, PredType::IEEE_F64LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 1D write col i double");
        }

    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, ivec &buffer_data)
{
    const int MSPACE1_RANK = 1; // Rank of the first dataset in memory
    int i, j;                   // loop indices */

    try
    {
        /*
        * Turn off the auto-printing when failure occurs so that we can
        * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
        * Select hyperslab for the dataset in the file, using 1x1 blocks,
        * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = 0;
        stride[0] = 1;
        count[0] = 1;
        block[0] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
        * Select hyperslab.
        * We will use L elements of the vector buffer starting at the
        * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        stridem[0] = 1;
        countm[0] = 1;
        blockm[0] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            * Write selection from the vector buffer to the dataset in the file.
        */
        // double buff_vector[Count]; // vector buffer for dset

        start[0] = index_i * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
            * Buffer update.
        */
        // for (i = 0; i < Count; i++)
        //     buff_vector[i] = buffer_data(i);

        // dataset.write(buff_vector, pre_def_type, mspace1, fspace);

        if (dtype == "int8")
        {
            int8_t buff_vector[Count]; // vector buffer for dset
            for (i = 0; i < Count; i++)
                buff_vector[i] = (int8_t) buffer_data(i);

            dataset.write(buff_vector, PredType::STD_I8LE, mspace1, fspace);
        }
        else if (dtype == "int16")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            int16_t buff_vector[Count]; // vector buffer for dset
            /*
            * Buffer update.
            */
            for (i = 0; i < Count; i++)
                buff_vector[i] = (int16_t) buffer_data(i);

            dataset.write(buff_vector, PredType::STD_I16LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 1D write col i int");
        }

    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write1D_dataset_subset(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t index_i, bvec &buffer_data)
{
    const int MSPACE1_RANK = 1; // Rank of the first dataset in memory
    int i, j;                   // loop indices */

    try
    {
        /*
        * Turn off the auto-printing when failure occurs so that we can
        * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
        * Select hyperslab for the dataset in the file, using 1x1 blocks,
        * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = 0;
        stride[0] = 1;
        count[0] = 1;
        block[0] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
        * Select hyperslab.
        * We will use L elements of the vector buffer starting at the
        * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[1];  // Start of hyperslab
        hsize_t stridem[1]; // Stride of hyperslab
        hsize_t countm[1];  // Block count
        hsize_t blockm[1];  // Block sizes
        startm[0] = 0;
        stridem[0] = 1;
        countm[0] = 1;
        blockm[0] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            * Write selection from the vector buffer to the dataset in the file.
        */
        bool buff_vector[Count]; // vector buffer for dset

        start[0] = index_i * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
            * Buffer update.
        */
        for (i = 0; i < Count; i++)
            buff_vector[i] = buffer_data(i);

        dataset.write(buff_vector, PredType::NATIVE_HBOOL, mspace1, fspace);

    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

/*
    READ and WRITE 2D dataset
*/
void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, vec &data_read)
{
    /*
        The shape of the matrix is : (Count , FN)
        Count is the number of row
        output type is double
    */
    int i, j;
    double TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {Count, 1}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = Count;
        countm[1] = 1;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = Count;
        count[1] = 1;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (col_i >= dims_outI[1])
        {
            it_error("The index is out of bound");
        }
        start[1] = col_i;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::IEEE_F64LE, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
            data_read.set(j, TxI_buffer[j]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, ivec &data_read)
{
    /*
        The shape of the matrix is : (Count , FN)
        Count is the number of row
        output type is double
    */
    int i, j;
    int16_t TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {Count, 1}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = Count;
        countm[1] = 1;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = Count;
        count[1] = 1;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (col_i >= dims_outI[1])
        {
            it_error("The index is out of bound");
        }
        start[1] = col_i;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::STD_I16LE, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
            data_read.set(j, (int) TxI_buffer[j]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read2D_from_col_i(string Fname, string Dname, hsize_t Count, int col_i, bvec &data_read)
{
    /*
        The shape of the matrix is : (Count , FN)
        Count is the number of row
        output type is double
    */
    int i, j;
    bool TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {Count, 1}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = Count;
        countm[1] = 1;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = Count;
        count[1] = 1;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (col_i >= dims_outI[1])
        {
            it_error("The index is out of bound (n_col)");
        }

        if (Count > dims_outI[0])
        {
            it_error("The index is out of bound (n_row)");
        }

        start[1] = col_i;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::NATIVE_HBOOL, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
            data_read.set(j, TxI_buffer[j]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, vec &data_read)
{
    /*
        The shape of the matrix is : (FN , COUNT)
        Count is the number of row
        output type is double
    */
    int i, j;
    double TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {1, Count}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = Count;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = Count;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (row_j >= dims_outI[0])
        {
            it_error("The index is out of bound (n_col)");
        }

        if (Count > dims_outI[1])
        {
            it_error("The index is out of bound (n_row)");
        }

        start[0] = row_j;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::IEEE_F64LE, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (i = 0; i < Count; i++)
            data_read.set(i, TxI_buffer[i]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, ivec &data_read)
{
    /*
        The shape of the matrix is : (FN , COUNT)
        Count is the number of row
        output type is double
    */
    int i, j;
    int16_t TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {1, Count}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = Count;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = Count;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (row_j >= dims_outI[0])
        {
            it_error("The index is out of bound (n_col)");
        }

        if (Count > dims_outI[1])
        {
            it_error("The index is out of bound (n_row)");
        }

        start[0] = row_j;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::STD_I16LE, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (i = 0; i < Count; i++)
            data_read.set(i, (int) TxI_buffer[i]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void read2D_from_row_j(string Fname, string Dname, hsize_t Count, int row_j, bvec &data_read)
{
    /*
        The shape of the matrix is : (FN , COUNT)
        Count is the number of row
        output type is double
    */
    int i, j;
    bool TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[2];
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {1, Count}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = Count;

        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        block[0] = 1;
        block[1] = 1;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = Count;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (row_j >= dims_outI[0])
        {
            it_error("The index is out of bound (n_col)");
        }

        if (Count > dims_outI[1])
        {
            it_error("The index is out of bound (n_row)");
        }

        start[0] = row_j;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        /*
            Read data back to the buffer
        */

        datasetI->read(TxI_buffer, PredType::NATIVE_HBOOL, mspaceI, fspaceI);

        data_read.set_length(Count, false);
        for (i = 0; i < Count; i++)
            data_read.set(i, TxI_buffer[i]);
        // data_read(j) = TxI_buffer[j];

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, vec &ith_col_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = Count; // MSPACE1_DIM;
        block[1] = 1;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count, 1}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = Count;
        blockm[1] = 1;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        if (dtype == "float")
        {
            float buff_vector[Count]; // vector buffer for dset

            start[1] = ith_col;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            for (jr = 0; jr < Count; jr++)
                buff_vector[jr] = (float)ith_col_buffer(jr);

            dataset.write(buff_vector, PredType::IEEE_F32LE, mspace1, fspace);
        }
        else if (dtype == "double")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            double buff_vector[Count]; // vector buffer for dset

            start[1] = ith_col;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            /*
            * Buffer update.
            */
            for (jr = 0; jr < Count; jr++)
                buff_vector[jr] = ith_col_buffer(jr);

            dataset.write(buff_vector, PredType::IEEE_F64LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 2D write col i double");
        }
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, ivec &ith_col_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = Count; // MSPACE1_DIM;
        block[1] = 1;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count, 1}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = Count;
        blockm[1] = 1;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        if (dtype == "int8")
        {
            int8_t buff_vector[Count]; // vector buffer for dset

            start[1] = ith_col;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            for (jr = 0; jr < Count; jr++)
                buff_vector[jr] = (int8_t) ith_col_buffer(jr);

            dataset.write(buff_vector, PredType::STD_I8LE, mspace1, fspace);
        }
        else if (dtype == "int16")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            int16_t buff_vector[Count]; // vector buffer for dset

            start[1] = ith_col;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            /*
            * Buffer update.
            */
            for (jr = 0; jr < Count; jr++)
                buff_vector[jr] = (int16_t) ith_col_buffer(jr);

            dataset.write(buff_vector, PredType::STD_I16LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 2D write col i double");
        }
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_col_i(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t ith_col, bvec &ith_col_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = Count; // MSPACE1_DIM;
        block[1] = 1;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {Count, 1}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = Count;
        blockm[1] = 1;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        bool buff_vector[Count]; // vector buffer for dset

        start[1] = ith_col;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
        for (jr = 0; jr < Count; jr++)
            buff_vector[jr] = ith_col_buffer(jr);

        dataset.write(buff_vector, PredType::NATIVE_HBOOL, mspace1, fspace);
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t row_j, vec &row_j_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = 1; // MSPACE1_DIM;
        block[1] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {1, Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = 1;
        blockm[1] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        if (dtype == "float")
        {
            float buff_vector[Count]; // vector buffer for dset

            start[0] = row_j;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            for (ic = 0; ic < Count; ic++)
                buff_vector[ic] = (float)row_j_buffer(ic);

            dataset.write(buff_vector, PredType::IEEE_F32LE, mspace1, fspace);
        }
        else if (dtype == "double")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            double buff_vector[Count]; // vector buffer for dset

            start[0] = row_j;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            /*
            * Buffer update.
            */
            for (ic = 0; ic < Count; ic++)
                buff_vector[ic] = row_j_buffer(ic);

            dataset.write(buff_vector, PredType::IEEE_F64LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 2D write col i double");
        }
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, size_t row_j, ivec &row_j_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = 1; // MSPACE1_DIM;
        block[1] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {1, Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = 1;
        blockm[1] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        if (dtype == "int8")
        {
            int8_t buff_vector[Count]; // vector buffer for dset

            start[0] = row_j;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            for (ic = 0; ic < Count; ic++)
                buff_vector[ic] = (int8_t) row_j_buffer(ic);

            dataset.write(buff_vector, PredType::STD_I8LE, mspace1, fspace);
        }
        else if (dtype == "int16")
        {
            /*
                * Write selection from the vector buffer to the dataset in the file.
            */
            int16_t buff_vector[Count]; // vector buffer for dset

            start[0] = row_j;
            fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            /*
            * Buffer update.
            */
            for (ic = 0; ic < Count; ic++)
                buff_vector[ic] = (int16_t) row_j_buffer(ic);

            dataset.write(buff_vector, PredType::STD_I16LE, mspace1, fspace);
        }
        else
        {
            it_error("PredType error: 2D write col i double");
        }
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write2D_to_dataset_row_j(DataSet &dataset, DataSpace &fspace, string type, hsize_t Count, size_t row_j, bvec &row_j_buffer)
{
    /*
        dataset shape (NROW, NCOL)
    */
    const int MSPACE1_RANK = 2; // Rank of the first dataset in memory
    int ic, jr;                 // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[2];  // Start of hyperslab
        hsize_t stride[2]; // Stride of hyperslab
        hsize_t count[2];  // Block count
        hsize_t block[2];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        stride[0] = 1;
        stride[1] = 1;
        count[0] = 1;
        count[1] = 1;
        block[0] = 1; // MSPACE1_DIM;
        block[1] = Count;

        /*
            * Create dataspace for the dataset.
        */
        hsize_t dim1[] = {1, Count}; /* Dimension size of the first dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
     * Select hyperslab.
     * We will use L elements of the vector buffer starting at the
     * zero element.  Selected elements are 0 2 3 . . . L-1
     */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = 1;
        blockm[0] = 1;
        blockm[1] = Count;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        bool buff_vector[Count]; // vector buffer for dset

        start[0] = row_j;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
        for (ic = 0; ic < Count; ic++)
            buff_vector[ic] = row_j_buffer(ic);

        dataset.write(buff_vector, PredType::NATIVE_HBOOL, mspace1, fspace);
    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

/*
    Write for mapping (The output is allways double)
*/

void read3D_index_z(string Fname, string Dname, hsize_t Count_Col, hsize_t Count_Row, hsize_t z_index, mat &buffer_mat)
{
    /*
        The shape of the matrix is : (Count_Row , COUNT_Col, Count_Z)
        output type is double
    */
    int i, j;
    float TxI_buffer[Count_Col]; /* output buffer */
    for (j = 0; j < Count_Col; j++)
        TxI_buffer[j] = 0;

    try
    {
        /*
            * Turn off the auto-printing when failure occurs so that we can
            * handle the errors appropriately
        */
        Exception::dontPrint();

        /*
            * Open the specified file and the specified dataset in the file.
       */
        H5File *file = new H5File(Fname, H5F_ACC_RDONLY);
        DataSet *datasetI = new DataSet(file->openDataSet(Dname)); // our standard

        /*
            * Get the class of the datatype that is used by the dataset.
        */
        H5T_class_t type_classI = datasetI->getTypeClass();

        /*
        * Get dataspace of the dataset.
        */
        DataSpace fspaceI = datasetI->getSpace();
        /*
        * Get the number of dimensions in the dataspace.
        */
        int rankI = fspaceI.getSimpleExtentNdims();
        /*
        * Get the dimension size of each dimension in the dataspace and
        * display them.
        */
        hsize_t dims_outI[3]; // ToDo
        int ndimsI = fspaceI.getSimpleExtentDims(dims_outI, NULL);

        /*
        * Create memory dataspace.
        */
        hsize_t mdim[] = {1, Count_Col}; /* Dimension sizes of the
                                                   dataset in memory when we
                                                   read selection from the
                                                   dataset on the disk */
        DataSpace mspaceI(2, mdim);
        /*
        * Select hyperslab in memory and file dataspace. Hyperslab has the same
        * size and shape as the selected hyperslabs for the file dataspace.
    */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        blockm[0] = 1;
        blockm[1] = 1;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = Count_Col;

        hsize_t start[3];  // Start of hyperslab
        hsize_t stride[3]; // Stride of hyperslab
        hsize_t count[3];  // Block count
        hsize_t block[3];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        block[1] = 1;
        block[2] = 1;
        stride[0] = 1;
        stride[1] = 1;
        stride[2] = 1;
        count[0] = 1;
        count[1] = Count_Col;
        count[2] = 1;
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */
        if (Count_Row > dims_outI[0])
        {
            it_error("3D dataset, The index is out of bound (n_col)");
        }

        if (Count_Col > dims_outI[1])
        {
            it_error("3D dataset, The index is out of bound (n_row)");
        }

        if (z_index >= dims_outI[2])
        {
            it_error("3D dataset, The index is out of bound (z_index)");
        }

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        start[2] = z_index;
        buffer_mat.set_size(Count_Row, Count_Col, false);
        for (size_t j = 0; j < Count_Row; j++)
        {
            start[0] = j;
            start[1] = 0;
            fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
            /*
                Read data back to the buffer
            */
            datasetI->read(TxI_buffer, PredType::IEEE_F32LE, mspaceI, fspaceI);
            for (i = 0; i < Count_Col; i++)
                buffer_mat.set(j, i, (double) TxI_buffer[i]);
        }

        delete datasetI;
        delete file;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};

void write3D_index_z(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count_Col, hsize_t Count_row, hsize_t zth_frame, mat &buffer_mat)
{
    /*
        mat MxBogU(dim, fl); OUR standard
        check if fl == Count and dim == given dim
    */
    const int MSPACE1_RANK = 2; // Rank of the dataset in memory
    hsize_t i, j, z;            // loop indices */

    try
    {
        /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
        Exception::dontPrint();
        /*
     * Select hyperslab for the dataset in the file, using 1x1 blocks,
     * (1,1) stride and (M,1) count starting at the position (0,0).
     */
        hsize_t start[3];  // Start of hyperslab
        hsize_t stride[3]; // Stride of hyperslab
        hsize_t count[3];  // Block count
        hsize_t block[3];  // Block sizes
        start[0] = 0;
        start[1] = 0;
        start[2] = 0;
        stride[0] = 1;
        stride[1] = 1;
        stride[2] = 1;
        count[0] = 1;
        count[1] = Count_Col;
        count[2] = 1;
        block[0] = 1;
        block[1] = 1;
        block[2] = 1;

        /*
            * Create dataspace for the dataset in memory.
        */
        hsize_t dim1[] = {1, Count_Col}; /* Dimension size of the dataset (in memory) */
        DataSpace mspace1(MSPACE1_RANK, dim1);

        /*
            * Select hyperslab.
            * We will use L elements of the vector buffer starting at the
            * zero element.  Selected elements are 0 2 3 . . . L-1
        */
        hsize_t startm[2];  // Start of hyperslab
        hsize_t stridem[2]; // Stride of hyperslab
        hsize_t countm[2];  // Block count
        hsize_t blockm[2];  // Block sizes
        startm[0] = 0;
        startm[1] = 0;
        stridem[0] = 1;
        stridem[1] = 1;
        countm[0] = 1;
        countm[1] = Count_Col;
        blockm[0] = 1;
        blockm[1] = 1;
        mspace1.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);

        if (dtype == "float")
        {
            /*
            * Write selection from the vector buffer to the dataset in the file. 
            */
            float buff_vector[Count_Col]; // vector buffer for dset
            start[2] = zth_frame;

            for (j = 0; j < Count_row; j++)
            {
                start[0] = j;
                start[1] = 0;

                fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
                /*
                * Buffer update.
                */
                for (i = 0; i < Count_Col; i++)
                    buff_vector[i] = (float) buffer_mat(j, i);

                dataset.write(buff_vector, PredType::IEEE_F32LE, mspace1, fspace);
            }
        }
        else if (dtype == "double")
        {
            /*
            * Write selection from the vector buffer to the dataset in the file. 
            */
            double buff_vector[Count_Col]; // vector buffer for dset
            start[2] = zth_frame;

            for (j = 0; j < Count_row; j++)
            {
                start[0] = j;
                start[1] = 0;

                fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
                /*
                * Buffer update.
                */
                for (i = 0; i < Count_Col; i++)
                    buff_vector[i] = buffer_mat(j, i);

                dataset.write(buff_vector, PredType::IEEE_F64LE, mspace1, fspace);
            }
        }
        else
        {
            it_error("PredType error: write for Mapping ");
        }

    } // end of try lock

    // catch failure caused by the H5File operations
    catch (FileIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
    }
    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
    }
};
