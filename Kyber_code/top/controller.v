module controller (
    input clk,
    input rst,
    input start,
    input [1:0] kyber_mode,//0: kyber512; 1: kyber768; 2: kyber1024;
    input [1:0] mode, // 0:KeyGen, 1:Enc, 2:Dec
    input [255:0] random_coin,
    output finish,
    input [13055:0] sk_in,
    input [6143:0] sk_out_cpa,
    output [13055:0] sk_out,
    input [255:0] m_in,
    input [255:0] m_out_cpa,
    input [6399:0] pk_out,
    input [6399:0] pk_in,
    input [6143:0] c_in,
    output [255:0] m_out,
    
    output reg ntt_start,
    output reg [1:0] ntt_mode,
    output reg ntt_is_add_or_sub,//0: ADD; 1: SUB;
    output reg [9:0] ntt_ram_r_start_offset_A,
    output reg [9:0] ntt_ram_r_start_offset_B,
    output reg [9:0] ntt_ram_w_start_offset,
    input NTT_finish,
    output reg NTT_rst,

    output reg [511:0] G_in,
    output reg G_active,
    output reg G_rst,
    output reg G_mode,
    input G_finish,
    input [511:0] G_out,
    output reg [255:0] G_rho,

    output reg CBD_rst, 
    output reg CBD_active,
    output reg [1:0] CBD_num, //1 => eta=3; 2 => eta=2
    output reg [9:0] CBD_ram_w_start_offset,
    input CBD_finsh,
    output reg [263:0] CBD_input,

    
    output reg A_gen_rst, 
    output reg A_gen_active,
    output reg [9:0] A_gen_ram_w_start_offset,
    output reg [15:0] A_gen_diff,
    input A_finsh,
    output reg [271:0] A_in,
    
    output reg coder_rst,
    output reg coder_active,
    output reg coder_load_input_Enc,
    output reg coder_load_input_Dec,
    output reg [3:0] coder_mode, 
    input CODER_finish,
    input [255:0] rho_from_pk,
    input [6143:0] c_out,
    output reg [6143:0] c_out_reg,
    output [6399:0] coder_pk_in,
    output [255:0] coder_m_in,
    input  [6399:0] pk_from_sk,

    output reg [12543:0] H_in,
    output reg H_active,
    output reg H_rst,
    output reg [1:0] H_mode,
    input H_finish,
    input [255:0] H_out,

    output reg [511:0] KDF_in,
    output reg KDF_active,
    output reg [1:0] KDF_mode,
    output reg KDF_rst,
    input KDF_finish,
    input [1535:0] KDF_out,
    output [255:0] K_out,

    output reg ram_r_sel, // 0:from coder, 1:from ntt
    output reg [1:0] ram_w_sel, // 0:from coder, 1:from ntt, 2:from A_gen, 3:from CBD

    input DO_finish,
    output reg DO_active,
    output reg [9:0] DO_ram_r_offset,
    output reg [9:0] DO_ram_w_offset,
    output reg DO_rst,
    output reg [2:0] DO_mode
);

reg start_reg;
reg A_finsh_reg,CBD_finsh_reg,NTT_finish_reg,CODER_finish_reg,G_finish_reg,H_finish_reg,KDF_finish_reg,DO_finish_reg;//每次次级模块完成后的信号
reg A_SAMPLE_finish,CBD_SAMPLE_finish,NTT_PHASE_finish,COODER_PHASE_finish,DO_DEC_PRENTT_finish,DO_DEC_POSTNTT_finish,DO_DEC_POSTINTT_finish,DO_POSTINTT_finish,DO_ENC_PRENTT_finish,DO_ENC_POSTINTT_finish,DO_KG_finish,DO_KG_A_POSTNTT_finish,NTT_U_finish,NTT_SE_finish,INTT_SU_finish;
reg [1:0] state_reg;//存储mode
reg [4:0] A_gen_state,A_gen_nstate;
reg [2:0] CBD_state,CBD_nstate;
reg [4:0] ntt_state,ntt_nstate;
reg [3:0] coder_state,coder_nstate;
reg [2:0] H_state,H_nstate;
reg [1:0] G_state,G_nstate;
reg [1:0] KDF_state,KDF_nstate;
reg [4:0] DO_state,DO_nstate;
reg A_gen_finish[3:0][3:0],CBD_S_finish[3:0],CBD_E_finish[3:0],CBD_R_finish[3:0],CBD_E1_finish[3:0],T_finish,NTT_S_finish[3:0],CBD_E2_finish,CODER_DECODE_csk_finish,CODER_DECODE_pkm_finish,CODER_ENCODE_m_finish,H_pk_finish,H_c_finish,NTT_deccpa_finish,NTT_ENC_INTT_finish;
//reg [255:0] rho,sigma;
reg [255:0] H_pk,H_c,H_m,G_k,G_r,G_sigma,KDF_K,rho;

// kyber_mode define
parameter kyber512 = 2'd0;
parameter kyber768 = 2'd1;
parameter kyber1024 = 2'd2;

// mode define
parameter KeyGen = 2'd0;
parameter Enc = 2'd1;
parameter Dec = 2'd2;
parameter FINISH = 2'd3;

// A_gen_state define
parameter A_gen_st_INTT = 5'd0;
parameter A_gen_st_A00 = 5'd1;
parameter A_gen_st_A01 = 5'd2;
parameter A_gen_st_A10 = 5'd3;
parameter A_gen_st_A11 = 5'd4;

// CBD_state define
parameter CBD_st_INTT = 3'd0;
parameter CBD_st_s0 = 3'd1; // KeyGen
parameter CBD_st_s1 = 3'd2; // KeyGen
parameter CBD_st_e0 = 3'd3; // KeyGen
parameter CBD_st_e1 = 3'd4; // KeyGen
parameter CBD_st_r0 = 3'd1; // Enc
parameter CBD_st_r1 = 3'd2; // Enc
parameter CBD_st_e10 = 3'd3; // Enc
parameter CBD_st_e11 = 3'd4; // Enc
parameter CBD_st_e2 = 3'd5; // Enc

// ntt_state define
parameter NTT_st_INTT          = 5'd0;
parameter NTT_st_INTT_2        = 5'd26;
parameter NTT_st_INTT_3        = 5'd27;
parameter NTT_st_INTT_4        = 5'd28;
parameter NTT_st_INTT_5        = 5'd29;

parameter NTT_st_NTT_s0        = 5'd1; // KeyGen-NTT(s0)
parameter NTT_st_NTT_s1        = 5'd2; // KeyGen-NTT(s1)
parameter NTT_st_MUL_A00_s0    = 5'd3; // KeyGen-MULT(A00*s0)
parameter NTT_st_MUL_A10_s1    = 5'd4; // KeyGen
parameter NTT_st_NTT_e0        = 5'd5; // KeyGen
parameter NTT_st_ADD_t0        = 5'd6; // KeyGen
parameter NTT_st_ADD_e0        = 5'd7; // KeyGen
parameter NTT_st_MUL_A01_s0    = 5'd8; // KeyGen
parameter NTT_st_MUL_A11_s1    = 5'd9; // KeyGen
parameter NTT_st_NTT_e1        = 5'd10; // KeyGen
parameter NTT_st_ADD_t1        = 5'd11; // KeyGen
parameter NTT_st_ADD_e1        = 5'd12; // KeyGen

