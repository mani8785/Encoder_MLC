#ifndef MLCMSD_H_
#define MLCMSD_H_

#include <iostream>
#include <sstream>
#include <itpp/itcomm.h>
#include <itpp/stat/misc_stat.h>
#include <itpp/itbase.h>

#include "H5Cpp.h"

using namespace std;
using namespace itpp;
using namespace H5;

struct LEVEL_INFO
{
    /* 
     fl : frame length, 
     pl : parity length,  
     kl : information length, 
     idl: input_data_length,  
    */
    int fl, pl, kl;
    LDPC_Code *my_ldpc;
    vec softbits;
    string peg_file_name;
    int level_no;
    double level_code_rate;
};

class MLCMSD
{
private:
    const hsize_t NoLs;                // num_levels = 6;
    hsize_t NoLiU;               // number_of_levels_in_use = 3;
    hsize_t CFL;           // common_frame_length;
    double R1;                 // code rate R1
    double R2;                 // code rate R2
    double R3;                 // code rate R3

public:
    // MLCMSD(): NoLs{6}
    // {
    //     // constructor
    // };

    MLCMSD(const hsize_t NoLs=6, hsize_t NoLiU_val = 3, hsize_t CFL_val = 1000000, double R1_val = 1.0, double R2_val = 1.0, double R3_val = 1.0)
        : NoLs{6}, NoLiU{NoLiU_val}, CFL{CFL_val}, R1{R1_val}, R2{R2_val}, R3{R3_val}
    {
    };

    ~MLCMSD()
    {
        // destructor
    }
    /*
        MLC environment
        Set methods
    */
    // void set_total_num_levels(int NoLs_val);
    void set_num_level_in_use(int NoLiU_val);
    void set_common_frame_length(int CFL_val);
    void set_Rate(double Rate_val, int level_no);

    /*
        MLC environment
        Get methods
    */

    hsize_t get_NoLiU() { return NoLiU; };
    hsize_t get_CFL() { return CFL; };
    

    /*
        MLC environment
        Check methods
    */

    void load_env(string filetxt, hsize_t &CFL, hsize_t &NoLs, hsize_t &NoLiU, double &R1, double &R2, double &R3);

    void check_structure(const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3);

    void initialize_struct(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name);

    void update_level_info(LEVEL_INFO *info_level, LDPC_Code *ldpc_in, int level_no, string h_name);

    /*
        Methods related to the loading of LDPC codes
    */
    int check_the_encoder_file_format(string code_name);

    void load_peg(string input_peg, int *ROW, int *COL, int *Z_size, imat *base_matrix);

    LDPC_Code fill_ldpc(string code_name);

    /*
        Display methods
    */
    void display_level(LEVEL_INFO *info_level, bool short_info);
    void display_table_title();

    /*
        Encoding methods 
        Applied to each frame
    */
    void encoder_one_level(const bmat &qxB_bin, const LEVEL_INFO &info_level, bmat &plain_texts, ivec &plain_texts_decim, bvec &enc_data_hard);
    void encoder_two_levels(const bmat &qxB_bin, const LEVEL_INFO &info_level1, const LEVEL_INFO &info_level2, bmat &plain_texts_two_levels, ivec &plain_texts_decim, bvec &enc_data_hard_1, bvec &enc_data_hard_2);
    void encoder_three_levels(const bmat *qxB_bin, const LEVEL_INFO *info_level1, const LEVEL_INFO *info_level2, const LEVEL_INFO *info_level3, bmat *plain_texts_LSBs, ivec &plain_texts_decim, bvec *enc_data_hard_1, bvec *enc_data_hard_2, bvec *enc_data_hard_3);
};

#endif /* class MLCMSD */