#ifndef MLCMSD_H_
#define MLCMSD_H_

#include <sstream>
#include <itpp/itcomm.h>
#include <itpp/stat/misc_stat.h>


using namespace std;
using namespace itpp;




struct QUANTIZER_INFO{
    /*
    bits2sym : the mapping symbols for modulator.
    m  : number of bits per symbol,  
    Mm : modulation order
    sym: The constellation points. Used as a quantization levels
    levels: used for the quantizer
    dx : quantizer step size,
    */
    int m = 6;
    int Mm = 1<<m;
    double Qrange = 3.0; // 6.0;
    double dx = 2.0*Qrange/(Mm-2.0);
    vec sym = linspace(-Qrange, Qrange, Mm); 
    // ivec bits2sym = "0:1:15";  // binarry  mapping m = 4
    ivec bits2sym = "0:1:63";  // binarry  mapping 
    vec levels = sym;
};

struct LEVEL_INFO{
    /* 
     fl : frame length, 
     pl : parity length,  
     kl : information length, 
     idl: input_data_length,  
    */
   int fl, pl, kl, idl;
   LDPC_Code *my_ldpc;
   vec softbits;
   string peg_file_name;
   int level_no;
   double level_code_rate;
};




class MLCMSD{
    private:
        int number_of_levels_in_use;
        int common_frame_length;

    public:
        MLCMSD(){
        // constructor
    }

        ~MLCMSD(){
        // destructor
    }

        void set_num_level_in_use(int num_lev);

        void set_common_frame_length(int fl);

        int get_num_level_in_use(){return number_of_levels_in_use; };

        int get_common_frame_length(){return common_frame_length; };

        void check_structure(const LEVEL_INFO * info_level1, const LEVEL_INFO * info_level2, const LEVEL_INFO * info_level3);

        void initialize_struct(LEVEL_INFO * info_level, LDPC_Code * ldpc_in, int level_no, int leng, string h_name);

        void update_level_info(LEVEL_INFO * info_level, LDPC_Code * ldpc_in, int level_no, int leng, string h_name);

        int check_the_input_type(string code_name);

        void load_peg(string input_peg, int* ROW, int* COL, int* Z_size, imat * base_matrix);

        LDPC_Code fill_ldpc(string code_name);

        void load_test_data(string data_name, int* len_data, vec * XA, vec * XB);

        void display_quant(QUANTIZER_INFO * info_adc);
        void display_level(LEVEL_INFO * info_level, bool short_info);
        void display_table_title();

        // ======================== One levels
        void encoder_one_level(const bmat * qxB_bin, const LEVEL_INFO * info_level, const QUANTIZER_INFO * info_adc, bmat * plain_texts, bvec * enc_data_hard);

        // ========================= Two levels
        void encoder_two_levels(const bmat * qxB_bin, const LEVEL_INFO * info_level1, const LEVEL_INFO * info_level2, const QUANTIZER_INFO * info_adc, bmat * plain_texts_two_levels, bvec * enc_data_hard_1, bvec * enc_data_hard_2);
        
        // ========================= Three levels
        void encoder_three_levels(const bmat * qxB_bin, const LEVEL_INFO * info_level1, const LEVEL_INFO * info_level2, const LEVEL_INFO * info_level3, const QUANTIZER_INFO * info_adc, bmat * plain_texts_LSBs, bvec * enc_data_hard_1, bvec * enc_data_hard_2, bvec * enc_data_hard_3);

};


#endif /* class MLCMSD */