parameter NTT_st_NTT_r0        = 5'd1; // Enc
parameter NTT_st_NTT_r1        = 5'd2; // Enc
parameter NTT_st_MUL_A00_r0    = 5'd3; // Enc
parameter NTT_st_MUL_A01_r1    = 5'd4; // Enc
parameter NTT_st_ADD_u0        = 5'd5; // Enc
parameter NTT_st_INVNTT_u0     = 5'd6; // Enc
parameter NTT_st_ADD_e10       = 5'd7; // Enc
parameter NTT_st_MUL_A10_r0    = 5'd8; // Enc
parameter NTT_st_MUL_A11_r1    = 5'd9; // Enc
parameter NTT_st_ADD_u1        = 5'd10; // Enc
parameter NTT_st_INVNTT_u1     = 5'd11; // Enc
parameter NTT_st_ADD_e11       = 5'd12; // Enc
parameter NTT_st_MUL_t0_r0     = 5'd13; // Enc
parameter NTT_st_MUL_t1_r1     = 5'd14; // Enc
parameter NTT_st_ADD_v         = 5'd15; // Enc
parameter NTT_st_INVNTT_v      = 5'd16; // Enc
parameter NTT_st_ADD_e2        = 5'd17; // Enc
parameter NTT_st_ADD_m         = 5'd18; // Enc

parameter NTT_st_NTT_u0        = 5'd19; // Dec
parameter NTT_st_NTT_u1        = 5'd20; // Dec
parameter NTT_st_MUL_s0_u0     = 5'd21; // Dec
parameter NTT_st_MUL_s1_u1     = 5'd22; // Dec
parameter NTT_st_ADD_su        = 5'd23; // Dec
parameter NTT_st_INVNTT_su     = 5'd24; // Dec
parameter NTT_st_SUB_v_su      = 5'd25; // Dec

// coder_state define
parameter coder_st_INTT = 4'd0;
parameter coder_st_encode_pk = 4'd1; // KeyGen
parameter coder_st_encode_sk = 4'd2; // KeyGen
parameter coder_st_decode_pk = 4'd3; // Enc
parameter coder_st_decode_m = 4'd4; // Enc
parameter coder_st_encode_c = 4'd5; // Enc
parameter coder_st_decode_sk = 4'd6; // Dec
parameter coder_st_decode_c = 4'd7; // Dec
parameter coder_st_encode_m = 4'd8; // Dec
parameter coder_st_INTT_2 = 4'd9;
parameter coder_st_INTT_3 = 4'd10;
parameter coder_st_INTT_4 = 4'd11;

// H_state define
parameter H_st_INTT = 3'd0;
parameter H_st_INTT_2 = 3'd6;

parameter H_st_KeyGen_pk = 3'd1;
parameter H_st_Enc_m = 3'd2;
parameter H_st_Enc_pk = 3'd3;
parameter H_st_Enc_c = 3'd4;
parameter H_st_Dec_c = 3'd5;

// G_state define
parameter G_st_INTT = 2'd0;
parameter G_st_KeyGen_d = 2'd1;
parameter G_st_Enc_Kr = 2'd2;
parameter G_st_Dec_Kr = 2'd3;

//KDF_state define
parameter KDF_st_INTT = 2'd0;
parameter KDF_st_Enc_K = 2'd1;
parameter KDF_st_Dec_K = 2'd2;

//DO define
parameter DO_st_INTT = 5'd0;
parameter DO_st_DEC_PRENTT = 5'd1;
parameter DO_st_DEC_POSTNTT = 5'd2;
parameter DO_st_INTT_2 = 5'd3;
parameter DO_st_INTT_3 = 5'd4;
parameter DO_st_DEC_POSTINTT = 5'd5;
parameter DO_st_INTT_4 = 5'd6;
parameter DO_st_ENC_A0_POSTNTT = 5'd7;
parameter DO_st_ENC_A1_POSTNTT = 5'd8;
parameter DO_st_ENC_E10_POSTINTT = 5'd9;
parameter DO_st_ENC_T_PRENTT = 5'd10;
parameter DO_st_INTT_5 = 5'd11;
parameter DO_st_ENC_U0_POSTINTT = 5'd12;
parameter DO_st_ENC_U1_POSTINTT = 5'd13;
parameter DO_st_ENC_V_POSTINTT = 5'd14;
parameter DO_st_ENC_E11_POSTINTT = 5'd15;
parameter DO_st_ENC_E2_POSTINTT = 5'd16;
parameter DO_st_KG_A0_POSTNTT = 5'd17;
parameter DO_st_KG_A1_POSTNTT = 5'd18;
parameter DO_st_KG_S_POSTNTT = 5'd19;
parameter DO_st_KG_T_POSTNTT = 5'd20;

// ram offset define
parameter ram_0_offset = 10'd0;
parameter ram_1_offset = 10'd32;
parameter ram_2_offset = 10'd64;
parameter ram_3_offset = 10'd96;
parameter ram_4_offset = 10'd128;
parameter ram_5_offset = 10'd160;
parameter ram_6_offset = 10'd192;
parameter ram_7_offset = 10'd224;
parameter ram_8_offset = 10'd256;
parameter ram_9_offset = 10'd288;
parameter ram_10_offset = 10'd320;
parameter ram_11_offset = 10'd352;
parameter ram_12_offset = 10'd384;
parameter ram_13_offset = 10'd416;
parameter ram_14_offset = 10'd448;
parameter ram_15_offset = 10'd480;
parameter ram_16_offset = 10'd512;
parameter ram_17_offset = 10'd544;


// coder mode define
parameter coder_mode_KeyGen_encode_sk = 4'd1;
parameter coder_mode_KeyGen_encode_pk = 4'd2;
parameter coder_mode_Enc_decode_pk = 4'd3;
parameter coder_mode_Enc_decode_m = 4'd4;
parameter coder_mode_Enc_encode_c = 4'd5;
parameter coder_mode_Dec_decode_sk = 4'd6;
parameter coder_mode_Dec_decode_c = 4'd7;
parameter coder_mode_Dec_encode_m = 4'd8;

// H_mode define
parameter H_mode_M = 2'd0;
parameter H_mode_PK = 2'd1;
parameter H_mode_C = 2'd2;

//G mode define
parameter G_mode_d = 1'd0;
parameter G_mode_Kr = 1'd1;

//KDF mode define
parameter KDF_mode_PRF_eta3 = 2'd1;
parameter KDF_mode_PRF_eta2 = 2'd2;
parameter KDF_mode_KDF = 2'd3;

//DO mode define
parameter INIT = 3'd0;
parameter PRENTT = 3'd1;
parameter POSTNTT = 3'd2;
parameter PREINTT = 3'd3;
parameter POSTINTT = 3'd4;

// ram_r_sel define
parameter ram_r_from_coder = 1'd0;
parameter ram_r_from_ntt = 1'd1;

// ram_w_sel define
parameter ram_w_from_coder = 2'd0;
parameter ram_w_from_ntt = 2'd1;
parameter ram_w_from_A_gen = 2'd2;
parameter ram_w_from_CBD = 2'd3;

integer i,j;

