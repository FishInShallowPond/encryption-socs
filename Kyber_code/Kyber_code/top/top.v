
`include"controller.v"
`include"ram_96x1024.v"
`include"../Hash/SHA3_512.v"
`include"../Hash/SHA3_256.v"
`include"../Hash/A_generator.v"
`include"../Hash/small_poly_generator.v"
`include"../NTT/ntt_processor.v"
`include"../coder/coder.v"

module top(
    input clk,
    input rst,
    input start,
    input [1:0] kyber_mode,
    input [1:0] mode,
    input [255:0] random_coin,

    input [255:0] m_in,
    input [6399:0] pk_in,// 12*256*2 + 256
    input [13055:0] sk_in,// 12*256*2
    input [6143:0] c_in, // 10*256*2 + 4*256

    output [255:0] m_out,
    output [6399:0] pk_out,// 12*256*2 + 256
    output [13055:0] sk_out,// 12*256*2
    output [6143:0] c_out_reg, // 10*256*2 + 4*256
    output [255:0] K_out,

    output finish
);


//controller
wire ntt_start, ntt_is_add_or_sub;
wire [1:0] ntt_mode;
wire [9:0]  ntt_ram_r_start_offset_A, ntt_ram_r_start_offset_B, ntt_ram_w_start_offset;

wire G_active, G_rst;

wire CBD_rst, CBD_active;
wire [1:0] CBD_num;
wire [9:0] CBD_ram_w_start_offset;

wire A_gen_rst, A_gen_active;
wire [9:0] A_gen_ram_w_start_offset;
wire [15:0] A_gen_diff;

wire coder_rst, coder_active, coder_load_input_Enc, coder_load_input_Dec;
wire [3:0] coder_mode;

wire ram_r_sel;
wire [1:0] ram_w_sel;

//G
wire G_finish;
wire [255:0] G_rho;
wire [511:0] G_in,G_out;



//A_gen
wire A_finsh;
wire A_wen;
wire [9:0] A_waddr;
wire [95:0] A_wdata;
wire [271:0] A_in;
reg [255:0] rho_temp;

//CBD
wire CBD_finsh;
wire CBD_wen;
wire [9:0] CBD_waddr;
wire [95:0] CBD_wdata; 

wire [263:0] CBD_input;

//ntt
wire NTT_finish;
wire ntt_wen, ntt_last_cycle;
wire [9:0] ntt_raddr, ntt_waddr;
wire [95:0] ntt_wdata;

//coder
wire CODER_finish;
wire [255:0] rho_from_pk;
wire coder_wen;
wire [9:0] coder_raddr, coder_waddr;
wire [95:0] coder_wdata;
wire [6143:0] sk_out_cpa;
wire [6399:0] pk_from_sk;
wire [255:0] m_out_cpa;
wire [6399:0] coder_pk_in;
wire [255:0] coder_m_in;
wire [6143:0] c_out;


//H
wire H_active;
wire H_rst;
wire [1:0] H_mode;
wire H_finish;
wire [255:0] H_out;
wire [12543:0] H_in;

//KDF
wire [511:0] KDF_in;
wire KDF_active,KDF_rst,KDF_finish;
wire [1:0] KDF_mode;
wire [1535:0] KDF_out;

//ram
wire [95:0] RAM_rdata_a, RAM_rdata_b;

reg [1:0] wen_temp_a,wen_temp_b;
reg [9:0] raddr_temp_a, raddr_temp_b, waddr_temp_a, waddr_temp_b;
reg [95:0] wdata_temp_a, wdata_temp_b;

//DO
wire DO_finish,DO_active,DO_wen,DO_rst;
wire [9:0] DO_ram_r_offset,DO_ram_w_offset;
wire [9:0] DO_raddr,DO_waddr;
wire [95:0] DO_wdata;
wire [2:0] DO_mode;


