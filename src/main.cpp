/*
Author: Hosma
Company: DTU QPIT
Title: Reverse reconciliation, MAP decoder, Sign-Magnitude
Details: load real data and test the decoder.
*/
#include <iostream>
#include <fstream> // header to read file
#include <itpp/itcomm.h>
#include <itpp/srccode/vq.h>
#include <itpp/comm/ldpc.h>
#include <iomanip>
#include <itpp/comm/modulator.h>
#include "mlcmsd.h"
#include "RogWH5.h"
#include <cstdlib>

// ================================================ Main function
int main(int argc, char **argv)
{
    string path_to_parity1 = "/home/hosma/Documents/VSCODE/Data/LDPC/H_1000000_0.92.it";
    string path_to_parity2 = "/home/hosma/Documents/VSCODE/Data/LDPC/H_1000000_0.50.it"; // 0.88
    string path_to_parity3 = "/home/hosma/Documents/VSCODE/Data/LDPC/H_1000000_0.50.it";

    // string path_to_inputH5 = "/home/hosma/Documents/100-Largefiles/20201219-IntegerTxRxSymbols/B2B/int8TxSymbols.h5";
    string path_to_inputH5 = "/home/hosma/Documents/100-Largefiles/20210115-TxRxSymbols for EC/20km/Rxfile.h5";
    string path_to_outptH5 = "/home/hosma/Documents/100-Largefiles/EncDataMLCMSD.h5";
    /*
        To measure time of encoding
    */
    Real_Timer my_timer;
    my_timer.tic();
    double start_time = my_timer.get_time();
    
    /*
        Construct MLC environment
    */
    MLCMSD mlc_env{}; // This will construct default environment for MLC-MSD    
    
    LDPC_Code ldpc1;                        // empty constructor
    LDPC_Code ldpc2;                        // empty constructor
    LDPC_Code ldpc3;                        // empty constructor
    int max_iter_LDPC = 50;                 // maximum number of iteration for the LDPC decoder.


    hsize_t NoLs;                               /* Number of levels */
    hsize_t NoLiU;                              /* Number of levels in use */
    hsize_t CFL;                                /* Common frame length */

    double R1, R2, R3;                      /* Code rates values it might not be used*/
    mlc_env.load_env("struct.txt", CFL, NoLs, NoLiU, R1, R2, R3);

    switch (mlc_env.get_NoLiU())
    {
    case 1:
        /* Load the encoder for MSB level*/
        ldpc1 = mlc_env.fill_ldpc(path_to_parity1); 
        ldpc1.set_exit_conditions(max_iter_LDPC, false, false);

        break;
    
    case 2:
        /* Load the encoders for the first two levels*/
        ldpc1 = mlc_env.fill_ldpc(path_to_parity1); 
        ldpc1.set_exit_conditions(max_iter_LDPC, false, false);

        ldpc2 = mlc_env.fill_ldpc(path_to_parity2); 
        ldpc2.set_exit_conditions(max_iter_LDPC, false, false);

        break;
    
    case 3:
        /* Load the encoders for the first three levels*/
        ldpc1 = mlc_env.fill_ldpc(path_to_parity1); 
        ldpc1.set_exit_conditions(max_iter_LDPC, false, false);

        ldpc2 = mlc_env.fill_ldpc(path_to_parity2); 
        ldpc2.set_exit_conditions(max_iter_LDPC, false, false);

        ldpc3 = mlc_env.fill_ldpc(path_to_parity3); 
        ldpc3.set_exit_conditions(max_iter_LDPC, false, false);
        break;

    default:
        break;
    }

    // ------------------------------ Level 1
    
    LEVEL_INFO info_l1;
    mlc_env.initialize_struct(&info_l1, &ldpc1, 1, path_to_parity1);
    // ------------------------------ Level 2
    LEVEL_INFO info_l2;
    mlc_env.initialize_struct(&info_l2, &ldpc2, 2, path_to_parity1);
    // ------------------------------ Level 3
    LEVEL_INFO info_l3;
    mlc_env.initialize_struct(&info_l3, &ldpc3, 3, path_to_parity1);
    // ================================== Compatibility test
    mlc_env.update_level_info(&info_l1, &ldpc1, 1, path_to_parity1);
    mlc_env.update_level_info(&info_l2, &ldpc2, 2, path_to_parity1);
    mlc_env.update_level_info(&info_l3, &ldpc3, 3, path_to_parity1);
    mlc_env.check_structure(&info_l1, &info_l2, &info_l3);
     /*
        Structure of the input h5:
            two datasets with names RxI and RxQ
            Integer
    */
    string DsetNames[2] = {"RxI", "RxQ"};
    bool multiple_group_flg = true; // set it true if the file contains multiple 1D group names
    if (multiple_group_flg)
    {
        string DsetNames_multi[2] = {"RxI0", "RxQ0"};
        combine_datasets(path_to_inputH5, DsetNames_multi, 10, "temp1D.h5", DsetNames);
        path_to_inputH5 = "temp1D.h5";
    }
    /*
        File prepration
            1- TNoEs: Total number of elements
            2- CFL : Common frame length
            3- TFN : Total Frame number
            4- Convert 2D dataset to appropritae 2D        
    */
    hsize_t Dims[2];    // Dimension of the dataset
    int rankI = 1;      // determines 1D or 2D
    string dtypeIn;     // Valid data type are : int8, int16, float, double
    get_dataset_info(path_to_inputH5, DsetNames[0], rankI, Dims, dtypeIn, true);
    hsize_t TNoEs, TFN, HCFL;
    HCFL = CFL/2;
    /*
        Reshape input dataset to appropriate dimension
            1-If input is 1D then convert it to a 2D dataset
                  2-remove 1D dataset
            2-If input is 2D convert it to the appropriate 2D format
    */
    if (rankI == 1)
    {
        TNoEs = Dims[0];
        TFN = TNoEs / HCFL;
        reshape_h5_1D_to_2D(path_to_inputH5, DsetNames, 2, "tempInp2D.h5", HCFL);
        if (multiple_group_flg)
            system("rm temp1D.h5");
        
        path_to_inputH5 = "tempInp2D.h5";
        get_dataset_info(path_to_inputH5, DsetNames[0], rankI, Dims, dtypeIn, true);
    } else if (rankI == 2)
    {
        TNoEs = Dims[0] * Dims[1];
        TFN = TNoEs / HCFL;
        reshape_h5_2D_to_1D(path_to_inputH5, DsetNames, 2, "temp1D.h5");
        reshape_h5_1D_to_2D("temp1D.h5", DsetNames, 2, "tempInp2D.h5", HCFL);
        path_to_inputH5 = "tempInp2D.h5";
        get_dataset_info(path_to_inputH5, DsetNames[0], rankI, Dims, dtypeIn, true);
        system("rm temp1D.h5");
    }
    /*
        Buffers to read the data from H5 file
    */
    ivec Rxi, RxIi, RxQi; 
    Rxi.set_length(CFL, false);
    RxIi.set_length(HCFL, false);
    RxQi.set_length(HCFL, false);
    
    bmat Rxi_bin;
    Rxi_bin.set_size(CFL, NoLs, false);
    
    int ifc = 0;
    // Boolean
    // =========== Binary hard vectors boolian 0 or 1
    bvec bin_b_level_1, bin_b_level_2, bin_b_level_3;
    /*
        Create a *.h5 file to wtite the encoded data in it
            path_to_outptH5.h5
                |______________________ SYND0,       Rank = 2,   (frame_number, ncheck1),     dtype Hbool
                |______________________ SYND1,       Rank = 2,   (frame_number, ncheck2),     dtype Hbool
                |______________________ SYND2,       Rank = 2,   (frame_number, ncheck3),     dtype Hbool                                
                |
                |______________________ PLAINTXT,    Rank = 2,   (frame_number, nvar*(6-i))),     dtype Hbool
   */
    printf("# ------------------------------------------------------\n");
    printf("# -------------- %8s", "MLC-MSD Encoding \n");
    printf("# ------------------------------------------------------\n");

    H5File file(path_to_outptH5, H5F_ACC_TRUNC);
    DataSpace fspace_S0;
    DataSet dataset_S0;
    string Dsname_S0 = "SYND0";
    hsize_t nrow_synd0 = (hsize_t) ldpc1.get_ncheck();
    hsize_t Dims_ds_S0[2] = {TFN, nrow_synd0};

    DataSpace fspace_S1;
    DataSet dataset_S1;
    string Dsname_S1 = "SYND1";
    hsize_t nrow_synd1 = (hsize_t) ldpc2.get_ncheck();
    hsize_t Dims_ds_S1[2] = {TFN, nrow_synd1};

    DataSpace fspace_S2;
    DataSet dataset_S2;
    string Dsname_S2 = "SYND2";
    hsize_t nrow_synd2 = (hsize_t) ldpc3.get_ncheck();
    hsize_t Dims_ds_S2[2] = {TFN, nrow_synd2};

    DataSpace fspace_plaintext;
    DataSet dataset_plaintext;
    string Dsname_plaintext = "PLAINTXT";
    // hsize_t nrow_plaintext = (hsize_t) (NoLs-1) * (hsize_t) CFL;
    hsize_t Dims_ds_plaintext[2] = {TFN, CFL};
    
    switch (mlc_env.get_NoLiU())
    {
    case 1:
        cout << "# * number of levels in use are: \t" << 1 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true

        /*
            Add dataset for Synd0
        */
       add_dataset(file, Dsname_S0, "bool", 2, Dims_ds_S0, fspace_S0, dataset_S0);
        /*
            Add dataset for plaintexts
        */
       add_dataset(file, Dsname_plaintext, "bool", 2, Dims_ds_plaintext, fspace_plaintext, dataset_plaintext);
        break;
    
    case 2:
        cout << "# * number of levels in use are: \t" << 2 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true
        mlc_env.display_level(&info_l2, true);

        /*
            Add dataset for Synd0 and Synd1
        */
        add_dataset(file, Dsname_S0, "bool", 2, Dims_ds_S0, fspace_S0, dataset_S0);
        add_dataset(file, Dsname_S1, "bool", 2, Dims_ds_S1, fspace_S1, dataset_S1);

        /*
            Add dataset for plaintexts
        */
        // nrow_plaintext = (hsize_t) (NoLs-2) * (hsize_t) CFL;
        Dims_ds_plaintext[0] = TFN;
        Dims_ds_plaintext[1] = CFL; // nrow_plaintext;
    
        add_dataset(file, Dsname_plaintext, "bool", 2, Dims_ds_plaintext, fspace_plaintext, dataset_plaintext);
        break;
    
    case 3:
        cout << "# * number of levels in use are: \t" << 3 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true
        mlc_env.display_level(&info_l2, true);
        mlc_env.display_level(&info_l3, true);
        /*
            Add dataset for Synd0 and Synd1, Synd2
        */

        add_dataset(file, Dsname_S0, "bool", 2, Dims_ds_S0, fspace_S0, dataset_S0);
        add_dataset(file, Dsname_S1, "bool", 2, Dims_ds_S1, fspace_S1, dataset_S1);
        add_dataset(file, Dsname_S2, "bool", 2, Dims_ds_S2, fspace_S2, dataset_S2);
        /*
            Add dataset for plaintexts
        */
        // nrow_plaintext = (hsize_t) (NoLs-3) * (hsize_t) CFL;
        Dims_ds_plaintext[0] = TFN;
        Dims_ds_plaintext[1] = CFL; // nrow_plaintext;
    
        add_dataset(file, Dsname_plaintext, "bool", 2, Dims_ds_plaintext, fspace_plaintext, dataset_plaintext);
        break;
    
    default:
        break;
    }
    
    /*
        Start randomize
    */
    RNG_randomize();

    /*
        loop over the number of frames
            encode each frame and store it in output h5 file
            We need some buffers to store temporary results
            TxIi : data read from TxI at ith frame
            TxQi : data read from TxQ at ith frame
            Txi  : [TxIi TxQi] has length equal to nvar (cw_length)

            synd : a vetor to store generated syndromes
            MxBogU: a matrix (8, nvar) to store the mapping values for each frame
    */
    printf("\e[1m");
    printf("# %-58s \n", "================= Start Encoding  ... ");
    for (size_t ifc = 0; ifc < TFN; ifc++)
    {
        printf("\r");
        printf("# \t ** %-16s\t : %-16d", " The current frame ", (int)ifc+1);
        fflush(stdout);
        // cout << "current Frame is:" << ifc << endl;

        // mlc_env.read_subset_of_1D_dataset_h5(path_to_inputH5, "TxI0", Ctype, HCFL, ifc, RxIi);
        // mlc_env.read_subset_of_1D_dataset_h5(path_to_inputH5, "TxQ0", Ctype, HCFL, ifc, RxQi);
         if (rankI == 1)
        {
            read1D_subset(path_to_inputH5, "RxI", HCFL, ifc, RxIi);
            read1D_subset(path_to_inputH5, "RxQ", HCFL, ifc, RxQi);
        }
        else if (rankI == 2)
        {
            read2D_from_row_j(path_to_inputH5, "RxI", HCFL, ifc, RxIi);
            read2D_from_row_j(path_to_inputH5, "RxQ", HCFL, ifc, RxQi);
        }
        /*
            Combine the two vectors in a new vector of length 2*cwl
        */
        Rxi.set_subvector(0, RxIi);
        Rxi.set_subvector(HCFL, RxQi);
        /*
            Convert to binary unsigned int
        */
        // for (size_t i = 0; i < CFL; i++)
        // {
        //     if (Rxi(i) < 0)
        //     {
        //         Rxi(i) += 64;
        //     }                        
        // }
        // ------------------------ Digitization
        cout << endl;
        for (int cc = 0; cc < CFL; cc++)
        {   
            Rxi_bin.set_row( cc, dec2bin(NoLs, Rxi(cc)) );
        }

        bin_b_level_1 = Rxi_bin.get_col(0); // just for test
        bin_b_level_2 = Rxi_bin.get_col(1);
        bin_b_level_3 = Rxi_bin.get_col(2);

        if (mlc_env.get_NoLiU() == 1)
        {
            // ----------------------- MLC-MSD one level
            bvec encded_data;
            encded_data.set_length(info_l1.pl, false);
            bmat plain_texts;
            ivec plain_texts_decim;
            plain_texts.set_size(CFL, NoLs - 1, false);
            plain_texts_decim.set_size(CFL, false);
            mlc_env.encoder_one_level(Rxi_bin, info_l1, plain_texts, plain_texts_decim, encded_data);

            write2D_to_dataset_row_j(dataset_S0, fspace_S0, "bool", nrow_synd0, ifc, encded_data);
            write2D_to_dataset_row_j(dataset_plaintext, fspace_plaintext, "int8", CFL, ifc, plain_texts_decim);

            // mlc_env.write_to_2D_dataset_ith_index_mapping(dataset_plaintext, fspace_plaintext, "boolean", (hsize_t) CFL, (hsize_t) (NoLs-1), ifc, plain_texts);
        }
        else if (mlc_env.get_NoLiU() == 2)
        {
            // ----------------------- MLC-MSD two levels
            bvec enc_data_1, enc_data_2;
            enc_data_1.set_length(info_l1.pl, false);
            enc_data_2.set_length(info_l2.pl, false);
            bmat plain_texts_new;
            plain_texts_new.set_size(CFL, NoLs - 2, false);

            ivec plain_texts_decim;
            plain_texts_decim.set_size(CFL, false);
            mlc_env.encoder_two_levels(Rxi_bin, info_l1, info_l2, plain_texts_new, plain_texts_decim, enc_data_1, enc_data_2);

            write2D_to_dataset_row_j(dataset_S0, fspace_S0, "bool", nrow_synd0, ifc, enc_data_1);
            write2D_to_dataset_row_j(dataset_S1, fspace_S1, "bool", nrow_synd1, ifc, enc_data_2);

            write2D_to_dataset_row_j(dataset_plaintext, fspace_plaintext, "int8", CFL, ifc, plain_texts_decim);
            // mlc_env.write_to_2D_dataset_ith_index_mapping(dataset_plaintext, fspace_plaintext, "boolean", (hsize_t) CFL, (hsize_t) (NoLs-2), ifc, plain_texts_new);
        }
        else if (mlc_env.get_NoLiU() == 3)
        {
            // ----------------------- MLC-MSD three levels
            bvec enc_data_1, enc_data_2, enc_data_3;
            enc_data_1.set_length(info_l1.pl, false);
            enc_data_2.set_length(info_l2.pl, false);
            enc_data_3.set_length(info_l3.pl, false);
            bmat plain_texts_2_to_0;
            plain_texts_2_to_0.set_size(CFL, NoLs - 3, false);
            
            ivec plain_texts_decim;
            plain_texts_decim.set_size(CFL, false);
            mlc_env.encoder_three_levels(&Rxi_bin, &info_l1, &info_l2, &info_l3, &plain_texts_2_to_0, plain_texts_decim, &enc_data_1, &enc_data_2, &enc_data_3);


            write2D_to_dataset_row_j(dataset_S0, fspace_S0, "bool", nrow_synd0, ifc, enc_data_1);
            write2D_to_dataset_row_j(dataset_S1, fspace_S1, "bool", nrow_synd1, ifc, enc_data_2);
            write2D_to_dataset_row_j(dataset_S2, fspace_S2, "bool", nrow_synd2, ifc, enc_data_3);

            
            write2D_to_dataset_row_j(dataset_plaintext, fspace_plaintext, "int8", CFL, ifc, plain_texts_decim);


            // mlc_env.write_to_2D_dataset_ith_index_mapping(dataset_plaintext, fspace_plaintext, "boolean", (hsize_t) CFL, (hsize_t) (NoLs-2), ifc, plain_texts_2_to_0);
        }
        else
        {
            it_error("We do not support more than 3 levels");
        }

        /*
            Write to the output file
        */
       

    } // end of frames

    /*
     * Reset the selection for the file dataspace fid.
     */
    // fspace_1.selectNone();
    // fspace_2.selectNone();

    double elapse_time = my_timer.get_time()-start_time;
    int hh = (int) elapse_time/3600;
    int mm = (int) (elapse_time-hh*3600)/60;
    int ss = (int) elapse_time - hh*3600 - 60*mm;
    printf("\n");
    printf("# \t ** %-16s\t : %-3d hh:%3d mm:%3d ss\n", " Elapsed time ", hh, mm, ss);
    printf("# \t ** %-16s\t : %-8.4f \n", " Throughput (Kbps)", TNoEs/elapse_time/1024*8);
    printf("\n# END of Simulation\n");
    printf("\e[0m");
    return 0;
}