assign sk_out = {sk_out_cpa,pk_out,H_pk,random_coin};
assign finish = state_reg == FINISH;
assign K_out = KDF_K;
assign coder_pk_in = (state_reg == Dec) ? pk_from_sk : pk_in;
// assign coder_m_in = (state_reg == Dec) ? m_out_cpa : H_m;
assign coder_m_in = (state_reg == Dec) ? m_out_cpa : m_in;
assign m_out = (CODER_ENCODE_m_finish) ? m_out_cpa : m_out;

//A采样控制
always @(*) begin
    case(state_reg)
    KeyGen , Enc , Dec : begin
        case(A_gen_state)
        A_gen_st_INTT: begin
            rho = (state_reg == KeyGen ? G_rho : rho_from_pk);
            A_gen_nstate = (state_reg == KeyGen ? G_finish_reg : CODER_DECODE_pkm_finish) ? A_gen_st_A00 : A_gen_st_INTT;
        end
        A_gen_st_A00: begin
            ram_w_sel = ram_w_from_A_gen;
            A_gen_ram_w_start_offset = ram_0_offset;
            A_in = {rho,8'd0,8'd0};
            A_gen_active = ~A_finsh_reg;

            A_gen_rst = A_finsh_reg;
            A_gen_nstate =  A_finsh_reg ? A_gen_st_A01 : A_gen_st_A00;
            A_gen_finish[0][0] = A_finsh_reg;
        end
        A_gen_st_A01: begin

            A_gen_ram_w_start_offset = ram_1_offset;
            A_in = {rho,8'd0,8'd1};
            A_gen_active = ~A_finsh_reg;

            A_gen_rst = A_finsh_reg;
            A_gen_nstate =  A_finsh_reg ? A_gen_st_A10 : A_gen_st_A01;
            A_gen_finish[0][1] = A_finsh_reg;
        end
        A_gen_st_A10: begin

            A_gen_ram_w_start_offset = ram_2_offset;
            A_in = {rho,8'd1,8'd0};
            A_gen_active = ~A_finsh_reg;

            A_gen_rst = A_finsh_reg;
            A_gen_nstate =  A_finsh_reg ? A_gen_st_A11 : A_gen_st_A10;
            A_gen_finish[1][0] = A_finsh_reg;
        end
        A_gen_st_A11: begin

            A_gen_ram_w_start_offset = ram_3_offset;
            A_in = {rho,8'd1,8'd1};
            A_gen_active = ~A_finsh_reg;

            A_gen_rst = A_finsh_reg;
            A_gen_nstate =  A_finsh_reg ? A_gen_st_INTT : A_gen_st_A11;
            A_gen_finish[1][1] = A_finsh_reg;
            A_SAMPLE_finish = A_finsh_reg;
        end
        endcase    
    end
    endcase
