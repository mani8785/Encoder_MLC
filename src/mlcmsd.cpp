#include <sstream>
#include "mlcmsd.h"

// void MLCMSD::set_total_num_levels(int NoLs_val)
// {
//     /*
//         Set the total number of levels
//     */
//     NoLs = NoLs_val;
// };

void MLCMSD::set_NoLiU(int NoLiU_val)
{
    /*
        Set the number of levels used encoder for MLC-MSD
    */
    NoLiU = NoLiU_val;
};

void MLCMSD::set_CFL(int CFL_val)
{
    /*
        Set the Common Frame Length (CFL) for all the codes.
            It is actually the read size of our buffer to read the file from h5
            It should be equal to the codeword length
    */
    CFL = CFL_val;
};

void MLCMSD::set_Rate(double Rate_val, int level_no)
{
    /*
        Set the code rate and check if the parity file exists in the folder of the bank of matrices
        Input: 
            Rate value a double 
            level_no: the level number it can be 1, 2 or 3

        Task:
            call set_ldpc for a given rate and level (see set_ldpc)
            It search in the folder "/home/hosma/Documents/VSCODE/Data/LDPC/"; 
            tries to find parity files like "H_1000000_0.92.it", "H_1000000_0.92.peg";
            standard name format: H_CFL_Rate.it or H_CFL_Rate.peg
    */

    stringstream streamR;
    streamR << fixed << setprecision(2) << Rate_val;
    string code_name = "H_"+to_str(CFL)+"_"+streamR.str(); // just name without .it or .peg

    if (level_no > 3 || level_no < 1)
    {
        it_error("Level_no can takes values in {1, 2, 3}");
    }

    switch (level_no)
    {
    case 0:
        it_error("level index starts from 1.");
        break;

    case 1:
        R1 = Rate_val;
        if (exists_test(code_name, ".it"))
        {
            set_ldpc(1, Hbank_path+code_name+".it");
        } 
        else if (exists_test(code_name, ".peg"))
        {
            set_ldpc(1, Hbank_path+code_name+".peg");
        } 
        else
        {
            it_error("The parity does not exists for level 1");
        }
        
        break;

    case 2:
        R2 = Rate_val;
        if (exists_test(code_name, ".it"))
        {
            set_ldpc(2, Hbank_path+code_name+".it");
        } 
        else if (exists_test(code_name, ".peg"))
        {
            set_ldpc(2, Hbank_path+code_name+".peg");
        } 
        else
        {
            it_error("The parity does not exists for level 2");
        }
        break;

    case 3:
        R3 = Rate_val;
        if (exists_test(code_name, ".it"))
        {
            set_ldpc(3, Hbank_path+code_name+".it");
        } 
        else if (exists_test(code_name, ".peg"))
        {
            set_ldpc(3, Hbank_path+code_name+".peg");
        } 
        else
        {
            it_error("The parity does not exists for level 3");
        }
        break;

    default:
        break;
    }
};

void MLCMSD::set_ldpc(int level_no, string ldpc_path)
{
    /*
        Set the ldpc code to the level i
        Input: 
            level_no: 1, 2 or 3
            path to the parity_matrix
        
        PS: only support .it or .peg file

        No output: the privte variables (LDPC classes) are set now
    */
    if (level_no > 3 || level_no < 1)
    {
        it_error("Level_no can takes values in {1, 2, 3}");
    }

    switch (level_no)
    {
    case 0:
        it_error("level index starts from 1.");
        break;

    case 1:
        C1 = fill_ldpc(ldpc_path);
        break;

    case 2:
        C2 = fill_ldpc(ldpc_path);
        break;

    case 3:
        C3 = fill_ldpc(ldpc_path);
        break;

    default:
        break;
    }
};