controller control(
    .clk(clk),
    .rst(rst),
    .start(start),
    .kyber_mode(kyber_mode),
    .mode(mode), //input
    .random_coin(random_coin),
    .finish(finish),
    .sk_in(sk_in),
    .sk_out_cpa(sk_out_cpa),
    .sk_out(sk_out),
    .m_in(m_in),
    .m_out_cpa(m_out_cpa),
    .pk_out(pk_out),
    .pk_in(pk_in),
    .c_in(c_in),
    .m_out(m_out),

    .ntt_start(ntt_start),
    .ntt_mode(ntt_mode),
    .ntt_is_add_or_sub(ntt_is_add_or_sub),
    .ntt_ram_r_start_offset_A(ntt_ram_r_start_offset_A),
    .ntt_ram_r_start_offset_B(ntt_ram_r_start_offset_B),
    .ntt_ram_w_start_offset(ntt_ram_w_start_offset),
    .NTT_finish(NTT_finish),
    .NTT_rst(NTT_rst),

    .G_in(G_in),
    .G_active(G_active),
    .G_rst(G_rst),
    .G_mode(G_mode),
    .G_finish(G_finish),
    .G_out(G_out),
    .G_rho(G_rho),

    .CBD_rst(CBD_rst),
    .CBD_active(CBD_active),
    .CBD_num(CBD_num),
    .CBD_ram_w_start_offset(CBD_ram_w_start_offset),
    .CBD_finsh(CBD_finsh),
    .CBD_input(CBD_input),

    .A_gen_rst(A_gen_rst),
    .A_gen_active(A_gen_active),
    .A_gen_ram_w_start_offset(A_gen_ram_w_start_offset),
    .A_gen_diff(A_gen_diff),
    .A_finsh(A_finsh),
    .A_in(A_in),

    .coder_rst(coder_rst),
    .coder_active(coder_active),
    .coder_load_input_Enc(coder_load_input_Enc),
    .coder_load_input_Dec(coder_load_input_Dec),
    .coder_mode(coder_mode),
    .CODER_finish(CODER_finish),
    .rho_from_pk(rho_from_pk),
    .c_out(c_out),
    .c_out_reg(c_out_reg),
    .coder_pk_in(coder_pk_in),
    .coder_m_in(coder_m_in),
    .pk_from_sk(pk_from_sk),

    .H_in(H_in),
    .H_active(H_active),
    .H_rst(H_rst),
    .H_mode(H_mode),
    .H_finish(H_finish),
    .H_out(H_out),

    .KDF_in(KDF_in),
    .KDF_active(KDF_active),
    .KDF_mode(KDF_mode),
    .KDF_rst(KDF_rst),
    .KDF_finish(KDF_finish),
    .KDF_out(KDF_out),
    .K_out(K_out),

    .ram_r_sel(ram_r_sel),
    .ram_w_sel(ram_w_sel),

    .DO_finish(DO_finish),
    .DO_active(DO_active),
    .DO_ram_r_offset(DO_ram_r_offset),
    .DO_ram_w_offset(DO_ram_w_offset),
    .DO_rst(DO_rst),
    .DO_mode(DO_mode)
);



A_generator A_gen(  
    .M(A_in),
    .ram_w_start_offset(A_gen_ram_w_start_offset), 
    .clk(clk),.rst(A_gen_rst), .active(A_gen_active),
    .enw(A_wen),
    .waddr(A_waddr),
    .dout(A_wdata),
    .A_finsh(A_finsh)
);

                    
small_poly_generator CBD(   
    .M(CBD_input), 
    .ram_w_start_offset(CBD_ram_w_start_offset),
    .n_num(CBD_num), 
    .clk(clk),.rst(CBD_rst), .active(CBD_active),
    .enw(CBD_wen),
    .waddr(CBD_waddr),
    .dout(CBD_wdata),
    .CBD_finsh(CBD_finsh)
); 


ntt_processer ntt(
    .clk(clk),
    .rst(NTT_rst),
    .start(ntt_start),
    .mode(ntt_mode),
    .is_add_or_sub(ntt_is_add_or_sub),
    .ram_r_start_offset_A(ntt_ram_r_start_offset_A),
    .ram_r_start_offset_B(ntt_ram_r_start_offset_B),
    .ram_w_start_offset(ntt_ram_w_start_offset),
    .ram_rdata(RAM_rdata_a),
    .last_cycle(ntt_last_cycle),
    .ram_wen(ntt_wen),
    .ram_raddr(ntt_raddr),
    .ram_waddr(ntt_waddr),
    .ram_wdata(ntt_wdata),
    .NTT_finish(NTT_finish)
);