end
//CBD采样控制
always @(*) begin
    case(state_reg)
    KeyGen : begin
        case(CBD_state)
            CBD_st_INTT: begin
                CBD_nstate = A_SAMPLE_finish ? CBD_st_s0 : CBD_st_INTT;
            end
            CBD_st_s0: begin
                ram_w_sel = ram_w_from_CBD;
                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_4_offset;
                CBD_input = {G_sigma,8'd0};
                CBD_active = ~CBD_finsh_reg;


                CBD_rst = CBD_finsh_reg;
                CBD_nstate = CBD_finsh_reg ? CBD_st_s1 : CBD_st_s0;
                CBD_S_finish[0] = CBD_finsh_reg;
            end
            CBD_st_s1: begin

                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_5_offset;
                CBD_input = {G_sigma,8'd1};
                CBD_active = ~CBD_finsh_reg;


                CBD_rst = CBD_finsh_reg;
                CBD_nstate = CBD_finsh_reg ? CBD_st_e0 : CBD_st_s1;
                CBD_S_finish[1] = CBD_finsh_reg;
            end
            CBD_st_e0: begin
                
                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_6_offset;
                CBD_input = {G_sigma,8'd2};
                CBD_active = ~CBD_finsh_reg;

                CBD_rst = CBD_finsh_reg;                
                CBD_nstate = CBD_finsh_reg ? CBD_st_e1 : CBD_st_e0;
                CBD_E_finish[0] = CBD_finsh_reg;
            end
            CBD_st_e1: begin
                
                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_7_offset;
                CBD_input = {G_sigma,8'd3};
                CBD_active = ~CBD_finsh_reg;

                CBD_rst = CBD_finsh_reg;               
                CBD_nstate = CBD_finsh_reg ? CBD_st_INTT : CBD_st_e1;
                CBD_E_finish[1] = CBD_finsh_reg;
                CBD_SAMPLE_finish = CBD_finsh_reg;
            end
        endcase
    end
    Enc , Dec : begin
        case(CBD_state)
            CBD_st_INTT: begin
                CBD_nstate = A_SAMPLE_finish ? CBD_st_r0 : CBD_st_INTT;
            end
            CBD_st_r0: begin
                ram_w_sel = ram_w_from_CBD;
                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_4_offset;
                CBD_input = {G_r,8'd0};
                CBD_active = ~CBD_finsh_reg;


                CBD_rst = CBD_finsh_reg;
                CBD_nstate = CBD_finsh_reg ? CBD_st_r1 : CBD_st_r0;
                CBD_R_finish[0] = CBD_finsh_reg;
            end
            CBD_st_r1: begin

                CBD_num = (kyber_mode == kyber512) ? 2'd1 : 2'd2;
                CBD_ram_w_start_offset = ram_5_offset;
                CBD_input = {G_r,8'd1};
                CBD_active = ~CBD_finsh_reg;


                CBD_rst = CBD_finsh_reg;
                CBD_nstate = CBD_finsh_reg ? CBD_st_e10 : CBD_st_r1;
                CBD_R_finish[1] = CBD_finsh_reg;
            end
            CBD_st_e10: begin
                
                CBD_num = 2'd2;
                CBD_ram_w_start_offset = ram_6_offset;
                CBD_input = {G_r,8'd2};
                CBD_active = ~CBD_finsh_reg;

                CBD_rst = CBD_finsh_reg;                
                CBD_nstate = CBD_finsh_reg ? CBD_st_e11 : CBD_st_e10;
                CBD_E1_finish[0] = CBD_finsh_reg;
            end
            CBD_st_e11: begin
                
                CBD_num = 2'd2;
                CBD_ram_w_start_offset = ram_7_offset;
                CBD_input = {G_r,8'd3};
                CBD_active = ~CBD_finsh_reg;

                CBD_rst = CBD_finsh_reg;                
                CBD_nstate = CBD_finsh_reg ? CBD_st_e2 : CBD_st_e11;
                CBD_E1_finish[1] = CBD_finsh_reg;
            end
            CBD_st_e2: begin
                
                CBD_num = 2'd2;
                CBD_ram_w_start_offset = ram_8_offset;
                CBD_input = {G_r,8'd4};
                CBD_active = ~CBD_finsh_reg;

                CBD_rst = CBD_finsh_reg;               
                CBD_nstate = CBD_finsh_reg ? CBD_st_INTT : CBD_st_e2;
                CBD_E2_finish = CBD_finsh_reg;
                CBD_SAMPLE_finish = CBD_finsh_reg;
            end
        endcase
    end
    endcase
end
//NTT模块控制
always @(*) begin
    case(state_reg)
    KeyGen : begin
        case(ntt_state)
            NTT_st_INTT: begin
                ntt_is_add_or_sub = 1'b0;
                ntt_nstate = /*CBD_S_finish[0]*/CBD_SAMPLE_finish ? NTT_st_NTT_s0 : NTT_st_INTT;
            end
            NTT_st_NTT_s0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_4_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_4_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && CBD_S_finish[1]) ? NTT_st_NTT_s1 : NTT_st_NTT_s0;
            end
            NTT_st_NTT_s1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_5_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_5_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && A_gen_finish[0][0]) ? NTT_st_NTT_e0 : NTT_st_NTT_s1;
                NTT_S_finish[1] = NTT_finish_reg;
            end
            NTT_st_NTT_e0:begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_6_offset;
                ntt_ram_r_start_offset_B = ram_6_offset;
                ntt_ram_w_start_offset = ram_6_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg) ? NTT_st_NTT_e1 : NTT_st_NTT_e0;                
            end
            NTT_st_NTT_e1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_7_offset;
                ntt_ram_r_start_offset_B = ram_7_offset;
                ntt_ram_w_start_offset = ram_7_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg) ? NTT_st_INTT_2 : NTT_st_NTT_e1;
                NTT_SE_finish = NTT_finish_reg;
            end
            NTT_st_INTT_2: begin
                ntt_nstate =  DO_KG_A_POSTNTT_finish ? NTT_st_MUL_A00_s0 : NTT_st_INTT_2;
            end
            NTT_st_MUL_A00_s0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && A_gen_finish[0][1]) ? NTT_st_MUL_A10_s1 : NTT_st_MUL_A00_s0;
            end
            NTT_st_MUL_A10_s1:begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_2_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_2_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && CBD_E_finish[0]) ? NTT_st_ADD_t0 : NTT_st_MUL_A10_s1;
            end
            NTT_st_ADD_t0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_2_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg) ? NTT_st_ADD_e0 : NTT_st_ADD_t0;  
            end
            NTT_st_ADD_e0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_6_offset;
                ntt_ram_w_start_offset = ram_6_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && A_gen_finish[1][0]) ? NTT_st_MUL_A01_s0 : NTT_st_ADD_e0;
            end
            NTT_st_MUL_A01_s0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_1_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_1_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && A_gen_finish[1][1]) ? NTT_st_MUL_A11_s1 : NTT_st_MUL_A01_s0;
            end
            NTT_st_MUL_A11_s1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg && CBD_E_finish[1]) ? NTT_st_ADD_t1 : NTT_st_MUL_A11_s1;
            end
            NTT_st_ADD_t1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_1_offset;
                ntt_ram_r_start_offset_B = ram_3_offset;
                ntt_ram_w_start_offset = ram_1_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg) ? NTT_st_ADD_e1 : NTT_st_ADD_t1;
            end
            NTT_st_ADD_e1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_1_offset;
                ntt_ram_r_start_offset_B = ram_7_offset;
                ntt_ram_w_start_offset = ram_7_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = (NTT_finish_reg) ? NTT_st_INTT : NTT_st_ADD_e1;
                T_finish = NTT_finish_reg;
                NTT_PHASE_finish = NTT_finish_reg;
            end
        endcase
    end
    Enc : begin
        case(ntt_state)
            NTT_st_INTT: begin
                ntt_nstate = DO_ENC_PRENTT_finish ? NTT_st_NTT_r0 : NTT_st_INTT;
            end
            NTT_st_NTT_r0: begin
                ntt_is_add_or_sub = 1'b0;
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_4_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_4_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_NTT_r1 : NTT_st_NTT_r0;
            end
            NTT_st_NTT_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_5_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_5_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A00_r0 : NTT_st_NTT_r1;
            end
            NTT_st_MUL_A00_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A01_r1 : NTT_st_MUL_A00_r0;
            end
            NTT_st_MUL_A01_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_1_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_1_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_u0 : NTT_st_MUL_A01_r1;
            end
            NTT_st_ADD_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_1_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_u0 : NTT_st_ADD_u0;
            end
            NTT_st_INVNTT_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_0_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A10_r0 : NTT_st_INVNTT_u0;                
            end
            
            NTT_st_MUL_A10_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_2_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_2_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A11_r1 : NTT_st_MUL_A10_r0;
            end
            NTT_st_MUL_A11_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_u1 : NTT_st_MUL_A11_r1;
            end
            NTT_st_ADD_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_2_offset;
                ntt_ram_r_start_offset_B = ram_3_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_u1 : NTT_st_ADD_u1;
            end
            NTT_st_INVNTT_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_3_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_t0_r0 : NTT_st_INVNTT_u1;
            end
            NTT_st_MUL_t0_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_t1_r1 : NTT_st_MUL_t0_r0;
            end
            NTT_st_MUL_t1_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_10_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_10_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_v : NTT_st_MUL_t1_r1;
            end
            NTT_st_ADD_v: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_10_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_v : NTT_st_ADD_v;
            end
            NTT_st_INVNTT_v: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_9_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT_5 : NTT_st_INVNTT_v;
                NTT_ENC_INTT_finish = NTT_finish_reg;
            end
            NTT_st_INTT_5: begin
                ntt_nstate =  DO_ENC_POSTINTT_finish ? NTT_st_ADD_e10 : NTT_st_INTT_5;
            end
            NTT_st_ADD_e10: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_6_offset;
                ntt_ram_w_start_offset = ram_6_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_e11 : NTT_st_ADD_e10;  
            end
            NTT_st_ADD_e11: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_7_offset;
                ntt_ram_w_start_offset = ram_7_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_e2 : NTT_st_ADD_e11;
            end
            NTT_st_ADD_e2: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_8_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_m : NTT_st_ADD_e2;
            end
            NTT_st_ADD_m: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_11_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT : NTT_st_ADD_m;
                NTT_PHASE_finish = NTT_finish_reg;
            end
        endcase
    end
    Dec : begin
        case(ntt_state)
            NTT_st_INTT: begin
                ntt_is_add_or_sub = 1'b0;
                ntt_nstate = DO_DEC_PRENTT_finish ? NTT_st_NTT_u0 : NTT_st_INTT;
            end
            NTT_st_NTT_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_12_offset;
                ntt_ram_r_start_offset_B = ram_12_offset;
                ntt_ram_w_start_offset = ram_12_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_NTT_u1 : NTT_st_NTT_u0;
            end
            NTT_st_NTT_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_13_offset;
                ntt_ram_r_start_offset_B = ram_13_offset;
                ntt_ram_w_start_offset = ram_13_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT_3 : NTT_st_NTT_u1;
                NTT_U_finish = NTT_finish_reg;
            end
            NTT_st_INTT_3: begin
                ntt_nstate = DO_DEC_POSTNTT_finish ? NTT_st_MUL_s0_u0 : NTT_st_INTT_3;
            end
            NTT_st_MUL_s0_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_12_offset;
                ntt_ram_r_start_offset_B = ram_15_offset;
                ntt_ram_w_start_offset = ram_12_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_s1_u1 : NTT_st_MUL_s0_u0;
            end
            NTT_st_MUL_s1_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_13_offset;
                ntt_ram_r_start_offset_B = ram_16_offset;
                ntt_ram_w_start_offset = ram_13_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_su : NTT_st_MUL_s1_u1;
            end
            NTT_st_ADD_su: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_12_offset;
                ntt_ram_r_start_offset_B = ram_13_offset;
                ntt_ram_w_start_offset = ram_12_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_su : NTT_st_ADD_su;
            end
            NTT_st_INVNTT_su: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_12_offset;
                ntt_ram_r_start_offset_B = ram_12_offset;
                ntt_ram_w_start_offset = ram_12_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT_4 : NTT_st_INVNTT_su;
                INTT_SU_finish = NTT_finish_reg;
            end
            NTT_st_INTT_4: begin
                ntt_nstate = DO_DEC_POSTINTT_finish ? NTT_st_SUB_v_su : NTT_st_INTT_4;
            end
            NTT_st_SUB_v_su: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_is_add_or_sub = 1'b1;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_14_offset;
                ntt_ram_r_start_offset_B = ram_12_offset;
                ntt_ram_w_start_offset = ram_12_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT_2 : NTT_st_SUB_v_su;
                NTT_deccpa_finish = NTT_finish_reg;
            end
            NTT_st_INTT_2: begin//中间加一个等待状态以避免sel信号的混乱
                ntt_nstate = DO_ENC_PRENTT_finish ? NTT_st_NTT_r0 : NTT_st_INTT_2;
            end
            NTT_st_NTT_r0: begin
                ntt_is_add_or_sub = 1'b0;
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_4_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_4_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_NTT_r1 : NTT_st_NTT_r0;
            end
            NTT_st_NTT_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd0;
                ntt_ram_r_start_offset_A = ram_5_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_5_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A00_r0 : NTT_st_NTT_r1;
            end
            NTT_st_MUL_A00_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A01_r1 : NTT_st_MUL_A00_r0;
            end
            NTT_st_MUL_A01_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_1_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_1_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_u0 : NTT_st_MUL_A01_r1;
            end
            NTT_st_ADD_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_1_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_u0 : NTT_st_ADD_u0;
            end
            NTT_st_INVNTT_u0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_0_offset;
                ntt_ram_w_start_offset = ram_0_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A10_r0 : NTT_st_INVNTT_u0;
            end
            
            NTT_st_MUL_A10_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_2_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_2_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_A11_r1 : NTT_st_MUL_A10_r0;
            end
            NTT_st_MUL_A11_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_u1 : NTT_st_MUL_A11_r1;
            end
            NTT_st_ADD_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_2_offset;
                ntt_ram_r_start_offset_B = ram_3_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_u1 : NTT_st_ADD_u1;
            end
            NTT_st_INVNTT_u1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_3_offset;
                ntt_ram_w_start_offset = ram_3_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_t0_r0 : NTT_st_INVNTT_u1;
            end
            
            NTT_st_MUL_t0_r0: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_4_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_MUL_t1_r1 : NTT_st_MUL_t0_r0;
            end
            NTT_st_MUL_t1_r1: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd2;
                ntt_ram_r_start_offset_A = ram_10_offset;
                ntt_ram_r_start_offset_B = ram_5_offset;
                ntt_ram_w_start_offset = ram_10_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_v : NTT_st_MUL_t1_r1;
            end
            NTT_st_ADD_v: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_10_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INVNTT_v : NTT_st_ADD_v;
            end
            NTT_st_INVNTT_v: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd1;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_9_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT_5 : NTT_st_INVNTT_v;
                NTT_ENC_INTT_finish = NTT_finish_reg;
            end
            NTT_st_INTT_5: begin
                ntt_nstate =  DO_ENC_POSTINTT_finish ? NTT_st_ADD_e10 : NTT_st_INTT_5;
            end
            NTT_st_ADD_e10: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_0_offset;
                ntt_ram_r_start_offset_B = ram_6_offset;
                ntt_ram_w_start_offset = ram_6_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_e11 : NTT_st_ADD_e10;  
            end
            NTT_st_ADD_e11: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_3_offset;
                ntt_ram_r_start_offset_B = ram_7_offset;
                ntt_ram_w_start_offset = ram_7_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_e2 : NTT_st_ADD_e11;
            end
            NTT_st_ADD_e2: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_8_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;

                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_ADD_m : NTT_st_ADD_e2;
            end
            NTT_st_ADD_m: begin
                ram_r_sel = ram_r_from_ntt;
                ram_w_sel = ram_w_from_ntt;
                ntt_mode = 2'd3;
                ntt_ram_r_start_offset_A = ram_9_offset;
                ntt_ram_r_start_offset_B = ram_11_offset;
                ntt_ram_w_start_offset = ram_9_offset;
                ntt_start = ~NTT_finish_reg;


                NTT_rst = NTT_finish_reg;
                ntt_nstate = NTT_finish_reg ? NTT_st_INTT : NTT_st_ADD_m;
                NTT_PHASE_finish = NTT_finish_reg;
            end
        endcase
    end
    endcase
