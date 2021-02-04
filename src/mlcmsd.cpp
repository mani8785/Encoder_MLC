#include <sstream>
#include "mlcmsd.h"

void MLCMSD::set_total_num_levels(int NoLs_val)
{
    /*
        Set the total number of levels
    */
    NoLs = NoLs_val;
};

void MLCMSD::set_num_level_in_use(int NoLiU_val)
{
    /*
        Set the number of levels used encoder for MLC-MSD
    */
    NoLiU = NoLiU_val;
};

void MLCMSD::set_common_frame_length(int CFL_val)
{
    /*
        Set the Common Frame Length (CFL) for all the codes.
        It is actually the read size of our buffer
    */
    CFL = CFL_val;
};

void MLCMSD::set_enc_pattern(bvec Enc_pattern_val)
{
    /*
        Set the encoding pattern.
        111000
    */
    int total_num_levels = Enc_pattern_val.length();
    if (NoLs != total_num_levels)
    {
        printf("Error Hint: The given pattern length is %d but should be %d", total_num_levels, NoLs);
        it_error("The pattern length does not match");
    }

    ivec Enc_pattern_val_int = to_ivec(Enc_pattern_val);
    if (sum(Enc_pattern_val_int) != NoLiU)
    {
        printf("Error Hint: Number of levels in use are %d but should be %d", sum(Enc_pattern_val_int), NoLiU);
        it_error("The number of levels in use does not match");
    }
    int pattern_dec = bin2dec(Enc_pattern_val);
    if (pattern_dec != 32 && pattern_dec != 48 && pattern_dec != 56)
    {
        printf("Error Hint: It is not a valid pattern. Valid patterns are (111000, 110000, 100000) \n");
        it_error("Wrong valid pattern");
    }

    EncPattern = Enc_pattern_val;
};

void MLCMSD::set_Rate(double Rate_val, int level_no)
{
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
        break;

    case 2:
        R2 = Rate_val;
        break;

    case 3:
        R3 = Rate_val;
        break;

    default:
        break;
    }
};