coder code(
    .clk(clk),
    .rst(coder_rst),
    .active(coder_active),
    .load_input_Enc(coder_load_input_Enc),
    .load_input_Dec(coder_load_input_Dec),
    .mode(coder_mode), 
   
    .pk_in(coder_pk_in), 
    .m_in(coder_m_in), 
    .sk_in(sk_in),
    .c_in(c_in), 
    
    .pk_out(pk_out), 
    .m_out(m_out_cpa), 
    .sk_out(sk_out_cpa), 
    .c_out(c_out), 

    .pk_from_sk(pk_from_sk),


    .rho_from_G(G_rho),
    .rho_from_pk(rho_from_pk),
    .ram_wen(coder_wen),
    .ram_raddr(coder_raddr),
    .ram_waddr(coder_waddr), 
    .ram_rdata(RAM_rdata_a),
    .ram_wdata(coder_wdata),
    .CODER_finish(CODER_finish)

);

SHA3_256 H(
    .M(H_in),
    .active(H_active),
    .clk(clk),
    .rst(H_rst),
    .kyber_mode(kyber_mode),
    .H_mode(H_mode),
    .finish(H_finish),
    .Z_rv(H_out)
);


SHA3_512 G( 
    .M(G_in),
    .active(G_active),
    .clk(clk),
    .rst(G_rst),
    .G_mode(G_mode),
    .finish(G_finish),
    .Z_rv(G_out)
);

SHAKE_256 KDF(
    .M(KDF_in),
    .active(KDF_active),
    .n_num(KDF_mode),
    .clk(clk),
    .rst(KDF_rst),
    .finish(KDF_finish),
    .Z_rv(KDF_out)
);

ram_96x1024 ram(
    .clk(clk),
    .rst(rst),
    .wena(wen_temp_a),
    .wenb(wen_temp_b),
    .raddra(raddr_temp_a),
    .raddrb(raddr_temp_b),
    .waddra(waddr_temp_a),
    .waddrb(waddr_temp_b),
    .dina(wdata_temp_a),
    .dinb(wdata_temp_b),
    .douta(RAM_rdata_a),
    .doutb(RAM_rdata_b)
);

dataordering DO(
    .clk(clk),
    .rst(DO_rst),
    .active(DO_active),
    .mode(DO_mode),
    .ram_r_offset(DO_ram_r_offset),
    .ram_w_offset(DO_ram_w_offset),
    .ram_rdata(RAM_rdata_b),
    .ram_wen(DO_wen),
    .ram_raddr(DO_raddr),
    .ram_waddr(DO_waddr),
    .ram_wdata(DO_wdata),
    .DATAORDERING_finish(DO_finish)
);

always@(*)begin


    case(ram_r_sel)
        1'b0:begin
            raddr_temp_a = coder_raddr;
            raddr_temp_b = DO_raddr;
        end
        1'b1:begin
            raddr_temp_a = ntt_raddr;
        end
    endcase

    case(ram_w_sel)
        2'd0:begin
            wen_temp_a = coder_wen;
            waddr_temp_a = coder_waddr;
            wdata_temp_a = coder_wdata;
            wen_temp_b = DO_wen;
            waddr_temp_b = DO_waddr;
            wdata_temp_b = DO_wdata;
        end
        2'd1:begin
            wen_temp_a = ntt_wen;
            waddr_temp_a = ntt_waddr;
            wdata_temp_a = ntt_wdata;
        end
        2'd2:begin
            wen_temp_a = A_wen;
            waddr_temp_a = A_waddr;
            wdata_temp_a = A_wdata;
        end
        2'd3:begin
            wen_temp_a = CBD_wen;
            waddr_temp_a = CBD_waddr;
            wdata_temp_a = CBD_wdata;        
        end
    endcase

end



endmodule