end
//CODER控制模块
always @(NTT_PHASE_finish,CODER_finish_reg,G_finish_reg,start_reg,NTT_deccpa_finish,NTT_ENC_INTT_finish,DO_KG_finish) begin
    case(state_reg)
    KeyGen : begin
        case(coder_state)
            coder_st_INTT:begin
                coder_load_input_Dec = 1'b0;
                coder_load_input_Enc = 1'b0;

                coder_nstate = (DO_KG_finish) ? coder_st_encode_pk : coder_st_INTT;
            end
            coder_st_encode_pk:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_KeyGen_encode_pk;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_encode_sk : coder_st_encode_pk;
            end
            coder_st_encode_sk:begin
                coder_mode = coder_mode_KeyGen_encode_sk;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_INTT : coder_st_encode_sk;
                COODER_PHASE_finish = CODER_finish_reg;
            end
        endcase
    end
    Enc : begin
        case(coder_state)
            coder_st_INTT:begin
                coder_load_input_Dec = 1'b0;
                coder_load_input_Enc = 1'b1;

                coder_nstate = (G_finish_reg) ? coder_st_decode_pk : coder_st_INTT;
            end
            coder_st_decode_pk:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Enc_decode_pk;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_decode_m : coder_st_decode_pk;
            end
            coder_st_decode_m:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Enc_decode_m;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_INTT_2 : coder_st_decode_m;
                CODER_DECODE_pkm_finish = CODER_finish_reg;
            end
            coder_st_INTT_2:begin
                coder_nstate = NTT_PHASE_finish ? coder_st_encode_c : coder_st_INTT_2;
            end
            coder_st_encode_c:begin
                coder_load_input_Enc = 1'b0;
                ram_r_sel = ram_r_from_coder;
                coder_mode = coder_mode_Enc_encode_c;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_INTT : coder_st_encode_c;
                c_out_reg = c_out;
                COODER_PHASE_finish = CODER_finish_reg;
            end
        endcase
    end
    Dec : begin
        case(coder_state)
            coder_st_INTT:begin
                coder_load_input_Dec = 1'b1;
                coder_load_input_Enc = 1'b0;

                coder_nstate = start_reg ? coder_st_decode_c : coder_st_INTT;
            end
            coder_st_decode_c:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Dec_decode_c;
                coder_active = ~CODER_finish_reg;

                coder_nstate = CODER_finish_reg ? coder_st_decode_sk : coder_st_decode_c;
            end
            coder_st_decode_sk:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Dec_decode_sk;
                coder_active = ~CODER_finish_reg;

                coder_nstate = CODER_finish_reg ? coder_st_INTT_2 : coder_st_decode_sk;
                CODER_DECODE_csk_finish = CODER_finish_reg;
            end
            coder_st_INTT_2:begin
                coder_nstate = NTT_deccpa_finish ? coder_st_encode_m : coder_st_INTT_2;
            end
            coder_st_encode_m:begin
                coder_load_input_Dec = 1'b0;
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Dec_encode_m;
                coder_active = ~CODER_finish_reg;

                coder_nstate = CODER_finish_reg ? coder_st_INTT_3 : coder_st_encode_m;
                CODER_ENCODE_m_finish = CODER_finish_reg;
            end
            coder_st_INTT_3:begin
                coder_nstate = G_finish_reg ? coder_st_decode_pk : coder_st_INTT_3;
            end
            coder_st_decode_pk:begin
                coder_load_input_Enc = 1'b1;
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Enc_decode_pk;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_decode_m : coder_st_decode_pk;
            end
            coder_st_decode_m:begin
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Enc_decode_m;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_INTT_4 : coder_st_decode_m;
                CODER_DECODE_pkm_finish = CODER_finish_reg;
            end
            coder_st_INTT_4:begin
                coder_nstate = NTT_PHASE_finish ? coder_st_encode_c : coder_st_INTT_4;
            end
            coder_st_encode_c:begin
                coder_load_input_Enc = 1'b0;
                ram_r_sel = ram_r_from_coder;
                ram_w_sel = ram_w_from_coder;
                coder_mode = coder_mode_Enc_encode_c;
                coder_active = ~CODER_finish_reg;

                coder_nstate = (CODER_finish_reg) ? coder_st_INTT : coder_st_encode_c;
                COODER_PHASE_finish = CODER_finish_reg;


            end
        endcase
    end
    endcase