bool MLCMSD::exists_test (const string& name, string type_h)
{
    /*
        Check if an specific file with given format exists in a default folder.
    */
    if (FILE *file = fopen((Hbank_path+name+type_h).c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}



void MLCMSD::load_env(string filetxt)
{
    /*
        This method load a txt file.
        It contains:
        CFL, NoLs, NoLiU: 
        R1, R2, R3,
    */
    hsize_t CFL, NoLs, NoLiU;
    double R1 = 1.0, R2 = 1.0, R3 = 1.0;

    string line, elements, temp_val; // The strings required to read the file.peg
    int row = 0;                     // counter to fill the base_matrix
    ifstream _file(filetxt);
    if (!_file.is_open())
    {
        cerr << "File can not be find or opened";
        exit(1);
    }

    /*
        Read the txt file and extract the information
    */
    for (string line; getline(_file, line);)
    { // read line by line and process the .txt file
        // some variables to define type of the lines
        size_t found_command = line.find("/*");   // found the command lines
        size_t found_command_2 = line.find("//"); // found the command lines
        size_t found_two_space = line.find("  ");

        if (found_command != string::npos)
            continue; // skip this line
        if (found_command_2 != string::npos)
            continue;     // skip this line( ignore if the line is command )
        if (line.empty()) // skip this line (ignore if the line is empty)
            continue;
        if (found_two_space == 0)
            continue; // redundant info is here
        else
        {
            istringstream streamL(line);
            switch (row)
            {
            case 0:
                for (int i = 0; streamL >> elements; i++)
                {
                    //	  cout << elements << endl; // This is to see how the for loop works ;)
                    if (i == 0)
                    {
                        CFL = atoi(elements.c_str()); //  // cout << *ROW << endl;
                    }
                    else if (i == 1)
                    {
                        NoLs = atoi(elements.c_str());
                    }
                    else
                    {
                        NoLiU = atoi(elements.c_str());
                    };
                }; // end for
                row++;
                break;

            case 1:
            for (int i = 0; streamL >> elements; i++)
                {
                    //	  cout << elements << endl; // This is to see how the for loop works ;)
                    if (i == 0)
                    {
                        R1 = atof(elements.c_str()); //  // cout << *ROW << endl;
                    }
                    else if (i == 1)
                    {
                        R2 = atof(elements.c_str());
                    }
                    else
                    {
                        R3 = atof(elements.c_str());
                    };
                }; // end for
                break;

            default:
                break;
            }

        } // end else
    }     // end of lines

    /* 
        Now update the structure 
    */
    set_NoLiU(NoLiU);
    set_CFL(CFL);
    
    /*
        Set the rates and LDPC parity to each level
    */
    switch (NoLiU)
    {
    case 1:
        set_Rate(R1, 1);
        break;

    case 2:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        break;

    case 3:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        set_Rate(R3, 3);
        break;

    default:
        break;
    }
};

void MLCMSD::check_env()
{
    /* this will check the structure of the environment.*/

    if (NoLiU == 1)
    {
        if (this->get_ldpc(1).get_nvar() == CFL)
        {
            printf("\t # The MSB level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(1).get_rate());
        }
        else
        {
            it_error("The frame lengths is not equal to codeword length");
        }
        
    }
    else if (NoLiU == 2)
    {
        if ((this->get_ldpc(1).get_nvar() == this->get_ldpc(2).get_nvar()) && ((this->get_ldpc(1).get_nvar() == CFL)))
        {
            printf("\t # The 1st level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(1).get_rate());
            printf("\t # The 2nd level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(2).get_rate());
        }
        else
        {
            it_error("The frame lengths are not equal");
        }
    }
    else
    {
        if ((this->get_ldpc(1).get_nvar() == this->get_ldpc(2).get_nvar()) && (this->get_ldpc(3).get_nvar() == this->get_ldpc(1).get_nvar()) && (this->get_ldpc(3).get_nvar() == CFL))
        {
            printf("\t # The 1st level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(1).get_rate());
            printf("\t # The 2nd level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(2).get_rate());
            printf("\t # The 3rd level with ldpc code of length %lld and rate %4.2f\n", CFL, this->get_ldpc(3).get_rate());
            
        }
        else
        {
            it_error("The frame lengths are not equal");
        }
    }


};




void MLCMSD::Rare_data_prepration(string path_to_inputH5, bool multiple_group_flg, hsize_t &TNoEs_val, hsize_t &TFN_val)
{
    /*
        Structure of the input h5:
            two datasets with names RxI and RxQ
            Integer
    */
    string DsetNames[2] = {"RxI", "RxQ"};
    // bool multiple_group_flg = true; // set it true if the file contains multiple 1D group names
    if (multiple_group_flg)
    {
        string DsetNames_multi[2] = {"RxI0", "RxQ0"};
        combine_datasets(path_to_inputH5, DsetNames_multi, 40, "temp1D.h5", DsetNames);
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

    TNoEs_val = TNoEs;
    TFN_val = TFN;

};

void MLCMSD::encoding_main(string path_to_inputH5, string path_to_outptH5, hsize_t TNoEs, hsize_t TFN)
{
    /*
        We prepare the input data in our desired shape
        The h5 has the dataset names RxI va RxQ
    */
    string DsetNames[2] = {"RxI", "RxQ"};
    hsize_t Dims[2];    // Dimension of the dataset
    int rankI = 1;      // determines 1D or 2D
    string dtypeIn;     // Valid data type are : int8, int16, float, double
    get_dataset_info(path_to_inputH5, DsetNames[0], rankI, Dims, dtypeIn, false);
    /*
        Buffers to read the data from H5 file
    */
    ivec Rxi, RxIi, RxQi; 
    hsize_t HCFL;
    HCFL = CFL/2;


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
                |______________________ PLAINTXT,    Rank = 2,   (frame_number, nvar),     dtype unsigned int
   */
    printf("# ------------------------------------------------------\n");
    printf("# -------------- %8s", "MLC-MSD Encoding \n");
    printf("# ------------------------------------------------------\n");

    H5File file(path_to_outptH5, H5F_ACC_TRUNC);
    DataSpace fspace_S0;
    DataSet dataset_S0;
    string Dsname_S0 = "SYND0";
    // hsize_t nrow_synd0 = (hsize_t) ldpc1.get_ncheck();
    hsize_t nrow_synd0 = (hsize_t) this->C1.get_ncheck();
    hsize_t Dims_ds_S0[2] = {TFN, nrow_synd0};

    DataSpace fspace_S1;
    DataSet dataset_S1;
    string Dsname_S1 = "SYND1";
    hsize_t nrow_synd1 = (hsize_t) this->C2.get_ncheck(); // ldpc2.get_ncheck();
    hsize_t Dims_ds_S1[2] = {TFN, nrow_synd1};

    DataSpace fspace_S2;
    DataSet dataset_S2;
    string Dsname_S2 = "SYND2";
    hsize_t nrow_synd2 = (hsize_t) this->C3.get_ncheck(); // ldpc3.get_ncheck();
    hsize_t Dims_ds_S2[2] = {TFN, nrow_synd2};

    DataSpace fspace_plaintext;
    DataSet dataset_plaintext;
    string Dsname_plaintext = "PLAINTXT";
    hsize_t Dims_ds_plaintext[2] = {TFN, CFL};
    
    switch (NoLiU)
    {
    case 1:
        cout << "# * number of levels in use are: \t" << 1 << endl;
        // mlc_env.display_level(&info_l1, true); // short info list is true
        this->display_level(1, true);

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
        // mlc_env.display_level(&info_l1, true); // short info list is true
        // mlc_env.display_level(&info_l2, true);
        this->display_level(1, true);
        this->display_level(2, true);

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
        // mlc_env.display_level(&info_l1, true); // short info list is true
        // mlc_env.display_level(&info_l2, true);
        // mlc_env.display_level(&info_l3, true);
        this->display_level(1, true);
        this->display_level(2, true);
        this->display_level(3, true);
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
            Convert data to unsigned int
        */
        Rxi+=32;
   
        // ------------------------ Digitization
        cout << endl;
        for (int cc = 0; cc < CFL; cc++)
        {   
            Rxi_bin.set_row( cc, dec2bin(NoLs, Rxi(cc)) );
        }

        bin_b_level_1 = Rxi_bin.get_col(0); // just for test
        bin_b_level_2 = Rxi_bin.get_col(1);
        bin_b_level_3 = Rxi_bin.get_col(2);

        if (NoLiU == 1)
        {
            // ----------------------- MLC-MSD one level
            bvec encded_data;
            // encded_data.set_length(info_l1.pl, false);
            encded_data.set_length(nrow_synd0, false);
            bmat plain_texts;
            ivec plain_texts_decim;
            plain_texts.set_size(CFL, NoLs - 1, false);
            plain_texts_decim.set_size(CFL, false);
            // mlc_env.encoder_one_level(Rxi_bin, info_l1, plain_texts, plain_texts_decim, encded_data);
            this->encoder_one_level(Rxi_bin, plain_texts, plain_texts_decim, encded_data);

            write2D_to_dataset_row_j(dataset_S0, fspace_S0, "bool", nrow_synd0, ifc, encded_data);
            write2D_to_dataset_row_j(dataset_plaintext, fspace_plaintext, "int8", CFL, ifc, plain_texts_decim);

            // mlc_env.write_to_2D_dataset_ith_index_mapping(dataset_plaintext, fspace_plaintext, "boolean", (hsize_t) CFL, (hsize_t) (NoLs-1), ifc, plain_texts);
        }
        else if (NoLiU == 2)
        {
            // ----------------------- MLC-MSD two levels
            bvec enc_data_1, enc_data_2;
            // enc_data_1.set_length(info_l1.pl, false);
            // enc_data_2.set_length(info_l2.pl, false);
            enc_data_1.set_length(nrow_synd0, false);
            enc_data_2.set_length(nrow_synd1, false);

            bmat plain_texts_new;
            plain_texts_new.set_size(CFL, NoLs - 2, false);

            ivec plain_texts_decim;
            plain_texts_decim.set_size(CFL, false);
            // mlc_env.encoder_two_levels(Rxi_bin, info_l1, info_l2, plain_texts_new, plain_texts_decim, enc_data_1, enc_data_2);
            this->encoder_two_levels(Rxi_bin, plain_texts_new, plain_texts_decim, enc_data_1, enc_data_2);

            write2D_to_dataset_row_j(dataset_S0, fspace_S0, "bool", nrow_synd0, ifc, enc_data_1);
            write2D_to_dataset_row_j(dataset_S1, fspace_S1, "bool", nrow_synd1, ifc, enc_data_2);

            write2D_to_dataset_row_j(dataset_plaintext, fspace_plaintext, "int8", CFL, ifc, plain_texts_decim);
            // mlc_env.write_to_2D_dataset_ith_index_mapping(dataset_plaintext, fspace_plaintext, "boolean", (hsize_t) CFL, (hsize_t) (NoLs-2), ifc, plain_texts_new);
        }
        else if (NoLiU == 3)
        {
            // ----------------------- MLC-MSD three levels
            bvec enc_data_1, enc_data_2, enc_data_3;
            enc_data_1.set_length(nrow_synd0, false);
            enc_data_2.set_length(nrow_synd1, false);
            enc_data_3.set_length(nrow_synd2, false);
            // enc_data_1.set_length(info_l1.pl, false);
            // enc_data_2.set_length(info_l2.pl, false);
            // enc_data_3.set_length(info_l3.pl, false);
            bmat plain_texts_2_to_0;
            plain_texts_2_to_0.set_size(CFL, NoLs - 3, false);
            
            ivec plain_texts_decim;
            plain_texts_decim.set_size(CFL, false);
            // mlc_env.encoder_three_levels(&Rxi_bin, &info_l1, &info_l2, &info_l3, &plain_texts_2_to_0, plain_texts_decim, &enc_data_1, &enc_data_2, &enc_data_3);
            this->encoder_three_levels(&Rxi_bin, &plain_texts_2_to_0, plain_texts_decim, &enc_data_1, &enc_data_2, &enc_data_3);


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
};

double MLCMSD::get_Rate(int level_no)
{   
    /*
        Return the rate of a givel level
    */
    if (level_no > 3 || level_no < 1)
    {
        it_error("Level_no can takes values in {1, 2, 3}");
        return -1.0;
    }

    switch (level_no)
    {
    case 0:
        it_error("level index starts from 1.");
        break;

    case 1:
        return R1;
        break;

    case 2:
        return R2;
        break;

    case 3:
        return R3;
        break;

    default:
        break;
    }
    return -1.0;
};

LDPC_Code MLCMSD::get_ldpc(int level_no)
{
    /*
        Return the LDPC code of a given level
    */
    if (level_no > 3 || level_no < 1)
    {
        it_error("Level_no can takes values in {1, 2, 3}");
        return C1;
    }

    switch (level_no)
    {
    case 0:
        it_error("level index starts from 1.");
        break;

    case 1:
        return C1;
        break;

    case 2:
        return C2;
        break;

    case 3:
        return C3;
        break;

    default:
        break;
    }
    return C1;
};



int MLCMSD::check_the_encoder_file_format(string code_name)
{
    /* Check the type of the codec input file
     0: *.it
     1: *.qc // the standard for AFF3CT
     2: *.peg
     3: skip
     4:
  */
    size_t found_type = code_name.find(".it");
    if (found_type != std::string::npos)
        return 0;
    found_type = code_name.find(".qc");
    if (found_type != std::string::npos)
        return 1;
    found_type = code_name.find(".peg");
    if (found_type != std::string::npos)
        return 2;
    found_type = code_name.find("skip");
    if (found_type != std::string::npos)
        return 3;
    else
        return 4;
};

void MLCMSD::load_peg(string input_peg, int *ROW, int *COL, int *Z_size, imat *base_matrix)
{
    /*
    This method load a peg file and generate a base matrix, 
    The peg file is for a BLDPC code, and contains N, M, Z
    then for each row the colums number of non zero matrices followed by rotation value
    
  */

    imat base_mat; // The base matrix for the LDPC parity. Later we can expand with the Z size

    string line, elements, temp_val; // The strings required to read the file.peg
    int row = 0;                     // counter to fill the base_matrix

    size_t pos = 0;          // demonstrate the positon using the find method
    string token;            // string to store portion of the line
    string delimiter = ", "; // the splitter for getting the col,z paitrs
    size_t pos_rv = 0;       // the position to take each term from token
    string r_v;              // the string to store the left number a,b : r_v = a
    string delim = ",";      // the seconf delimiter to get number from pair a,b

    ifstream _file(input_peg);
    if (!_file.is_open())
    {
        cerr << "File can not be find or opened";
        exit(1);
    }

    for (string line; getline(_file, line);)
    { // read line by line and process the .peg file
        // some variables to define type of the lines
        size_t found_command = line.find("/*");   // found the command lines
        size_t found_command_2 = line.find("//"); // found the command lines
        size_t found_u = line.find("unsigned");   // found the unsigned
        size_t found_end = line.find("}");        // found the end of file
        size_t found_two_space = line.find("  ");

        if (found_command != string::npos)
            continue; // skip this line
        if (found_command_2 != string::npos)
            continue; // skip this line( ignore if the line is command )
        if (found_end != string::npos)
            continue;     // skip this line (ignore the end line)
        if (line.empty()) // skip this line (ignore if the line is empty)
            continue;
        if (found_u != string::npos) // get matrix structure from this line
        {
            istringstream streamL(line);
            for (int i = 0; streamL >> elements; i++)
            {
                //	  cout << elements << endl; // This is to see how the for loop works ;)
                if (i == 5)
                {
                    *COL = atoi(elements.c_str()); //  // cout << *ROW << endl;
                }
                else if (i == 6)
                {
                    *ROW = atoi(elements.c_str());
                }
                else
                {
                    *Z_size = atoi(elements.c_str());
                };
            }; // end for
            continue;
        } // end for found_unsigned_short
        if (found_two_space == 0)
            continue; // redundant info is here
        else
        {
            base_mat.set_size(*ROW, *COL, false); // create the base_matrix
            for (int col = 0; col < *COL; col++)
                base_mat(row, col) = -1; // fill each row of the matrix with -1

            //   DEBUG: See the line : cout << row << "is  "<<  line << endl;
            pos = 0;
            pos_rv = 0;
            while ((pos = line.find(delimiter)) != std::string::npos)
            { // split for ", " token = a,b
                token = line.substr(0, pos);
                pos_rv = 0;
                while ((pos_rv = token.find(delim)) != std::string::npos) // split for "," r_v = a
                {
                    r_v = token.substr(0, pos_rv);                          // r_v = a
                    token.erase(0, pos_rv + delim.length());                // token = b
                    base_mat(row, atoi(r_v.c_str())) = atoi(token.c_str()); // base_mat(row, a) = b
                                                                            //  cout << "row = "<<  row << ", col = "<< r_v << ", Z = " << atoi(token.c_str()) << endl;
                }
                //	cout << token << std::endl;
                line.erase(0, pos + delimiter.length()); // delete a,b from the begining of the line
            }
            //      cout << line << endl; // last token, erease line untill last token
            // Repeat the process for the last token in line     // a,b, ToDo just one time is enough no need for while loop

            pos_rv = line.find(delim);                             // line = c,d, : the last pair in the line. pos_rv = find the first position of ","
            r_v = line.substr(0, pos_rv);                          // r_v = c
            line.erase(0, pos_rv + delim.length());                // line = d,
            base_mat(row, atoi(r_v.c_str())) = atoi(line.c_str()); // base_mat(row, c) = d
            //      cout << "row = "<<  row << ", col = "<< r_v << ", Z = " << atoi(line.c_str()) << endl;
            row++; // next rwo of the matrix
        }          // end else
    }              // end of lines

    //  cout << base_mat <<endl;
    *base_matrix = base_mat;
}

LDPC_Code MLCMSD::fill_ldpc(string code_name)
{
    LDPC_Code C; // empty constructor
    int check_input_type = check_the_encoder_file_format(code_name);

    if (check_input_type == 0)
    {
        C.load_code(code_name); // load the .it file
        cout << "\e[1m"
             << "=================== Load from .it file  ==============="
             << "\e[0m" << endl;
    }
    else if (check_input_type == 1)
    {
        cout << "\e[1m"
             << "=================== Do not support  .qc file  ==============="
             << "\e[0m" << endl;
    }
    else if (check_input_type == 2)
    {
        cout << "\e[1m"
             << "=================== Load from .peg file for BLDPC  ==============="
             << "\e[0m" << endl;
        int NN_size, MM_size, ZZ_size;
        imat base_matrix;
        load_peg(code_name, &NN_size, &MM_size, &ZZ_size, &base_matrix); // argv[1]
        BLDPC_Parity H;
        H.expand_base(base_matrix, ZZ_size);
        C.set_code(&H);
        // DEBUG: cout << base_matrix << endl;
    }
    else if (check_input_type == 3)
    {
        cout << "\e[1m"
             << "=================== No decoding will be done in this level   ==============="
             << "\e[0m" << endl;
    }
    else if (check_input_type == 4)
        cout << "\e[1m"
             << "=================== Do not support  ==============="
             << "\e[0m" << endl;

    return C;
}



void MLCMSD::display_level(int level_no, bool short_info)
{
    if (short_info)
    {
        printf("# * Code info: \n");
        printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8.3f\n# \t ** %8s\t= %8d\n",
               "Info. bits (K)", this->get_ldpc(level_no).get_ninfo(),
               "Codeword size (N)", this->get_ldpc(level_no).get_nvar(),
               "Code Rate (R)", this->get_ldpc(level_no).get_rate(),
               "level number", level_no);
    }
    else
    {
        printf("# \e[1m Parameters: \e[0m \n");
        printf("# * Code info: \n");
        printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8.3f\n",
               "Info. bits (K)", this->get_ldpc(level_no).get_ninfo(),
               "Codeword size (N)", this->get_ldpc(level_no).get_nvar(),
               "Code Rate (R)", this->get_ldpc(level_no).get_rate());
    }
}

void MLCMSD::display_table_title()
{
    printf("\e[1m");
    printf("# %8s | %8s || %8s | %8s | %8s | %8s | %8s ||\n",
           "--------", "--------", "--------", "--------", "--------", "--------", "--------");
    printf("# %8s | %8s || %8s | %8s | %8s | %8s | %8s ||\n",
           " level ", "Code", "FRA", "FE", "BE", "BER", "FER");
    printf("# %8s | %8s || %8s | %8s | %8s | %8s | %8s ||\n",
           "no.", "Rate", "", "", "", "", "");
    printf("# %8s | %8s || %8s | %8s | %8s | %8s | %8s ||\n",
           "--------", "--------", "--------", "--------", "--------", "--------", "--------");
    printf("\e[0m");
}




void MLCMSD::encoder_one_level(const bmat &qxB_bin, bmat &plain_texts, ivec &plain_texts_decim, bvec &enc_data_hard)
{
    bvec bin_xB_5 = qxB_bin.get_col(0);        // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1; // ToDo check if QLLR is required
    if (this->get_ldpc(1).get_ncheck() > 0)
    {
        QLLRvec synd_llr_xB_5 = this->get_ldpc(1).soft_syndrome_check(sign_xB_5);
        enc_data_hard = synd_llr_xB_5 > 0;
    }
    plain_texts = qxB_bin.get(0, CFL - 1, 1, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts.get_row(i)));
    }    
    
}

void MLCMSD::encoder_two_levels(const bmat &qxB_bin, bmat &plain_texts_two_levels, ivec &plain_texts_decim, bvec &enc_data_hard_1, bvec &enc_data_hard_2)
{
    bvec bin_xB_5 = qxB_bin.get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    bvec bin_xB_4 = qxB_bin.get_col(1);
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    ivec sign_xB_4 = 2 * to_ivec(bin_xB_4) - 1;

    if (this->get_ldpc(1).get_ncheck() > 0)
    {
        QLLRvec synd_llr_xB_5 = this->get_ldpc(1).soft_syndrome_check(sign_xB_5);
        enc_data_hard_1 = synd_llr_xB_5 > 0;
    }
    if (this->get_ldpc(2).get_ncheck() > 0)
    {
        QLLRvec synd_llr_xB_4 = this->get_ldpc(2).soft_syndrome_check(sign_xB_4);
        enc_data_hard_2 = synd_llr_xB_4 > 0;
    }

    plain_texts_two_levels = qxB_bin.get(0, CFL - 1, 2, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts_two_levels.get_row(i)));
    }
}

void MLCMSD::encoder_three_levels(const bmat *qxB_bin, bmat *plain_texts_LSBs, ivec &plain_texts_decim, bvec *enc_data_hard_1, bvec *enc_data_hard_2, bvec *enc_data_hard_3)
{
    bvec bin_xB_5 = qxB_bin->get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    bvec bin_xB_4 = qxB_bin->get_col(1);
    bvec bin_xB_3 = qxB_bin->get_col(2);
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    ivec sign_xB_4 = 2 * to_ivec(bin_xB_4) - 1;
    ivec sign_xB_3 = 2 * to_ivec(bin_xB_3) - 1;
    if (this->get_ldpc(1).get_ncheck()  > 0)
    {
        QLLRvec synd_llr_xB_5 = this->get_ldpc(1).soft_syndrome_check(sign_xB_5);
        *enc_data_hard_1 = synd_llr_xB_5 > 0;
    }
    if (this->get_ldpc(2).get_ncheck()  > 0)
    {
        QLLRvec synd_llr_xB_4 = this->get_ldpc(2).soft_syndrome_check(sign_xB_4);
        *enc_data_hard_2 = synd_llr_xB_4 > 0;
    }

    if (this->get_ldpc(3).get_ncheck()  > 0)
    {
        QLLRvec synd_llr_xB_3 = this->get_ldpc(3).soft_syndrome_check(sign_xB_3);
        *enc_data_hard_3 = synd_llr_xB_3 > 0;
    }
    *plain_texts_LSBs = qxB_bin->get(0, CFL - 1, 3, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts_LSBs->get_row(i)));
    }
}


/*
    May be redundant functions
    load_env(string filetxt, hsize_t &CFL, hsize_t &NoLs, hsize_t &NoLiU, double &R1, double &R2, double &R3)
    initialize_struct(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name)
    update_level_info(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name)
    void MLCMSD::check_structure(const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3)
*/

void MLCMSD::load_env(string filetxt, hsize_t &CFL, hsize_t &NoLs, hsize_t &NoLiU, double &R1, double &R2, double &R3)
{
    /*
    OLD METHOD OF LOAD ENV
    This method load a txt file.
    It contains:
    CFL, NoLs, NoLiU: 
    R1, R2, R3,
  */
    string line, elements, temp_val; // The strings required to read the file.peg
    int row = 0;                     // counter to fill the base_matrix
    ifstream _file(filetxt);
    if (!_file.is_open())
    {
        cerr << "File can not be find or opened";
        exit(1);
    }

    for (string line; getline(_file, line);)
    { // read line by line and process the .txt file
        // some variables to define type of the lines
        size_t found_command = line.find("/*");   // found the command lines
        size_t found_command_2 = line.find("//"); // found the command lines
        size_t found_two_space = line.find("  ");

        if (found_command != string::npos)
            continue; // skip this line
        if (found_command_2 != string::npos)
            continue;     // skip this line( ignore if the line is command )
        if (line.empty()) // skip this line (ignore if the line is empty)
            continue;
        if (found_two_space == 0)
            continue; // redundant info is here
        else
        {
            istringstream streamL(line);
            switch (row)
            {
            case 0:
                for (int i = 0; streamL >> elements; i++)
                {
                    //	  cout << elements << endl; // This is to see how the for loop works ;)
                    if (i == 0)
                    {
                        CFL = atoi(elements.c_str()); //  // cout << *ROW << endl;
                    }
                    else if (i == 1)
                    {
                        NoLs = atoi(elements.c_str());
                    }
                    else
                    {
                        NoLiU = atoi(elements.c_str());
                    };
                }; // end for
                row++;
                break;

            case 1:
            for (int i = 0; streamL >> elements; i++)
                {
                    //	  cout << elements << endl; // This is to see how the for loop works ;)
                    if (i == 0)
                    {
                        R1 = atof(elements.c_str()); //  // cout << *ROW << endl;
                    }
                    else if (i == 1)
                    {
                        R2 = atof(elements.c_str());
                    }
                    else
                    {
                        R3 = atof(elements.c_str());
                    };
                }; // end for
                break;

            default:
                break;
            }

        } // end else
    }     // end of lines

    /* Now update the structure into the c++*/
    set_NoLiU(NoLiU);
    // set_total_num_levels(NoLs);
    set_CFL(CFL);
    

    switch (NoLiU)
    {
    case 1:
        set_Rate(R1, 1);
        break;

    case 2:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        break;

    case 3:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        set_Rate(R3, 3);
        break;

    default:
        break;
    }
};

void MLCMSD::initialize_struct(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name)
{
    size_t found_type = h_name.find("skip");
    if (found_type != std::string::npos)
    {
        info_level->my_ldpc = ldpc_in;
        info_level->fl = 0.0;
        info_level->pl = 0.0;
        info_level->kl = 0.0;
        info_level->peg_file_name = h_name;
        info_level->level_no = level_no;
        info_level->level_code_rate = 1.0;
    }
    else
    {
        info_level->my_ldpc = ldpc_in;
        info_level->fl = ldpc_in->get_nvar();
        info_level->pl = ldpc_in->get_ncheck();
        info_level->kl = info_level->fl - info_level->pl;
        info_level->peg_file_name = h_name;
        info_level->level_no = level_no;
        info_level->level_code_rate = ldpc_in->get_rate();
    }
}

void MLCMSD::update_level_info(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name)
{
    size_t found_type = h_name.find("skip");
    if (found_type != std::string::npos)
    {
        info_level->my_ldpc = ldpc_in;
        info_level->fl = this->get_CFL();
        info_level->pl = 0.0;
        info_level->kl = this->get_CFL();
        info_level->peg_file_name = h_name;
        info_level->level_no = level_no;
        info_level->level_code_rate = 1.0;
    }
};

void MLCMSD::check_structure(const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3)
{
    int nvar1 = info_level1->fl;
    int nvar2 = info_level2->fl;
    int nvar3 = info_level3->fl;

    int cfl = this->get_CFL();
    int lniu = this->get_NoLiU();

    if (lniu == 1)
    {
        this->set_CFL(nvar1);
    }
    else if (lniu == 2)
    {
        if ((nvar1 == nvar2) && ((nvar2 == cfl)))
        {
            cout << "Frame length are equal for all the levels" << endl;
        }
        else
        {
            it_error("The frame lengths are not equal");
        }
    }
    else
    {
        if ((nvar1 == nvar2) && (nvar3 == nvar1) && (nvar3 == cfl))
        {
            cout << "Frame length are equal for all the levels" << endl;
        }
        else
        {
            it_error("The frame lengths are not equal");
        }
    }
}

void MLCMSD::display_level(LEVEL_INFO *info_level, bool short_info)
{
    string file_name = info_level->peg_file_name;
    if (short_info)
    {
        printf("# * Code info: \n");
        printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8.3f\n# \t ** %8s\t= %8d\n",
               "Info. bits (K)", info_level->kl,
               "Codeword size (N)", info_level->fl,
               "Code Rate (R)", info_level->level_code_rate,
               "level number", info_level->level_no);
    }
    else
    {
        printf("# \e[1m Parameters: \e[0m \n");
        printf("# * Code info: \n");
        printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8.3f\n",
               "Info. bits (K)", info_level->kl,
               "Codeword size (N)", info_level->fl,
               "Code Rate (R)", info_level->level_code_rate);
        printf("# * Decoder info: \n");
        printf("# \t ** %8s\t= %8s\n# \t ** %8s\t= %-16s\n# \t ** %8s\t= %8d\n",
               "Decoding method ", "BP-LLR-SPA",
               "H matrix info", file_name.c_str(),
               "Maximum iteration", info_level->my_ldpc->get_nrof_iterations());

        printf("# * Channel info: \n");
        printf("# \t ** %8s\t= %8s\n# \t ** %8s\t= %8s\n# \t ** %8s\t= %8s\n",
               "Channel Type ", "BI-AWGN",
               "Noise Type   ", "Eb/N0 (dB)",
               "Estimated Noise  ", "Unknown");
    }
}

