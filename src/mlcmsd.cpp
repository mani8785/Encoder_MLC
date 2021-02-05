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
        It is actually the read size of our buffer
    */
    CFL = CFL_val;
};

void MLCMSD::set_Rate(double Rate_val, int level_no)
{
    /*
        Set the cose rate and check if the parity file exists in the folder
    */

    stringstream streamR;
    streamR << fixed << setprecision(2) << Rate_val;
    string code_name = "H_"+to_str(CFL)+"_"+streamR.str();

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


double MLCMSD::get_Rate(int level_no)
{
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
        info_level->fl = this->get_CFL();
        info_level->pl = 0.0;
        info_level->kl = this->get_CFL();
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