end
//H
always@(state_reg,H_state,COODER_PHASE_finish,H_finish_reg)begin//防止H_out的变化导致误触发，因此详细列出了期望触发的信号。同时也解决了由于模块复位时清输出导致输出不能及时送入寄存器的问题
    case(state_reg)
        KeyGen : begin
            case(H_state)
                H_st_INTT: begin
                    H_nstate = COODER_PHASE_finish ? H_st_KeyGen_pk : H_st_INTT;
                end
                H_st_KeyGen_pk: begin
                    H_in = pk_out;
                    H_mode = H_mode_PK;
                    H_active = ~H_finish_reg;

                    H_pk = H_finish_reg ? H_out : H_pk;
                    H_rst = H_finish_reg;
                    H_nstate = H_finish_reg ? H_st_INTT : H_st_KeyGen_pk;
                    state_reg =  H_finish_reg ? FINISH : mode;
                end
            endcase
        end
        Enc : begin
            case(H_state)
                H_st_INTT: begin
                    H_nstate = start_reg ? H_st_Enc_pk : H_st_INTT;
                end
                H_st_Enc_m: begin
                    H_in = m_in;
                    H_mode = H_mode_M;
                    H_active = ~H_finish_reg;

                    H_m = H_finish_reg ? H_out : H_m;
                    H_rst = H_finish_reg;
                    H_nstate = H_finish_reg ? H_st_Enc_pk : H_st_Enc_m;
                end
                H_st_Enc_pk: begin
                    H_in = pk_in;
                    H_mode = H_mode_PK;
                    H_active = ~H_finish_reg;

                    H_pk = H_finish_reg ? H_out : H_pk;
                    H_rst = H_finish_reg;
                    H_nstate = H_finish_reg ? H_st_INTT_2 : H_st_Enc_pk;
                    H_pk_finish = H_finish_reg;
                end
                H_st_INTT_2: begin
                    H_nstate = COODER_PHASE_finish ? H_st_Enc_c : H_st_INTT_2;
                end
                H_st_Enc_c: begin
                    H_in = c_out;
                    H_mode = H_mode_C;
                    H_active = ~H_finish_reg;

                    H_c = H_finish_reg ? H_out : H_c;
                    H_rst = H_finish_reg;
                    H_nstate = H_finish_reg ? H_st_INTT : H_st_Enc_c;
                    H_c_finish = H_finish_reg;
                end
            endcase
        end
        Dec : begin
            case(H_state)
                H_st_INTT: begin
                    H_nstate = COODER_PHASE_finish ? H_st_Dec_c : H_st_INTT;
                end
                H_st_Dec_c: begin
                    H_in = c_in;
                    H_mode = H_mode_C;
                    H_active = ~H_finish_reg;

                    H_c = H_finish_reg ? H_out : H_c;
                    H_rst = H_finish_reg;
                    H_nstate = H_finish_reg ? H_st_INTT : H_st_Dec_c;
                end
            endcase
        end
    endcase
end
//G
always@(state_reg,G_state,G_finish_reg,H_pk_finish,COODER_PHASE_finish,CODER_ENCODE_m_finish)begin
    case(state_reg)
        KeyGen : begin
            case(G_state)
                G_st_INTT: begin
                    G_nstate = start_reg ? G_st_KeyGen_d : G_st_INTT;
                end
                G_st_KeyGen_d: begin
                    G_in = random_coin;
                    G_mode = G_mode_d;
                    G_active = ~G_finish_reg;

                    {G_rho,G_sigma} = G_finish_reg ? G_out : {G_rho,G_sigma};
                    G_rst = G_finish_reg;
                    G_nstate = G_finish_reg ? G_st_INTT : G_st_KeyGen_d;
                end
            endcase
        end
        Enc : begin
            case(G_state)
                G_st_INTT: begin
                    G_nstate = H_pk_finish ? G_st_Enc_Kr : G_st_INTT;
                end
                G_st_Enc_Kr: begin
                    // G_in = {H_m,H_pk};
                    G_in = {m_in,H_pk};
                    G_mode = G_mode_Kr;
                    G_active = ~G_finish_reg;

                    {G_k,G_r} = G_finish_reg ? G_out : {G_k,G_r};
                    G_rst = G_finish_reg;
                    G_nstate = G_finish_reg ? G_st_INTT : G_st_Enc_Kr;

                end
            endcase
        end
        Dec : begin
            case(G_state)
                G_st_INTT: begin
                    G_nstate = CODER_ENCODE_m_finish ? G_st_Dec_Kr : G_st_INTT;
                end
                G_st_Dec_Kr: begin
                    G_in = {m_out_cpa,sk_in[511:256]};
                    G_mode = G_mode_Kr;
                    G_active = ~G_finish_reg;

                    {G_k,G_r} = G_finish_reg ? G_out : {G_k,G_r};
                    G_rst = G_finish_reg;
                    G_nstate = G_finish_reg ? G_st_INTT : G_st_Dec_Kr;

                end
            endcase
        end
    endcase
