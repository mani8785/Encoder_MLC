#include <sstream>
#include "mlc_msd.h"

void MLCMSD::set_num_level_in_use(int num_lev)
{
    number_of_levels_in_use = num_lev;
};

void MLCMSD::set_common_frame_length(int fl){
    common_frame_length = fl;
};

int MLCMSD::check_the_input_type(string code_name)
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

void MLCMSD::initialize_struct(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, int leng, string h_name)
{
    size_t found_type = h_name.find("skip");
    if (found_type != std::string::npos)
    {
        info_level->my_ldpc = ldpc_in;
        info_level->fl = 0.0;
        info_level->pl = 0.0;
        info_level->kl = 0.0;
        info_level->idl = leng;
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
        info_level->idl = leng;
        info_level->peg_file_name = h_name;
        info_level->level_no = level_no;
        info_level->level_code_rate = ldpc_in->get_rate();
    }
}

void MLCMSD::update_level_info(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, int leng, string h_name)
{
    size_t found_type = h_name.find("skip");
    if (found_type != std::string::npos)
    {
        info_level->my_ldpc = ldpc_in;
        info_level->fl = this->get_common_frame_length();
        info_level->pl = 0.0;
        info_level->kl = this->get_common_frame_length();
        info_level->idl = leng;
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
    int check_input_type = check_the_input_type(code_name);

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

vec MLCMSD::calc_sof_bits(ivec *qxA, bmat *qxA_bin, bmat *qxB_bin, double N0, int level_no, LEVEL_INFO *info_level, QUANTIZER_INFO *info_adc)
{
    int fl = info_level->fl;
    int m = info_adc->m;
    if (m > 6)
        it_error("The quantizer with more than 6 bits is not supported");

    vec softbits;
    ivec l_qxB_4to0, r_qxB_4to0;
    ivec pp_qxB_3to0, pn_qxB_3to0, np_qxB_3to0, nn_qxB_3to0;
    bmat qxB_4to0_bin, qxB_3to0_bin;
    softbits.set_length(info_level->fl, false);
    l_qxB_4to0.set_length(info_level->fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    r_qxB_4to0.set_length(info_level->fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    pp_qxB_3to0.set_size(fl, false);
    pn_qxB_3to0.set_size(fl, false);
    np_qxB_3to0.set_size(fl, false);
    nn_qxB_3to0.set_size(fl, false);
    //    cout << "The ber before decoding is  = " << sum/fl << endl;
    // ----------------------------------    // ----------------------------------    // Find a-posteriori probability
    qxB_4to0_bin = qxB_bin->get(0, fl - 1, 1, m - 1); // binary equivalent of the LSBs of qXB  // p_qxB_0_2
    qxB_3to0_bin = qxB_bin->get(0, fl - 1, 2, m - 1); // binary equivalent of the LSBs of qXB, 2 LSB

    if (level_no == 1)
    {
        // l_qxB_4to0 : the quantized version of Bob's data with MSB bit set to zero for 6 bit quantizer
        // r_qxB_4to0 : the quantized version of Bob's data with MSB bit set to one (right)
        // qxB_4to0_bin : Binary version of  Bob's Low significant bits
        // qxB_02.set_length(info_l1.fl, false);   // The LSBs of qxB when MSB is det to 0; [0 xB_02]
        // r_qxB_4to0.set_length(info_l1.fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]

        ivec l_qxB_4to0, r_qxB_4to0;
        // bmat qxB_4to0_bin;
        softbits.set_length(info_level->fl, false);
        l_qxB_4to0.set_length(info_level->fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
        r_qxB_4to0.set_length(info_level->fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
        //    cout << "The ber before decoding is  = " << sum/fl << endl;
        // ----------------------------------    // ----------------------------------    // Find a-posteriori probability
        // qxB_4to0_bin = qxB_bin->get(0, info_level->fl - 1, 1, info_adc->m - 1); // binary equivalent of the LSBs of qXB  // p_qxB_0_2
        for (int cc = 0; cc < info_level->fl; cc++)
        {
            l_qxB_4to0(cc) = bin2dec(qxB_4to0_bin.get_row(cc));
            r_qxB_4to0(cc) = 8 + l_qxB_4to0(cc); // n_qxB_0_2
        }
        // ----------------------------------    // ----------------------------------    //	Adjust Alice's data
        for (int cc = 0; cc < info_level->fl; cc++)
            softbits(cc) = (-8.0) * (l_qxB_4to0(cc) + r_qxB_4to0(cc) - 2.0 * qxA->get(cc)) / N0;

        return softbits;
    }
    else if (level_no == 2)
    {
        for (int cc = 0; cc < fl; cc++)
        {
            // necessary for level 4:
            l_qxB_4to0(cc) = bin2dec(qxB_4to0_bin.get_row(cc));
            r_qxB_4to0(cc) = 8 + l_qxB_4to0(cc);

            // necessary for levels 3 and 4:
            pp_qxB_3to0(cc) = bin2dec(qxB_3to0_bin.get_row(cc));
            pn_qxB_3to0(cc) = 4 + pp_qxB_3to0(cc);
            np_qxB_3to0(cc) = 8 + pp_qxB_3to0(cc);
            nn_qxB_3to0(cc) = 12 + pp_qxB_3to0(cc);
        }
        double num = 1;
        double denum = 1;
        for (int cc = 0; cc < fl; cc++)
        {
            num = exp(-1.0 * pow(nn_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pn_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
            denum = exp(-1.0 * pow(np_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pp_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
            softbits(cc) = log(num / denum);
        }
    }
    else if (level_no == 3)
    {
    }
    else
    {
        it_error("Currently just 3 levels are supported");
    }

    return softbits;
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
        this->set_common_frame_length(nvar2);
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
        if ((nvar1 == nvar2) && (nvar3 == nvar1) && (nvar3==cfl))
        {
            cout << "Frame length are equal for all the levels" << endl;
        }
        else
        {
            it_error("The frame lengths are not equal");
        }
    }
}

double MLCMSD::calc_snr_numerical(vec *x_A, vec *x_B)
{
    double ma = mean(*x_A); // mean x_A
    double mb = mean(*x_B); // mean x_B
    double varA = variance(*x_A);
    double varB = variance(*x_B);
    double rho = mean(elem_mult((*x_A - ma), (*x_B - mb))) / sqrt(varA * varB);
    // cout << "The rho = "<< rho << "and the sigma_b = " <<  pow(rho, -1.0) << endl;
    // cout << "The snr form the method 1 is: " << varA/ (pow(rho, -2.0)-varA)  << endl;
    return varA / (pow(rho, -2.0) - varA);
}

void MLCMSD::display_quant(QUANTIZER_INFO *info_adc)
{
    printf("# * Quantizerl info: \n");
    printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d -- %8d\n# \t ** %8s\t= %8.3f -- %8.3f\n",
           "Number of bits   ", info_adc->m,
           "Output symbols   ", info_adc->bits2sym(0), info_adc->bits2sym(info_adc->Mm - 1),
           "Quantizer Range  ", -info_adc->Qrange, info_adc->Qrange);
}

void MLCMSD::display_level(LEVEL_INFO *info_level, bool short_info)
{
    int number_of_frames = info_level->idl / info_level->fl + 1;
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
        printf("# * Test Data info: \n");
        printf("# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n# \t ** %8s\t= %8d\n",
               "Data length     ", info_level->idl,
               "frame length    ", info_level->fl,
               "frame number    ", number_of_frames);
        printf("#\n# The simulation is running ...\n");
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

void MLCMSD::display_efficiency(BERC *berc, BLERC *ferc, double snr, double n0, LEVEL_INFO * info_level, LDPC_Code *ldpc, int level_no)
{
    double capacity_level = (0.5 * log2(1 + snr));
    capacity_level = 0.2;

    printf("\r");
    printf("  %8d | %8.2f || %8.f | %8.f | %8.f | %.2e | %.2e ||\n",
           level_no,
           info_level->level_code_rate,
           ferc->get_total_blocks(),
           ferc->get_errors(),
           berc->get_errors(),
           berc->get_errorrate(),
           ferc->get_errorrate());
    fflush(stdout);
    printf("# %8s | %8s || %8s | %8s | %8s | %8s | %8s ||\n",
           "--------", "--------", "--------", "--------", "--------", "--------", "--------");
}

// ==================== Enc/Dec for one level
void MLCMSD::encoder_one_level(const bmat *qxB_bin, const LEVEL_INFO *info_level, const QUANTIZER_INFO *info_adc, bmat *plain_texts, bvec *enc_data_hard)
{
    int fl = info_level->fl;
    int m = info_adc->m;
    bvec bin_xB_5 = qxB_bin->get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    if (info_level->pl > 0)
    {
        QLLRvec synd_llr_xB_5 = info_level->my_ldpc->soft_syndrome_check(sign_xB_5);
        *enc_data_hard = synd_llr_xB_5 > 0;
    }
    *plain_texts = qxB_bin->get(0, fl - 1, 1, m - 1);
}

void MLCMSD::decoder_one_level(const ivec *qxA, const bmat *qxA_bin, const bmat *qxB_LSBs_mat, const bvec *synd, const double N0, const int level_no, const LEVEL_INFO *info_level, const QUANTIZER_INFO *info_adc, bvec *decoded_hard)
{
    int fl = info_level->fl;
    int m = info_adc->m;
    QLLRvec llr_in, llr_out;
    if (m > 6)
        it_error("The quantizer with more than 6 bits is not supported");

    vec softbits;
    ivec l_qxB_4to0_local, r_qxB_4to0_local;
    // bmat qxB_4to0_bin;
    softbits.set_length(fl, false);
    l_qxB_4to0_local.set_length(fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    r_qxB_4to0_local.set_length(fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    //    cout << "The ber before decoding is  = " << sum/fl << endl;
    // ----------------------------------    // ----------------------------------    // Find a-posteriori probability
    // softbits.set_length(info_level->fl, false);
    // l_qxB_4to0.set_length(info_level->fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    // r_qxB_4to0.set_length(info_level->fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    // ----------------------------------    // ----------------------------------    // Find a-posteriori probability
    for (int cc = 0; cc < info_level->fl; cc++)
    {
        l_qxB_4to0_local(cc) = bin2dec(qxB_LSBs_mat->get_row(cc));
        r_qxB_4to0_local(cc) = (1 << (m - 1)) + l_qxB_4to0_local(cc); // n_qxB_0_2
    }
    // ----------------------------------    // ----------------------------------    //	Adjust Alice's data
    for (int cc = 0; cc < info_level->fl; cc++)
        softbits(cc) = (-(1 << (m - 1))) * (l_qxB_4to0_local(cc) + r_qxB_4to0_local(cc) - 2.0 * qxA->get(cc)) / N0;


    llr_in = info_level->my_ldpc->get_llrcalc().to_qllr(softbits);

    if (info_level->pl > 0)
    {
        int ittt = info_level->my_ldpc->bp_decode_s(llr_in, *synd, llr_out);
        *decoded_hard = llr_out > 0; // 0-> -1,   1 -> 1 :  2c-1
    }else{
        *decoded_hard = llr_in > 0;
    }
    
}

// ==================== Enc/Dec for Two levels
void MLCMSD::encoder_two_levels(const bmat *qxB_bin, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const QUANTIZER_INFO *info_adc, bmat *plain_texts_two_levels, bvec *enc_data_hard_1, bvec *enc_data_hard_2)
{
    int fl = info_level1->fl;
    int m = info_adc->m;
    bvec bin_xB_5 = qxB_bin->get_col(0); // bin_a_level_i = x_A > 0; // 0-> -1,   1 -> 1 :  2c-1
    bvec bin_xB_4 = qxB_bin->get_col(1);
    ivec sign_xB_5 = 2 * to_ivec(bin_xB_5) - 1;
    ivec sign_xB_4 = 2 * to_ivec(bin_xB_4) - 1;

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

    *plain_texts_two_levels = qxB_bin->get(0, fl - 1, 2, m - 1);
}

void MLCMSD::decoder_two_levels(const ivec *qxA, const bmat *qxA_bin, const bmat *qxB_LSBs, const bvec *enc_data_1, const bvec *enc_data_2, const double N0, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const QUANTIZER_INFO *info_adc, bvec *dec_data_1, bvec *dec_data_2)
{
    int fl = info_level1->fl;
    int m = info_adc->m;
    QLLRvec llr_in1, llr_out1, llr_in2, llr_out2;
    if (m > 6)
        it_error("The quantizer with more than 6 bits is not supported");

    vec softbits1, softbits2;
    ivec l_qxB_4to0, r_qxB_4to0;                             // should be updated after decoding of the second level
    ivec pp_qxB_3to0, pn_qxB_3to0, np_qxB_3to0, nn_qxB_3to0; // should be updated by plain_texts(qxB_LSBs)
    bmat qxB_4to0_bin;
    softbits1.set_length(fl, false);
    softbits2.set_length(fl, false);
    l_qxB_4to0.set_length(fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    r_qxB_4to0.set_length(fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    pp_qxB_3to0.set_size(fl, false);
    pn_qxB_3to0.set_size(fl, false);
    np_qxB_3to0.set_size(fl, false);
    nn_qxB_3to0.set_size(fl, false);
    //---------------------------------------- For second MSB
    qxB_4to0_bin.set_size(fl, m - 1, false); // binary equivalent of the LSBs of qXB
    // qxB_3to0_bin.set_size(fl, m-2, false);
    // qxB_3to0_bin = *qxB_LSBs; // binary equivalent of the LSBs of qXB  :  qxB_LSBs
    for (int cc = 0; cc < fl; cc++)
    {
        pp_qxB_3to0(cc) = bin2dec(qxB_LSBs->get_row(cc));
        pn_qxB_3to0(cc) = (1 << (m - 2)) + pp_qxB_3to0(cc);
        np_qxB_3to0(cc) = (1 << (m - 1)) + pp_qxB_3to0(cc);
        nn_qxB_3to0(cc) = (1 << (m - 1)) + (1 << (m - 2)) + pp_qxB_3to0(cc);
    }
    double num = 1;
    double denum = 1;
    for (int cc = 0; cc < fl; cc++)
    {
        num = exp(-1.0 * pow(nn_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pn_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
        denum = exp(-1.0 * pow(np_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pp_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
        softbits2(cc) = log(num / denum);
    }
    llr_in2 = info_level2->my_ldpc->get_llrcalc().to_qllr(softbits2);
    int ittt = info_level2->my_ldpc->bp_decode_s(llr_in2, *enc_data_2, llr_out2); // modified LDPC decoder
    *dec_data_2 = llr_out2 > 0;                                                   // 0-> -1,   1 -> 1 :  2c-1
    // --------------------------------------- For MSB
    for (int ii = 0; ii < m - 1; ii++)
    {
        if (ii == 0)
        {
            qxB_4to0_bin.set_col(ii, *dec_data_2);
        }
        else
        {
            qxB_4to0_bin.set_col(ii, qxB_LSBs->get_col(ii - 1));
        }
    }

    // qxB_4to0_bin.set_cols(2, *qxB_LSBs);
    // qxB_4to0_bin.set_col(1, *dec_data_2);

    // bvec dec_data_1_temp;
    // dec_data_1_temp.set_length(fl, false);
    // decoder_one_level(qxA, qxA_bin, &qxB_4to0_bin, synd1, N0, 1, info_level1, info_adc, &dec_data_1_temp);

    for (int cc = 0; cc < fl; cc++)
    {
        l_qxB_4to0(cc) = bin2dec(qxB_4to0_bin.get_row(cc));
        r_qxB_4to0(cc) = 8 + l_qxB_4to0(cc); // n_qxB_0_2
    }
    // ----------------------------------    // ----------------------------------    //	Adjust Alice's data
    for (int cc = 0; cc < fl; cc++)
        softbits1(cc) = (-8.0) * (l_qxB_4to0(cc) + r_qxB_4to0(cc) - 2.0 * qxA->get(cc)) / N0;

    llr_in1 = info_level1->my_ldpc->get_llrcalc().to_qllr(softbits1);
    int ittt2 = info_level1->my_ldpc->bp_decode_s(llr_in1, *enc_data_1, llr_out1);
    *dec_data_1 = llr_out1 > 0;
}

void MLCMSD::decoder_two_levels_new(const ivec *qxA, const bmat *qxA_bin, const bmat *qxB_LSBs, const bvec *enc_data_1, const bvec *enc_data_2, const double N0, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const QUANTIZER_INFO *info_adc, bvec *dec_data_1, bvec *dec_data_2)
{
    int fl = info_level1->fl;
    int m = info_adc->m;
    QLLRvec llr_in1, llr_out1, llr_in2, llr_out2;
    if (m > 6)
        it_error("The quantizer with more than 6 bits is not supported");

    vec softbits1, softbits2;
    ivec l_qxB_4to0, r_qxB_4to0;                             // should be updated after decoding of the second level
    ivec pp_qxB_3to0, pn_qxB_3to0, np_qxB_3to0, nn_qxB_3to0; // should be updated by plain_texts(qxB_LSBs)
    softbits1.set_length(fl, false);
    softbits2.set_length(fl, false);
    l_qxB_4to0.set_length(fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    r_qxB_4to0.set_length(fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    pp_qxB_3to0.set_size(fl, false);
    pn_qxB_3to0.set_size(fl, false);
    np_qxB_3to0.set_size(fl, false);
    nn_qxB_3to0.set_size(fl, false);
    //---------------------------------------- For second MSB
    // qxB_3to0_bin.set_size(fl, m-2, false);
    // qxB_3to0_bin = *qxB_LSBs; // binary equivalent of the LSBs of qXB  :  qxB_LSBs
    for (int cc = 0; cc < fl; cc++)
    {
        pp_qxB_3to0(cc) = bin2dec(qxB_LSBs->get_row(cc));
        pn_qxB_3to0(cc) = (1 << (m - 2)) + pp_qxB_3to0(cc);
        np_qxB_3to0(cc) = (1 << (m - 1)) + pp_qxB_3to0(cc);
        nn_qxB_3to0(cc) = (1 << (m - 1)) + (1 << (m - 2)) + pp_qxB_3to0(cc);
    }
    double num = 1;
    double denum = 1;
    for (int cc = 0; cc < fl; cc++)
    {
        num = exp(-1.0 * pow(nn_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pn_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
        denum = exp(-1.0 * pow(np_qxB_3to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pp_qxB_3to0(cc) - qxA->get(cc), 2) / N0);
        softbits2(cc) = log(num / denum);
    }
    llr_in2 = info_level2->my_ldpc->get_llrcalc().to_qllr(softbits2);

    if (info_level2->pl > 0)
    {
        int ittt = info_level2->my_ldpc->bp_decode_s(llr_in2, *enc_data_2, llr_out2); // modified LDPC decoder
        *dec_data_2 = llr_out2 > 0;  
    }else{
        *dec_data_2 = llr_in2 > 0;
    }


                                                     // 0-> -1,   1 -> 1 :  2c-1
    // --------------------------------------- For MSB
    // ----------------------- MLC-MSD two levels
    bmat qxB_4to0_bin;
    qxB_4to0_bin.set_size(fl, m - 1, false); // binary equivalent of the LSBs of qXB
    qxB_4to0_bin.set_cols(1, *qxB_LSBs);
    qxB_4to0_bin.set_col(0, *dec_data_2);
    MLCMSD temp_1d_level;
    temp_1d_level.set_num_level_in_use(1);
    temp_1d_level.set_common_frame_length(this->get_common_frame_length());
    temp_1d_level.decoder_one_level(qxA, qxA_bin, &qxB_4to0_bin, enc_data_1, N0, 1, info_level1, info_adc, dec_data_1);
}

// ==================== Enc/Dec for Three levels

void MLCMSD::encoder_three_levels(const bmat *qxB_bin, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3, const QUANTIZER_INFO *info_adc, bmat *plain_texts_LSBs, bvec *enc_data_hard_1, bvec *enc_data_hard_2, bvec *enc_data_hard_3)
{
    int fl = info_level1->fl;
    int m = info_adc->m;
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
    *plain_texts_LSBs = qxB_bin->get(0, fl - 1, 3, m - 1);
}

void MLCMSD::decoder_three_levels(const ivec *qxA, const bmat *qxA_bin, const bmat *qxB_LSBs_for_3_levels, const bvec *enc_data_1, const bvec *enc_data_2, const bvec *enc_data_3, const double N0, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3, const QUANTIZER_INFO *info_adc, bvec *dec_data_1, bvec *dec_data_2, bvec *dec_data_3)
{
    int fl = info_level1->fl;
    int m = info_adc->m;
    QLLRvec llr_in1, llr_out1, llr_in2, llr_out2, llr_in3, llr_out3;
    if (m > 6)
        it_error("The quantizer with more than 6 bits is not supported");

    vec softbits1, softbits2, softbits3;
    // ivec l_qxB_4to0, r_qxB_4to0;                             // should be updated after decoding of the second level
    // ivec pp_qxB_3to0, pn_qxB_3to0, np_qxB_3to0, nn_qxB_3to0; // should be updated by plain_texts(qxB_LSBs)
    ivec ppp_qxB_2to0, ppn_qxB_2to0, pnp_qxB_2to0, pnn_qxB_2to0; // should be updated by plain_texts(qxB_LSBs)
    ivec npp_qxB_2to0, npn_qxB_2to0, nnp_qxB_2to0, nnn_qxB_2to0; // should be updated by plain_texts(qxB_LSBs)
    // bmat qxB_4to0_bin;
    // softbits1.set_length(fl, false);
    // softbits2.set_length(fl, false);
    softbits3.set_length(fl, false);
    // l_qxB_4to0.set_length(fl, false); // The LSBs of qxB when MSB is det to 0; [0 xB_02]
    // r_qxB_4to0.set_length(fl, false); // the LSBs of qxB when MSB is set to 1; [1 xB_02]
    // pp_qxB_3to0.set_size(fl, false);
    // pn_qxB_3to0.set_size(fl, false);
    // np_qxB_3to0.set_size(fl, false);
    // nn_qxB_3to0.set_size(fl, false);

    ppp_qxB_2to0.set_size(fl, false);
    ppn_qxB_2to0.set_size(fl, false);
    pnp_qxB_2to0.set_size(fl, false);
    pnn_qxB_2to0.set_size(fl, false);
    npp_qxB_2to0.set_size(fl, false);
    npn_qxB_2to0.set_size(fl, false);
    nnp_qxB_2to0.set_size(fl, false);
    nnn_qxB_2to0.set_size(fl, false);
    //---------------------------------------- For third MSB
    for (int cc = 0; cc < fl; cc++)
    {
        ppp_qxB_2to0(cc) = bin2dec(qxB_LSBs_for_3_levels->get_row(cc), true); // ToDo
        ppn_qxB_2to0(cc) = (1 << (m - 3)) + ppp_qxB_2to0(cc);
        pnp_qxB_2to0(cc) = (1 << (m - 2)) + ppp_qxB_2to0(cc);
        pnn_qxB_2to0(cc) = (1 << (m - 2)) + (1 << (m - 3)) + ppp_qxB_2to0(cc);

        npp_qxB_2to0(cc) = (1 << (m - 1)) + ppp_qxB_2to0(cc);
        npn_qxB_2to0(cc) = (1 << (m - 1)) + (1 << (m - 3)) + ppp_qxB_2to0(cc);
        nnp_qxB_2to0(cc) = (1 << (m - 1)) + (1 << (m - 2)) + ppp_qxB_2to0(cc);
        nnn_qxB_2to0(cc) = (1 << (m - 1)) + (1 << (m - 2)) + (1 << (m - 3)) + ppp_qxB_2to0(cc);
    }

    double num = 1;
    double denum = 1;
    for (int cc = 0; cc < fl; cc++)
    {
        num = exp(-1.0 * pow(ppn_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pnn_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(npn_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(nnn_qxB_2to0(cc) - qxA->get(cc), 2) / N0);
        denum = exp(-1.0 * pow(ppp_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(pnp_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(npp_qxB_2to0(cc) - qxA->get(cc), 2) / N0) + exp(-1.0 * pow(nnp_qxB_2to0(cc) - qxA->get(cc), 2) / N0);
        softbits3(cc) = log(num / denum);
    }
    


    llr_in3 = info_level3->my_ldpc->get_llrcalc().to_qllr(softbits3);

    if (info_level3->pl > 0)
    {
        int ittt = info_level3->my_ldpc->bp_decode_s(llr_in3, *enc_data_3, llr_out3); // modified LDPC decoder
        *dec_data_3 = llr_out3 > 0;  
    }else{
        *dec_data_3 = llr_in3 > 0;
    }

    //---------------------------------------- For second MSB
    // ----------------------- MLC-MSD two levels
    bmat plain_texts_new;
    plain_texts_new.set_size(fl, m - 2, false);
    plain_texts_new.set_cols(1, *qxB_LSBs_for_3_levels);
    plain_texts_new.set_col(0, *dec_data_3);
    MLCMSD temp_2d_level;
    temp_2d_level.set_num_level_in_use(2);
    temp_2d_level.set_common_frame_length(this->get_common_frame_length());
    temp_2d_level.decoder_two_levels_new(qxA, qxA_bin, &plain_texts_new, enc_data_1, enc_data_2, N0, info_level1, info_level2, info_adc, dec_data_1, dec_data_2);
};