// ==================== Enc/Dec for one level
void MLCMSD::encoder_one_level(const bmat &qxB_bin, const LEVEL_INFO &info_level, bmat &plain_texts, ivec &plain_texts_decim, bvec &enc_data_hard)
{
    bvec bin_xB_5 = qxB_bin.get_col(0);        // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1; // ToDo check if QLLR is required
    if (info_level.pl > 0)
    {
        QLLRvec synd_llr_xB_5 = info_level.my_ldpc->soft_syndrome_check(sign_xB_5);
        enc_data_hard = synd_llr_xB_5 > 0;
    }
    plain_texts = qxB_bin.get(0, CFL - 1, 1, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts.get_row(i)));
    }
    
    
}
// ==================== Enc/Dec for Two levels
void MLCMSD::encoder_two_levels(const bmat &qxB_bin, const LEVEL_INFO &info_level1, const LEVEL_INFO &info_level2, bmat &plain_texts_two_levels, ivec &plain_texts_decim, bvec &enc_data_hard_1, bvec &enc_data_hard_2)
{
    bvec bin_xB_5 = qxB_bin.get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    bvec bin_xB_4 = qxB_bin.get_col(1);
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    ivec sign_xB_4 = 2 * to_ivec(bin_xB_4) - 1;

    if (info_level1.pl > 0)
    {
        QLLRvec synd_llr_xB_5 = info_level1.my_ldpc->soft_syndrome_check(sign_xB_5);
        enc_data_hard_1 = synd_llr_xB_5 > 0;
    }
    if (info_level2.pl > 0)
    {
        QLLRvec synd_llr_xB_4 = info_level2.my_ldpc->soft_syndrome_check(sign_xB_4);
        enc_data_hard_2 = synd_llr_xB_4 > 0;
    }

    plain_texts_two_levels = qxB_bin.get(0, CFL - 1, 2, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts_two_levels.get_row(i)));
    }
}