end
//KDF
always@(state_reg,KDF_state,H_c_finish,KDF_finish_reg,H_finish_reg)begin
    case(state_reg)
        Enc : begin
            case(KDF_state)
                KDF_st_INTT: begin
                    KDF_nstate = H_c_finish ? KDF_st_Enc_K : KDF_st_INTT;
                end
                KDF_st_Enc_K: begin
                    KDF_in = {G_k,H_c};
                    KDF_mode = KDF_mode_KDF;
                    KDF_active = ~KDF_finish_reg;

                    KDF_K = KDF_finish_reg ? KDF_out[255:0] : KDF_K;
                    KDF_rst = KDF_finish_reg;
                    KDF_nstate = KDF_finish_reg ? KDF_st_INTT : KDF_st_Enc_K;
                    state_reg =  KDF_finish_reg ? FINISH : mode;
                end
            endcase
        end
        Dec : begin
            case(KDF_state)
                KDF_st_INTT: begin
                    KDF_nstate = H_finish_reg ? KDF_st_Dec_K : KDF_st_INTT;
                end
                KDF_st_Dec_K: begin
                    KDF_in = (c_in == c_out) ? {G_k,H_c} : {sk_in[255:0],H_c};
                    KDF_mode = KDF_mode_KDF;
                    KDF_active = ~KDF_finish_reg;

                    KDF_K = KDF_finish_reg ? KDF_out[255:0] : KDF_K;
                    KDF_rst = KDF_finish_reg;
                    KDF_nstate = KDF_finish_reg ? KDF_st_INTT : KDF_st_Dec_K;
                    state_reg =  KDF_finish_reg ? FINISH : mode;
                end
            endcase
        end
    endcase
end
//DO
always@(state_reg,DO_state,CODER_DECODE_csk_finish,DO_finish_reg,NTT_U_finish,INTT_SU_finish,CBD_SAMPLE_finish,NTT_ENC_INTT_finish,NTT_SE_finish,NTT_PHASE_finish)begin
    case(state_reg)
        KeyGen: begin
            case(DO_state)
                DO_st_INTT: begin
                    DO_nstate = NTT_SE_finish ? DO_st_KG_A0_POSTNTT : DO_st_INTT;
                end
                DO_st_KG_A0_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_0_offset;
                    DO_ram_w_offset = ram_0_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_KG_A1_POSTNTT : DO_st_KG_A0_POSTNTT;
                end
                DO_st_KG_A1_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_2_offset;
                    DO_ram_w_offset = ram_2_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_2 : DO_st_KG_A1_POSTNTT;
                    DO_KG_A_POSTNTT_finish = DO_finish_reg;
                end
                DO_st_INTT_2 : begin
                    DO_nstate = NTT_PHASE_finish ? DO_st_KG_S_POSTNTT : DO_st_INTT_2;
                end
                DO_st_KG_S_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_4_offset;
                    DO_ram_w_offset = ram_4_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_KG_T_POSTNTT : DO_st_KG_S_POSTNTT;
                end
                DO_st_KG_T_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_6_offset;
                    DO_ram_w_offset = ram_6_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT : DO_st_KG_T_POSTNTT;
                    DO_KG_finish = DO_finish_reg;
                end
            endcase
        end
        Enc: begin
            case(DO_state)
                DO_st_INTT: begin
                    DO_nstate = CBD_SAMPLE_finish ? DO_st_ENC_A0_POSTNTT : DO_st_INTT;
                end
                DO_st_ENC_A0_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_0_offset;
                    DO_ram_w_offset = ram_0_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_A1_POSTNTT : DO_st_ENC_A0_POSTNTT;
                end
                DO_st_ENC_A1_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_2_offset;
                    DO_ram_w_offset = ram_2_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E10_POSTINTT : DO_st_ENC_A1_POSTNTT;
                end
                DO_st_ENC_E10_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_6_offset;
                    DO_ram_w_offset = ram_6_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E11_POSTINTT : DO_st_ENC_E10_POSTINTT;
                end
                DO_st_ENC_E11_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_7_offset;
                    DO_ram_w_offset = ram_7_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E2_POSTINTT : DO_st_ENC_E11_POSTINTT;
                end
                DO_st_ENC_E2_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_8_offset;
                    DO_ram_w_offset = ram_8_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_T_PRENTT : DO_st_ENC_E2_POSTINTT;
                end
                DO_st_ENC_T_PRENTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_9_offset;
                    DO_ram_w_offset = ram_9_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_5 : DO_st_ENC_T_PRENTT;
                    DO_ENC_PRENTT_finish = DO_finish_reg;
                end
                DO_st_INTT_5: begin
                    DO_nstate = NTT_ENC_INTT_finish ? DO_st_ENC_U0_POSTINTT : DO_st_INTT_5;
                end
                DO_st_ENC_U0_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_0_offset;
                    DO_ram_w_offset = ram_0_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_U1_POSTINTT : DO_st_ENC_U0_POSTINTT;
                end
                DO_st_ENC_U1_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_3_offset;
                    DO_ram_w_offset = ram_3_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_V_POSTINTT : DO_st_ENC_U1_POSTINTT;
                end
                DO_st_ENC_V_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_9_offset;
                    DO_ram_w_offset = ram_9_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT : DO_st_ENC_V_POSTINTT;
                    DO_ENC_POSTINTT_finish = DO_finish_reg;
                end
            endcase
        end
        Dec: begin
            case(DO_state)
                DO_st_INTT: begin
                    DO_nstate = CODER_DECODE_csk_finish ? DO_st_DEC_PRENTT : DO_st_INTT;
                end
                DO_st_DEC_PRENTT : begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_12_offset;
                    DO_ram_w_offset = ram_12_offset;
                    DO_mode = PRENTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_2 : DO_st_DEC_PRENTT;
                    DO_DEC_PRENTT_finish = DO_finish_reg;
                end
                DO_st_INTT_2 : begin
                    DO_nstate = NTT_U_finish ? DO_st_DEC_POSTNTT : DO_st_INTT_2;
                end
                DO_st_DEC_POSTNTT : begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_15_offset;
                    DO_ram_w_offset = ram_15_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_3 : DO_st_DEC_POSTNTT;
                    DO_DEC_POSTNTT_finish = DO_finish_reg;
                end
                DO_st_INTT_3: begin
                    DO_nstate = INTT_SU_finish ? DO_st_DEC_POSTINTT : DO_st_INTT_3;
                end
                DO_st_DEC_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_12_offset;
                    DO_ram_w_offset = ram_12_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_4 : DO_st_DEC_POSTINTT;
                    DO_DEC_POSTINTT_finish = DO_finish_reg;
                end
                DO_st_INTT_4: begin
                    DO_nstate = CBD_SAMPLE_finish ? DO_st_ENC_A0_POSTNTT : DO_st_INTT_4;
                end
                DO_st_ENC_A0_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_0_offset;
                    DO_ram_w_offset = ram_0_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_A1_POSTNTT : DO_st_ENC_A0_POSTNTT;
                end
                DO_st_ENC_A1_POSTNTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_2_offset;
                    DO_ram_w_offset = ram_2_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E10_POSTINTT : DO_st_ENC_A1_POSTNTT;
                end
                DO_st_ENC_E10_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_6_offset;
                    DO_ram_w_offset = ram_6_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E11_POSTINTT : DO_st_ENC_E10_POSTINTT;
                end
                DO_st_ENC_E11_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_7_offset;
                    DO_ram_w_offset = ram_7_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_E2_POSTINTT : DO_st_ENC_E11_POSTINTT;
                end
                DO_st_ENC_E2_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_8_offset;
                    DO_ram_w_offset = ram_8_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_T_PRENTT : DO_st_ENC_E2_POSTINTT;
                end
                DO_st_ENC_T_PRENTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_9_offset;
                    DO_ram_w_offset = ram_9_offset;
                    DO_mode = POSTNTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT_5 : DO_st_ENC_T_PRENTT;
                    DO_ENC_PRENTT_finish = DO_finish_reg;
                end
                DO_st_INTT_5: begin
                    DO_nstate = NTT_ENC_INTT_finish ? DO_st_ENC_U0_POSTINTT : DO_st_INTT_5;
                end
                DO_st_ENC_U0_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_0_offset;
                    DO_ram_w_offset = ram_0_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_U1_POSTINTT : DO_st_ENC_U0_POSTINTT;
                end
                DO_st_ENC_U1_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_3_offset;
                    DO_ram_w_offset = ram_3_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_ENC_V_POSTINTT : DO_st_ENC_U1_POSTINTT;
                end
                DO_st_ENC_V_POSTINTT: begin
                    ram_r_sel = ram_r_from_coder;
                    ram_w_sel = ram_w_from_coder;
                    DO_ram_r_offset = ram_9_offset;
                    DO_ram_w_offset = ram_9_offset;
                    DO_mode = POSTINTT;
                    DO_active = ~DO_finish_reg;

                    DO_rst = DO_finish_reg;
                    DO_nstate = DO_finish_reg ? DO_st_INTT : DO_st_ENC_V_POSTINTT;
                    DO_ENC_POSTINTT_finish = DO_finish_reg;
                end
            endcase
        end
    endcase