void MLCMSD::load_env(string filetxt, hsize_t &CFL, hsize_t &NoLs, hsize_t &NoLiU, double &R1, double &R2, double &R3)
{
    /*
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
    set_num_level_in_use(NoLiU);
    set_total_num_levels(NoLs);
    set_common_frame_length(CFL);
    

    switch (NoLiU)
    {
    case 1:
        set_Rate(R1, 1);
        set_enc_pattern("1 0 0 0 0 0");
        break;

    case 2:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        set_enc_pattern("1 1 0 0 0 0");
        break;

    case 3:
        set_Rate(R1, 1);
        set_Rate(R2, 2);
        set_Rate(R3, 3);
        set_enc_pattern("1 1 1 0 0 0");
        break;

    default:
        break;
    }
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
        info_level->fl = this->get_common_frame_length();
        info_level->pl = 0.0;
        info_level->kl = this->get_common_frame_length();
        info_level->peg_file_name = h_name;
        info_level->level_no = level_no;
        info_level->level_code_rate = 1.0;
    }
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

void MLCMSD::load_test_data(string data_name, int *len_data, vec *XA, vec *XB)
{
    // ======================================================================= // load data file
    // Declare the it_file class
    it_file af;
    // Open the file "it_file_test.it" for reading
    af.open(data_name);
    // Read the variable a from the file. Put result in vector a.

    af >> Name("XAI") >> *XA;
    af >> Name("XBI") >> *XB;
    af >> Name("idl") >> *len_data;
}

void MLCMSD::check_structure(const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3)
{
    int nvar1 = info_level1->fl;
    int nvar2 = info_level2->fl;
    int nvar3 = info_level3->fl;

    int cfl = this->get_common_frame_length();
    int lniu = this->get_num_level_in_use();

    if (lniu == 1)
    {
        this->set_common_frame_length(nvar1);
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

/*
    read and write from/to files frame by frame
*/

void MLCMSD::get_dataset_info(string Fname, string Dname, H5T_class_t &Ctype, int &Rank, hsize_t Dims[], size_t &type_size, bool DispF)
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
                printf("# -------------- %8s", "Tx data information \n");
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

void MLCMSD::get_dataset_info(string Fname, string Dname, int &Rank, hsize_t Dims[], H5std_string &my_dtype, bool DispF)
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
    //     printf("# %-58s \n", "================= Load Transmitter Data   ");
    //     printf("# %78s \n", "------------------------------------------------------------------------------- ");
    //     printf("# %16s | %16s | %16s | %16s \n", " H5 Class Type ", "Data Size (Byte)", "Rank  ", " Dimension  ");
    //     printf("# %-78s \n", "------------------------------------------------------------------------------ ");
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
                    // printf("# %-16s | %-16d | %-16d | (%8d, %8d) \n", " FLOAT", (int)type_size, Rank, (int) Dims[0], 1);
                    // printf("# %-78s \n", "------------------------------------------------------------------------------ ");

                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "FLOAT",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
                    printf("# \n");
                    printf("\e[0m");
                }
                else if (Rank == 2)
                {
                    // printf("# %-16s | %-16d | %-16d | (%8d, %8d) \n", " FLOAT", (int)type_size, Rank, (int) Dims[0], (int) Dims[1]);
                    // printf("# %-78s \n", "------------------------------------------------------------------------------ ");
                    printf("\e[1m");
                    printf("# --------------------------------------------------------\n");
                    printf("# -------------- %8s", "Tx data information \n");
                    printf("# --------------------------------------------------------\n");
                    printf("# * Dataset info: \n");
                    printf("# \t ** %-16s\t= %8s\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= %8d\n# \t ** %-16s\t= (%-8d, %8d)\n",
                           "H5 Class Type", "FLOAT",
                           "Data Size (Byte)", (int)type_size,
                           "Rank", Rank,
                           "Dimension", (int)Dims[0], (int)Dims[1]);
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
            if (type_size == 4)
            {
                my_dtype = "int8";
            }
            else if (type_size == 8)
            {
                my_dtype = "int16";
            }

            if (DispF)
            {
                if (Rank == 1)
                {
                    Dims[1] = 1;
                    // printf("# %-16s | %-16d | %-16d | (%8d, %8d) \n", " INTEGER", (int)type_size, Rank, (int)Dims[0], 1);
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
                else if (Rank == 2)
                {
                    // printf("# %-16s | %-16d | %-16d | (%8d, %8d) \n", " INTEGER", (int)type_size, Rank, (int)Dims[0], (int)Dims[1]);
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

void MLCMSD::reshape_h5(string FnameInp, string Dnames[], int numdatasets, string FnameOut)
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
                    read_col_i_from_h5(FnameInp, Dnames[0], Ctype, Dims[0], ifc, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, PredType::IEEE_F32LE, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F32LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, Dnames[1], Ctype, Dims[0], ifc, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, PredType::IEEE_F32LE, Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ
            }
            else if (size_type == 8)
            {
                add_dataset(file, Dnames[0], PredType::IEEE_F64LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, Dnames[0], Ctype, Dims[0], ifc, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, PredType::IEEE_F64LE, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F64LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, Dnames[1], Ctype, Dims[0], ifc, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, PredType::IEEE_F64LE, Dims[0], ifc, temp_col_vals);

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



void MLCMSD::combine_datasets(string FnameInp, string Dnames[], int numdatasets, string FnameOut)
{
    H5T_class_t Ctype; // class type
    size_t size_type;  // size of the data in byte
    hsize_t Dims[2];   // dataset dimensions
    int Rank;
    get_dataset_info(FnameInp, Dnames[0], Ctype, Rank, Dims, size_type, false);

    // ToDo check that staructure of the two datasets are the same: Size and Format
    if (Rank == 1)
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

        Dims[1] = numdatasets;
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
                    read_col_i_from_h5(FnameInp, "TxI"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, PredType::IEEE_F32LE, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F32LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, "TxQ"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, PredType::IEEE_F32LE, Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ
            }
            else if (size_type == 8)
            {
                add_dataset(file, Dnames[0], PredType::IEEE_F64LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    // read_col_i_from_h5(FnameInp, Dnames[0], Ctype, Dims[0], ifc, temp_col_vals);
                    read_col_i_from_h5(FnameInp, "TxI"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, PredType::IEEE_F64LE, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::IEEE_F64LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    // read_col_i_from_h5(FnameInp, Dnames[1], Ctype, Dims[0], ifc, temp_col_vals);
                    read_col_i_from_h5(FnameInp, "TxQ"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, PredType::IEEE_F64LE, Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ
            }
            else
            {
                it_error("Do not support float with this size");
            }
        }
        else if (Ctype == H5T_INTEGER)
        {
            ivec temp_col_vals;
            temp_col_vals.set_length(Dims[0], false);
            if (size_type == 1)
            {
                add_dataset(file, Dnames[0], PredType::STD_I8LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_subset_of_1D_dataset_h5(FnameInp, "TxI"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, size_type, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::STD_I8LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_subset_of_1D_dataset_h5(FnameInp, "TxQ"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, size_type, Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ

            }
            else if (size_type == 2)
            {
                add_dataset(file, Dnames[0], PredType::STD_I16LE, 1, Total_lements, fspace, dataset);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, "TxI"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(dataset, fspace, size_type, Dims[0], ifc, temp_col_vals);

                } // end of frames for TxI

                add_dataset(file, Dnames[1], PredType::STD_I16LE, 1, Total_lements, fspaceQ, datasetQ);
                for (size_t ifc = 0; ifc < Dims[1]; ifc++)
                {
                    read_col_i_from_h5(FnameInp, "TxQ"+to_str(ifc), Ctype, Dims[0], 0, temp_col_vals);
                    write_to_1D_dataset_ith_frame(datasetQ, fspaceQ, size_type, Dims[0], ifc, temp_col_vals);

                } // end of frames forn TxQ
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



void MLCMSD::read_col_i_from_h5(string Fname, string Dname, H5T_class_t &Ctype, hsize_t Count, int col_i, ivec &data_read)
{
    /*
        L is the half of the cw length
    */
    int i, j;
    int TxI_buffer[Count]; /* output buffer */
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
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            it_error("This is not integer fromat");
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {

            IntType inttype = datasetI->getIntType();
            size_t size = inttype.getSize();

            if (size == 1)
            {
                datasetI->read(TxI_buffer, PredType::STD_I8LE, mspaceI, fspaceI);
            }
            else if (size == 2)
            {
                datasetI->read(TxI_buffer, PredType::STD_I16LE, mspaceI, fspaceI);
            }
            else
            {
                it_error("We do not supoort this data fromat");
            }
        }

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
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
};

void MLCMSD::read_col_i_from_h5(string Fname, string Dname, H5T_class_t &Ctype, hsize_t Count, int col_i, vec &data_read)
{
    /*
        L is the half of the cw length
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
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            FloatType floattype = datasetI->getFloatType();
            size_t size = floattype.getSize();
            if (size == 4)
            {
                datasetI->read(TxI_buffer, PredType::IEEE_F32LE, mspaceI, fspaceI);
            }
            else if (size == 8)
            {
                datasetI->read(TxI_buffer, PredType::IEEE_F64LE, mspaceI, fspaceI);
            }
            else
            {
                it_error("We do not supoort this data fromat for float");
            }
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {
            it_error("This is not float data type");
        }

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
            data_read(j) = TxI_buffer[j];

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

void MLCMSD::read_subset_of_1D_dataset_h5(string Fname, string Dname, H5T_class_t &Ctype, hsize_t Count, int col_i, ivec &data_read)
{

    hsize_t size;
    /*
        L is the half of the cw length
    */
    int i, j;
    int8_t TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
    {
        TxI_buffer[j] = 0;
    }

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
        hsize_t mdim[] = {Count}; /* Dimension sizes of the
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
        countm[0] = Count;

        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        stride[0] = 1;
        count[0] = Count; // dims_outI[0];
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */

        start[0] = col_i * Count;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);
        /*
            Read data back to the buffer
        */
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            it_error("This is not integer fromat");
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {

            IntType inttype = datasetI->getIntType();
            size = inttype.getSize();

            if (size == 1)
            {
                datasetI->read(TxI_buffer, PredType::STD_I8LE, mspaceI, fspaceI);
            }
            else if (size == 2)
            {
                datasetI->read(TxI_buffer, PredType::STD_I16LE, mspaceI, fspaceI);
            }
            else
            {
                it_error("We do not supoort this data fromat");
            }
        }

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
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
};

void MLCMSD::read_subset_of_1D_dataset_h5(string Fname, string Dname, H5T_class_t &Ctype, hsize_t Count, int col_i, vec &data_read)
{

    hsize_t size;
    /*
        L is the half of the cw length
    */
    int i, j;
    double TxI_buffer[Count]; /* output buffer */
    for (j = 0; j < Count; j++)
    {
        TxI_buffer[j] = 0;
    }

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
        hsize_t mdim[] = {Count}; /* Dimension sizes of the
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
        countm[0] = Count;

        hsize_t start[1];  // Start of hyperslab
        hsize_t stride[1]; // Stride of hyperslab
        hsize_t count[1];  // Block count
        hsize_t block[1];  // Block sizes
        start[0] = 0;
        block[0] = 1;
        stride[0] = 1;
        count[0] = Count; // dims_outI[0];
        /*
            read col_i from Tx and Rx and combine them together in a vector (data_read)
        */

        start[0] = col_i * Count;
        fspaceI.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        mspaceI.selectHyperslab(H5S_SELECT_SET, countm, startm, stridem, blockm);
        /*
            Read data back to the buffer
        */
        if (Ctype == H5T_FLOAT) // H5T_FLOAT
        {
            FloatType floattype = datasetI->getFloatType();
            size = floattype.getSize();
            if (size == 4)
            {
                datasetI->read(TxI_buffer, PredType::IEEE_F32LE, mspaceI, fspaceI);
            }
            else if (size == 8)
            {
                datasetI->read(TxI_buffer, PredType::IEEE_F64LE, mspaceI, fspaceI);
            }
            else
            {
                it_error("We do not supoort this data fromat for float");
            }
        }
        else if (Ctype == H5T_INTEGER) // H5T_INTEGER
        {
            it_error("This is not float data type");
        }

        data_read.set_length(Count, false);
        for (j = 0; j < Count; j++)
        {
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
};

void MLCMSD::add_dataset(H5File &hdf5file, string DATASET_NAME, PredType pre_def_type, hsize_t FSPACE_RANK, hsize_t Dims[], DataSpace &fspace, DataSet &dataset)
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
    else if (pre_def_type == PredType::NATIVE_FLOAT)
    {
        /*
            * Create property list for a dataset and set up fill values.
        */
        float fillvalue = 0.0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::NATIVE_FLOAT, &fillvalue); // fill value type
        /*
    * Create dataspace for the syndrome dataset in the file.
    */
        fspace = DataSpace(FSPACE_RANK, Dims);
        /*
        *    Create dataset and write it into the file.
        */
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::NATIVE_FLOAT, fspace, plist);
    }
    else if (pre_def_type == PredType::STD_I16LE)
    {

        int16_t fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::STD_I16LE, &fillvalue);

        fspace = DataSpace(FSPACE_RANK, Dims);

        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::STD_I16LE, fspace, plist);
    }
    else if (pre_def_type == PredType::NATIVE_DOUBLE)
    {
        double fillvalue = 0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::NATIVE_DOUBLE, &fillvalue);

        fspace = DataSpace(FSPACE_RANK, Dims);

        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::NATIVE_DOUBLE, fspace, plist);
    }
    else if (pre_def_type == PredType::NATIVE_HBOOL)
    {
        /*
            * Create property list for a dataset and set up fill values.
        */
        bool fillvalue = 0.0; /* Fill value for the dataset syndrome */
        DSetCreatPropList plist;
        plist.setFillValue(PredType::NATIVE_HBOOL, &fillvalue); // fill value type
        /*
    * Create dataspace for the syndrome dataset in the file.
    */
        fspace = DataSpace(FSPACE_RANK, Dims);
        /*
        *    Create dataset and write it into the file.
        */
        dataset = hdf5file.createDataSet(DATASET_NAME, PredType::NATIVE_HBOOL, fspace, plist);
    }
    else
    {
        it_error("Do not support this data type");
    }
};

void MLCMSD::write_to_1D_dataset_ith_frame(DataSet &dataset, DataSpace &fspace, PredType pre_def_type, hsize_t Count, size_t ith_frame, vec &ith_frame_Tx)
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
     *    
     */
        double buff_vector[Count]; // vector buffer for dset

        start[0] = ith_frame * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
        * Buffer update.
        */

        for (i = 0; i < Count; i++)
            buff_vector[i] = ith_frame_Tx(i);

        dataset.write(buff_vector, pre_def_type, mspace1, fspace);

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

void MLCMSD::write_to_1D_dataset_ith_frame(DataSet &dataset, DataSpace &fspace, PredType pre_def_type, hsize_t Count, size_t ith_frame, ivec &ith_frame_Tx)
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
     *    
     */
        int16_t buff_vector[Count]; // vector buffer for dset

        start[0] = ith_frame * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
        * Buffer update.
        */

        for (i = 0; i < Count; i++)
            buff_vector[i] = ith_frame_Tx(i);

        dataset.write(buff_vector, pre_def_type, mspace1, fspace);

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

void MLCMSD::write_to_1D_dataset_ith_frame(DataSet &dataset, DataSpace &fspace, size_t type_size, hsize_t Count, size_t ith_frame, ivec &ith_frame_Tx)
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
     *    
     */
    if (type_size == 1)
    {
        int8_t buff_vector[Count]; // vector buffer for dset

        start[0] = ith_frame * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
        * Buffer update.
        */

        for (i = 0; i < Count; i++)
            buff_vector[i] = (int8_t) ith_frame_Tx(i);

        dataset.write(buff_vector, PredType::STD_I8LE, mspace1, fspace);
    } else if (type_size == 2)
    {
        int16_t buff_vector[Count]; // vector buffer for dset

        start[0] = ith_frame * Count;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
        * Buffer update.
        */

        for (i = 0; i < Count; i++)
            buff_vector[i] = ith_frame_Tx(i);

        dataset.write(buff_vector, PredType::STD_I16LE, mspace1, fspace);
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



void MLCMSD::write_to_2D_dataset_ith_frame(DataSet &dataset, DataSpace &fspace, PredType pre_def_type, hsize_t Count, size_t ith_frame, bvec &ith_frame_Tx)
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
        block[0] = Count;
        block[1] = 1;

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
     *    
     */
        bool buff_vector[Count]; // vector buffer for dset

        start[1] = ith_frame;
        fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);

        /*
        * Buffer update.
        */
        for (i = 0; i < Count; i++)
            buff_vector[i] = ith_frame_Tx(i);

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

void MLCMSD::write_to_2D_dataset_ith_index_mapping(DataSet &dataset, DataSpace &fspace, PredType pre_def_type, hsize_t Count, hsize_t dim, hsize_t zth_frame, bmat &buffer_mat)
{
    /*
        mat MxBogU(fl, dim); OUR standard
        check if fl == Count and dim == given dim
    */
    const int MSPACE1_RANK = 1; // Rank of the dataset in memory
    hsize_t i, j;               // loop indices */

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
        block[0] = Count;
        block[1] = 1;

        /*
            * Create dataspace for the dataset in memory.
        */
        hsize_t dim1[] = {Count}; /* Dimension size of the dataset (in memory) */
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

        if (pre_def_type == PredType::NATIVE_HBOOL)
        {
            /*
            * Write selection from the vector buffer to the dataset in the file. 
            */
            bool buff_vector[Count]; // vector buffer for dset
            start[1] = zth_frame;

            for (j = 0; j < dim; j++)
            {
                start[0] = j * Count;

                fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
                /*
                    * Buffer update.
                */
                for (i = 0; i < Count; i++)
                    buff_vector[i] = buffer_mat(i, j);

                dataset.write(buff_vector, PredType::NATIVE_HBOOL, mspace1, fspace);
            }
        }
        else
        {
            it_error("PredType error: write for PLAINTEXT bool type? ");
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

void MLCMSD::write_to_2D_dataset_ith_index_mapping(DataSet &dataset, DataSpace &fspace, string dtype, hsize_t Count, hsize_t dim, hsize_t zth_frame, bmat &buffer_mat)
{
    /*
        mat MxBogU(fl, dim); OUR standard
        check if fl == Count and dim == given dim
    */
    const int MSPACE1_RANK = 1; // Rank of the dataset in memory
    hsize_t i, j;               // loop indices */

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
        block[0] = Count;
        block[1] = 1;

        /*
            * Create dataspace for the dataset in memory.
        */
        hsize_t dim1[] = {Count}; /* Dimension size of the dataset (in memory) */
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

        if (dtype == "boolean")
        {
            /*
            * Write selection from the vector buffer to the dataset in the file. 
            */
            bool buff_vector[Count]; // vector buffer for dset
            start[1] = zth_frame;

            for (j = 0; j < dim; j++)
            {
                start[0] = j * Count;

                fspace.selectHyperslab(H5S_SELECT_SET, count, start, stride, block);
                /*
                * Buffer update.
                */
                for (i = 0; i < Count; i++)
                    buff_vector[i] = buffer_mat(i, j);

                dataset.write(buff_vector, PredType::NATIVE_HBOOL, mspace1, fspace);
            }
        }
        else
        {
            it_error("PredType error: write for Plaintext bool ");
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