// ======================================================================== other methods

/*
    Encoder functions:
        I generated two new functions using it++ lirary.
        Encoder is actually  a matrix multiplication.
    Input :
        a vector of length n_var
    Output:
        syndrome : a binary vector of length n_check
    Task:
        For a given matrix H and input llr_in
        Step 1: hard_in = llr_in < 0;
        Step 2: synd = hard_in * H.T

*/

bool LDPC_Code::syndrome_check_hosma(const bvec &x, const bvec &synd_orig) const
{
    QLLRvec llr = 1 - 2 * to_ivec(x);
    return syndrome_check_hosma(llr, synd_orig);
};


bool LDPC_Code::syndrome_check_hosma(const QLLRvec &LLR, const bvec &synd_orig) const
{
    // Please note the IT++ convention that a sure zero corresponds to
    // LLR=+infinity
    int i, j, synd, vi;

    for (j = 0; j < ncheck; j++)
    {
        synd = synd_orig(j);
        int vind = j; // tracks j+i*ncheck
        for (i = 0; i < sumX2(j); i++)
        {
            vi = V(vind);
            if (LLR(vi) < 0)
            {
                synd++;
            }
            vind += ncheck;
        }
        if ((synd & 1) == 1)
        {
            return false; // codeword is invalid
        }
    }
    return true; // codeword is valid
};