end
//时序电路
always @(posedge clk or posedge rst) begin
    if(rst) begin
        state_reg <= FINISH;

        A_gen_rst <= 1'b1;
        CBD_rst <= 1'b1;
        NTT_rst <= 1'b1;
        coder_rst <= 1'b1;
        H_rst <= 1'b1;
        G_rst <= 1'b1;
        KDF_rst <= 1'b1;
        DO_rst <= 1'b1;
        
        A_gen_state <= A_gen_st_INTT;
        CBD_state <= CBD_st_INTT;
        ntt_state <= NTT_st_INTT;
        coder_state <= coder_st_INTT;
        H_state <= H_st_INTT;
        G_state <= G_st_INTT;
        KDF_state <= KDF_st_INTT;
        DO_state <= DO_st_INTT;

        A_gen_nstate <= A_gen_st_INTT;
        CBD_nstate <= CBD_st_INTT;
        ntt_nstate <= NTT_st_INTT;
        coder_nstate <= coder_st_INTT;
        H_nstate <= H_st_INTT;
        G_nstate <= G_st_INTT;
        KDF_nstate <= KDF_st_INTT;
        DO_nstate <= DO_st_INTT;

        A_gen_active <= 1'b0;
        CBD_active <= 1'b0;
        ntt_start <= 1'b0;
        coder_active <= 1'b0;
        H_active <= 1'b0;
        G_active <= 1'b0;
        KDF_active <= 1'b0;

        A_SAMPLE_finish <= 1'b0;
        CBD_SAMPLE_finish <= 1'b0;
        COODER_PHASE_finish <= 1'b0;
        NTT_PHASE_finish <= 1'b0;
        DO_DEC_PRENTT_finish <= 1'b0;
        DO_DEC_POSTNTT_finish <= 1'b0;
        DO_DEC_POSTINTT_finish <= 1'b0;
        DO_ENC_PRENTT_finish <= 1'b0;
        DO_ENC_POSTINTT_finish <= 1'b0;
        DO_POSTINTT_finish <= 1'b0;
        DO_KG_A_POSTNTT_finish <= 1'b0;
        DO_KG_finish <= 1'b0;
        NTT_U_finish <= 1'b0;
        NTT_SE_finish <= 1'b0;
        INTT_SU_finish <= 1'b0;


    for (i=0; i<4; i=i+1) begin
        for (j=0; j<4; j=j+1)
            A_gen_finish[i][j] <= 1'b0;
        CBD_S_finish[i] <= 1'b0;
        CBD_E_finish[i] <= 1'b0;
        CBD_R_finish[i] <= 1'b0;
        CBD_E1_finish[i] <= 1'b0;
        NTT_S_finish[i] <= 1'b0;
    end
        T_finish <= 1'b0;
        CBD_E2_finish <= 1'b0;
        CODER_DECODE_csk_finish <= 1'b0;
        CODER_DECODE_pkm_finish <= 1'b0;
        CODER_ENCODE_m_finish <= 1'b0;
        NTT_deccpa_finish <= 1'b0;
        NTT_ENC_INTT_finish <= 1'b0;
        H_pk_finish <= 1'b0;
        H_c_finish <= 1'b0;


    end else begin
        A_gen_state <= A_gen_nstate;
        CBD_state <= CBD_nstate;
        ntt_state <= ntt_nstate;
        coder_state <= coder_nstate;
        H_state <= H_nstate;
        G_state <= G_nstate;
        KDF_state <= KDF_nstate;
        DO_state <= DO_nstate;

        A_gen_active <= 1'b0;
        CBD_active <= 1'b0;
        ntt_start <= 1'b0;
        coder_active <= 1'b0;
        H_active <= 1'b0;
        G_active <= 1'b0;
        KDF_active <= 1'b0;
        DO_active <= 1'b0;

        A_gen_rst <= 1'b0;
        CBD_rst <= 1'b0;
        NTT_rst <= 1'b0;
        coder_rst <= 1'b0;
        H_rst <= 1'b0;
        G_rst <= 1'b0;
        KDF_rst <= 1'b0;

        A_finsh_reg <= A_finsh;
        CBD_finsh_reg <= CBD_finsh;
        CODER_finish_reg <= CODER_finish;
        NTT_finish_reg <= NTT_finish;
        H_finish_reg <= H_finish;
        G_finish_reg <= G_finish;
        KDF_finish_reg <= KDF_finish;
        DO_finish_reg <= DO_finish;

        A_SAMPLE_finish <= 1'b0;
        CBD_SAMPLE_finish <= 1'b0;
        COODER_PHASE_finish <= 1'b0;
        NTT_PHASE_finish <= 1'b0;
        DO_DEC_PRENTT_finish <= 1'b0;
        DO_DEC_POSTNTT_finish <= 1'b0;
        DO_DEC_POSTINTT_finish <= 1'b0;
        DO_ENC_PRENTT_finish <= 1'b0;
        DO_ENC_POSTINTT_finish <= 1'b0;
        DO_POSTINTT_finish <= 1'b0;
        DO_KG_A_POSTNTT_finish <= 1'b0;
        DO_KG_finish <= 1'b0;
        NTT_U_finish <= 1'b0;
        NTT_SE_finish <= 1'b0;
        INTT_SU_finish <= 1'b0;
        CODER_DECODE_csk_finish <= 1'b0;
        CODER_DECODE_pkm_finish <= 1'b0;
        CODER_ENCODE_m_finish <= 1'b0;
        NTT_deccpa_finish <= 1'b0;
        NTT_ENC_INTT_finish <= 1'b0;

        H_pk_finish <= 1'b0;
        H_c_finish <= 1'b0;

        start_reg <= start;


        if (state_reg == FINISH && start) state_reg <= mode;
    end

end
   
endmodule