// ==================== Enc/Dec for Three levels

void MLCMSD::encoder_three_levels(const bmat *qxB_bin, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3, bmat *plain_texts_LSBs, ivec &plain_texts_decim, bvec *enc_data_hard_1, bvec *enc_data_hard_2, bvec *enc_data_hard_3)
{
    bvec bin_xB_5 = qxB_bin->get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    bvec bin_xB_4 = qxB_bin->get_col(1);
    bvec bin_xB_3 = qxB_bin->get_col(2);
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    ivec sign_xB_4 = 2 * to_ivec(bin_xB_4) - 1;
    ivec sign_xB_3 = 2 * to_ivec(bin_xB_3) - 1;
    if (info_level1->pl > 0)
    {
        QLLRvec synd_llr_xB_5 = info_level1->my_ldpc->soft_syndrome_check(sign_xB_5);
        *enc_data_hard_1 = synd_llr_xB_5 > 0;
    }
    if (info_level2->pl > 0)
    {
        QLLRvec synd_llr_xB_4 = info_level2->my_ldpc->soft_syndrome_check(sign_xB_4);
        *enc_data_hard_2 = synd_llr_xB_4 > 0;
    }

    if (info_level3->pl > 0)
    {
        QLLRvec synd_llr_xB_3 = info_level3->my_ldpc->soft_syndrome_check(sign_xB_3);
        *enc_data_hard_3 = synd_llr_xB_3 > 0;
    }
    *plain_texts_LSBs = qxB_bin->get(0, CFL - 1, 3, NoLs - 1);
    for (size_t i = 0; i < CFL; i++)
    {
        plain_texts_decim.set(i, bin2dec(plain_texts_LSBs->get_row(i)));
    }
}

