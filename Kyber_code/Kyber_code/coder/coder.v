`include "../coder/compress.v"
`include "../coder/decompress.v"

module coder (
    input clk,
    input rst,
    input active,
    input load_input_Enc,
    input load_input_Dec,
    input [3:0] mode, 
   
    input [6399:0] pk_in, // 12*256*2 + 256
    input [255:0] m_in, 
    input [13055:0] sk_in, // 12*256*2
    input [6143:0] c_in, // 10*256*2 + 4*256
    
    output [6399:0] pk_out, // 12*256*2 + 256
    output [255:0] m_out, 
    output [6143:0] sk_out, // 12*256*2
    output [6143:0] c_out, // 10*256*2 + 4*256

    output reg [6399:0] pk_from_sk,


    input [255:0] rho_from_G,
    output [255:0] rho_from_pk,   //load_input_Enc=1时，=pk_in[255:0]
    output reg ram_wen,
    output reg [9:0] ram_raddr,
    output reg [9:0] ram_waddr, 
    input [95:0] ram_rdata,
    output reg [95:0] ram_wdata,
    output reg CODER_finish

);

// ram offset
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


// mode define
parameter WAIT = 4'd0;
parameter KeyGen_encode_sk = 4'd1; // 65 cycle (active 1 + working 64)
parameter KeyGen_encode_pk = 4'd2; // 65 cycle (active 1 + working 64)
parameter Enc_decode_pk = 4'd3; // 65 cycle (active 1 + working 64)
parameter Enc_decode_m = 4'd4; // 34 cycle (active 1 + working 33)
parameter Enc_encode_c = 4'd5; // 98 cycle (active 1 + working 97)
parameter Dec_decode_sk = 4'd6; // 65 cycle (active 1 + working 64)
parameter Dec_decode_c = 4'd7; // 98 cycle (active 1 + working 97)
parameter Dec_encode_m = 4'd8; // 34 cycle (active 1 + working 33)

// registers
reg [3:0] mode_reg;
reg [6:0] cnt;
reg [6143:0] t_c_reg; // store t at Key_gen, store t/c at Enc, store  c  at Dec
reg [6143:0] s_m_reg; // store s at Key_gen, store  m  at Enc, store s/m at Dec
reg [6143:0] m_reg;
reg [255:0] rho_reg;
reg [5119:0] decode_u_reg;
reg [1023:0] decode_v_reg;


// 
reg [3:0] d;
wire [7:0] comp_out_d1;
wire [31:0] comp_out_d4;
wire [79:0] comp_out_d10;
reg [7:0] decomp_in_d1;
reg [31:0] decomp_in_d4;
reg [79:0] decomp_in_d10;
wire [95:0] decomp_out_data;
reg last_cycle;

compress comp (
    .clk(clk),
    .rst(rst),
    .d(d), 
    .in_data(ram_rdata),
    .out_data_d1(comp_out_d1),
    .out_data_d4(comp_out_d4),
    .out_data_d10(comp_out_d10)
);

decompress decomp (
    .clk(clk),
    .rst(rst),
    .d(d),
    .in_data_d1(decomp_in_d1),
    .in_data_d4(decomp_in_d4),
    .in_data_d10(decomp_in_d10),
    .out_data(decomp_out_data)
);

always @(posedge clk or posedge rst) begin
    if(rst) begin
        mode_reg <= WAIT;
        cnt <= 7'd0;
        CODER_finish <= 1'd0;
    end
    else begin
        if(active) begin
            mode_reg <= mode;
            cnt <= 7'd0;
        end
        else if(last_cycle) begin
            mode_reg <= WAIT;
            cnt <= 7'd0;
            CODER_finish <= 1'd1;
        end
        else if(mode_reg != WAIT) begin
            cnt <= cnt + 7'd1;
        end
        if(!cnt) CODER_finish <= 1'b0;
    end
end



// output wires
assign pk_out = { t_c_reg, rho_reg };
// assign m_out = m_reg[255:0];
assign m_out = m_reg;
assign sk_out = s_m_reg;
assign c_out = t_c_reg;
assign rho_from_pk = rho_reg[255:0];

// register wires
reg [95:0] t_c_reg_t_in [0:63];
wire [95:0] t_c_reg_t [0:63];

reg [79:0] t_c_reg_u_in [0:63];
wire [79:0] t_c_reg_u [0:63];

reg [31:0] t_c_reg_v_in [0:31];
wire [31:0] t_c_reg_v [0:31];

reg [95:0] s_m_reg_s_in [0:63];//从ram中读取出来的数据从放在这
wire [95:0] s_m_reg_s [0:63];

reg [7:0] s_m_reg_m_in [0:31];
reg [7:0] m_reg_in [0:31];
wire [7:0] s_m_reg_m [0:31];

genvar i1;
//把t_c_reg中的数据按照情况分为t/u/v,把s_m_reg中的数据按照情况分为s/m
generate
    for(i1=0; i1<64; i1=i1+1) begin
        // assign t_c_reg_t[i1] = t_c_reg[(i1*96+95):(i1*96)];//系数整体以大端模式存储，高位标号的向量存储低次幂的系数
        assign t_c_reg_t[i1] = {t_c_reg[(i1*96+83):(i1*96+80)],t_c_reg[(i1*96+95):(i1*96+88)],
                                t_c_reg[(i1*96+79):(i1*96+72)],t_c_reg[(i1*96+87):(i1*96+84)],
                                t_c_reg[(i1*96+59):(i1*96+56)],t_c_reg[(i1*96+71):(i1*96+64)],
                                t_c_reg[(i1*96+55):(i1*96+48)],t_c_reg[(i1*96+63):(i1*96+60)],
                                t_c_reg[(i1*96+35):(i1*96+32)],t_c_reg[(i1*96+47):(i1*96+40)],
                                t_c_reg[(i1*96+31):(i1*96+24)],t_c_reg[(i1*96+39):(i1*96+36)],
                                t_c_reg[(i1*96+11):(i1*96+8)],t_c_reg[(i1*96+23):(i1*96+16)],
                                t_c_reg[(i1*96+7):(i1*96)],t_c_reg[(i1*96+15):(i1*96+12)]};
        // assign t_c_reg_u[i1] = decode_u_reg[(i1*80+79):(i1*80)];//系数整体以大端模式存储，高位标号的向量存储低次幂的系数
        assign t_c_reg_u[i1] = {decode_u_reg[(i1*80+65):(i1*80+64)],decode_u_reg[(i1*80+79):(i1*80+72)],
                                decode_u_reg[(i1*80+59):(i1*80+56)],decode_u_reg[(i1*80+71):(i1*80+66)],
                                decode_u_reg[(i1*80+53):(i1*80+48)],decode_u_reg[(i1*80+63):(i1*80+60)],
                                decode_u_reg[(i1*80+47):(i1*80+40)],decode_u_reg[(i1*80+55):(i1*80+54)],
                                decode_u_reg[(i1*80+25):(i1*80+24)],decode_u_reg[(i1*80+39):(i1*80+32)],
                                decode_u_reg[(i1*80+19):(i1*80+16)],decode_u_reg[(i1*80+31):(i1*80+26)],
                                decode_u_reg[(i1*80+13):(i1*80+8)],decode_u_reg[(i1*80+23):(i1*80+20)],
                                decode_u_reg[(i1*80+7):(i1*80)],decode_u_reg[(i1*80+15):(i1*80+14)]};
        // assign s_m_reg_s[i1] = s_m_reg[(i1*96+95):(i1*96)];//系数整体以大端模式存储，高位标号的向量存储低次幂的系数
        assign s_m_reg_s[i1] = {s_m_reg[(i1*96+83):(i1*96+80)],s_m_reg[(i1*96+95):(i1*96+88)],
                                s_m_reg[(i1*96+79):(i1*96+72)],s_m_reg[(i1*96+87):(i1*96+84)],
                                s_m_reg[(i1*96+59):(i1*96+56)],s_m_reg[(i1*96+71):(i1*96+64)],
                                s_m_reg[(i1*96+55):(i1*96+48)],s_m_reg[(i1*96+63):(i1*96+60)],
                                s_m_reg[(i1*96+35):(i1*96+32)],s_m_reg[(i1*96+47):(i1*96+40)],
                                s_m_reg[(i1*96+31):(i1*96+24)],s_m_reg[(i1*96+39):(i1*96+36)],
                                s_m_reg[(i1*96+11):(i1*96+8)],s_m_reg[(i1*96+23):(i1*96+16)],
                                s_m_reg[(i1*96+7):(i1*96)],s_m_reg[(i1*96+15):(i1*96+12)]};
    end
    for(i1=0; i1<32; i1=i1+1) begin
        // assign t_c_reg_v[i1] = decode_v_reg[(i1*32+31):(i1*32)];//系数整体以大端模式存储，高位标号的向量存储低次幂的系数
        assign t_c_reg_v[i1] = {decode_v_reg[(i1*32+27):(i1*32+24)],decode_v_reg[(i1*32+31):(i1*32+28)],
                                decode_v_reg[(i1*32+19):(i1*32+16)],decode_v_reg[(i1*32+23):(i1*32+20)],
                                decode_v_reg[(i1*32+11):(i1*32+8)],decode_v_reg[(i1*32+15):(i1*32+12)],
                                decode_v_reg[(i1*32+3):(i1*32)],decode_v_reg[(i1*32+7):(i1*32+4)]};
        // assign s_m_reg_m[i1] = s_m_reg[(i1*8+7):(i1*8)];//系数整体以小端模式存储，高位标号的向量存储高次幂的系数
        assign s_m_reg_m[i1] = {s_m_reg[i1*8+0],s_m_reg[i1*8+1],s_m_reg[i1*8+2],s_m_reg[i1*8+3],
                                s_m_reg[i1*8+4],s_m_reg[i1*8+5],s_m_reg[i1*8+6],s_m_reg[i1*8+7]};
    end
endgenerate

integer i2;
always @(posedge clk or posedge rst) begin
    if(rst) begin
        t_c_reg <= 6144'd0;//12*256*2
        s_m_reg <= 6144'd0;
        rho_reg <= 256'd0;
    end
    else if(load_input_Enc) begin
        rho_reg <= pk_in[255:0];
        t_c_reg <= pk_in[6399:256];
        s_m_reg <= m_in;
    end
    else if(load_input_Dec) begin
        decode_u_reg <= c_in[6143:1024];
        decode_v_reg <= c_in[1023:0];
        s_m_reg <= sk_in[13055:6912];
        pk_from_sk <= sk_in[6911:512];

    end
    else begin
        case(mode_reg)//所有encode步骤中将分立寄存器整合成单一寄存器的步骤
        KeyGen_encode_sk: 
            for(i2=0; i2<64; i2=i2+1)
                s_m_reg[i2*96 +: 96] <= {s_m_reg_s_in[i2][91:84],s_m_reg_s_in[i2][75:72],s_m_reg_s_in[i2][95:92],s_m_reg_s_in[i2][83:76],
                                        s_m_reg_s_in[i2][67:60],s_m_reg_s_in[i2][51:48],s_m_reg_s_in[i2][71:68],s_m_reg_s_in[i2][59:52],
                                        s_m_reg_s_in[i2][43:36],s_m_reg_s_in[i2][27:24],s_m_reg_s_in[i2][47:44],s_m_reg_s_in[i2][35:28],
                                        s_m_reg_s_in[i2][19:12],s_m_reg_s_in[i2][3:0],s_m_reg_s_in[i2][23:20],s_m_reg_s_in[i2][11:4]};//512个系数需要64个地址
        KeyGen_encode_pk: begin
            if(cnt == 4'd0) begin
                // for(i2=0; i2<32; i2=i2+1)
                    rho_reg <= rho_from_G;
            end
            for(i2=0; i2<64; i2=i2+1)
                t_c_reg[i2*96 +: 96] <= {t_c_reg_t_in[i2][91:84],t_c_reg_t_in[i2][75:72],t_c_reg_t_in[i2][95:92],t_c_reg_t_in[i2][83:76],
                                        t_c_reg_t_in[i2][67:60],t_c_reg_t_in[i2][51:48],t_c_reg_t_in[i2][71:68],t_c_reg_t_in[i2][59:52],
                                        t_c_reg_t_in[i2][43:36],t_c_reg_t_in[i2][27:24],t_c_reg_t_in[i2][47:44],t_c_reg_t_in[i2][35:28],
                                        t_c_reg_t_in[i2][19:12],t_c_reg_t_in[i2][3:0],t_c_reg_t_in[i2][23:20],t_c_reg_t_in[i2][11:4]};
        end
        Enc_encode_c: begin
            for(i2=0; i2<64; i2=i2+1)//c1:10*256*2,每个系数10b,一共256*2个系数;每次传8个系数,需要传64次
                t_c_reg[(i2*80+32*32) +: 80] <= {t_c_reg_u_in[i2][77:70],
                                                t_c_reg_u_in[i2][65:60],t_c_reg_u_in[i2][79:78],
                                                t_c_reg_u_in[i2][53:50],t_c_reg_u_in[i2][69:66],
                                                t_c_reg_u_in[i2][41:40],t_c_reg_u_in[i2][59:54],
                                                t_c_reg_u_in[i2][49:42],
                                                t_c_reg_u_in[i2][37:30],
                                                t_c_reg_u_in[i2][25:20],t_c_reg_u_in[i2][39:38],
                                                t_c_reg_u_in[i2][13:10],t_c_reg_u_in[i2][29:26],
                                                t_c_reg_u_in[i2][1:0],t_c_reg_u_in[i2][19:14],
                                                t_c_reg_u_in[i2][9:2]};
            for(i2=0; i2<32; i2=i2+1)//c2:4*256,每个系数4b,一共256个系数;每次传8个系数,需要传32次
                t_c_reg[i2*32 +: 32] <= {t_c_reg_v_in[i2][27:24],t_c_reg_v_in[i2][31:28],
                                        t_c_reg_v_in[i2][19:16],t_c_reg_v_in[i2][23:20],
                                        t_c_reg_v_in[i2][11:8],t_c_reg_v_in[i2][15:12],
                                        t_c_reg_v_in[i2][3:0],t_c_reg_v_in[i2][7:4]};
        end
        Dec_encode_m:
            for(i2=0; i2<32; i2=i2+1)//m:1*256,每个系数1b,一共256个系数;每次传8个系数,需要传32次
                m_reg[i2*8 +: 8] <= {m_reg_in[i2][0],m_reg_in[i2][1],m_reg_in[i2][2],m_reg_in[i2][3],
                                    m_reg_in[i2][4],m_reg_in[i2][5],m_reg_in[i2][6],m_reg_in[i2][7]};
        endcase
    end
end

always @(*) begin
    ram_wen = 1'b0;
    ram_raddr = 10'd0;
    ram_waddr = 10'd0;
    ram_wdata = 96'd0;
    d = 4'd1;
    decomp_in_d1 = 8'd0;
    decomp_in_d4 = 32'd0;
    decomp_in_d10 = 80'd0;
    last_cycle = 1'b0;
    // for(i2=0; i2<64; i2=i2+1) begin//在decode过程中把t/u/v/s/m数据存放在ram中的同时也存放在其对应的输入寄存器中备份
    //     t_c_reg_t_in[i2] = t_c_reg_t[i2];
    //     t_c_reg_u_in[i2] = t_c_reg_u[i2];
    //     s_m_reg_s_in[i2] = s_m_reg_s[i2];
    // end
    // for(i2=0; i2<32; i2=i2+1) begin
    //     t_c_reg_v_in[i2] = t_c_reg_v[i2];
    //     s_m_reg_m_in[i2] = s_m_reg_m[i2];
    // end

    case(mode_reg)//encode时数据从ram读取到reg中，decode时数据从reg中写入到ram中；所有encode/decode过程中的数据控制
        KeyGen_encode_sk: begin
            ram_raddr = ram_4_offset + cnt;
            s_m_reg_s_in[63-cnt] = ram_rdata;
            last_cycle = cnt == 7'd63;
        end
        KeyGen_encode_pk: begin
            ram_raddr = ram_6_offset + cnt;
            t_c_reg_t_in[63-cnt] = ram_rdata;
            last_cycle = cnt == 7'd63;
        end
        Enc_decode_pk: begin
            ram_wen = 1'b1;
            ram_waddr = ram_9_offset + cnt;
            ram_wdata = t_c_reg_t[63-cnt];
            last_cycle = cnt == 7'd63;
        end
        Enc_decode_m: begin
            ram_wen = cnt != 7'd0 ? 1'b1 : 1'b0;
            ram_waddr = ram_11_offset + cnt - 10'd1;
            ram_wdata = decomp_out_data;
            d = 4'd1;
            decomp_in_d1 = s_m_reg_m[31-cnt];
            last_cycle = cnt == 7'd32;
        end
        Enc_encode_c: begin
            if(cnt == 7'd0) begin
                d = 4'd10;
                ram_raddr = ram_6_offset + cnt;
            end
            else if(cnt < 7'd64) begin
                d = 4'd10;
                ram_raddr = ram_6_offset + cnt;
                t_c_reg_u_in[64-cnt] = comp_out_d10;
            end
            else if(cnt == 7'd64) begin
                d = 4'd4;
                ram_raddr = ram_9_offset + cnt - 7'd64;
                t_c_reg_u_in[64-cnt] = comp_out_d10;
            end
            else begin
                d = 4'd4;
                ram_raddr = ram_9_offset + cnt - 7'd64;
                t_c_reg_v_in[96-cnt] = comp_out_d4;
            end
            last_cycle = cnt == 7'd96;
        end
        Dec_decode_sk: begin
            ram_wen = 1'b1;
            ram_waddr = ram_15_offset + cnt;
            ram_wdata = s_m_reg_s[63-cnt];
            last_cycle = cnt == 7'd63;
        end
        Dec_decode_c: begin
            ram_wen = (cnt != 7'd0 && cnt != 7'd65) ? 1'b1 : 1'b0;
            ram_waddr = cnt < 7'd65 ? ram_12_offset + cnt - 10'd1 : ram_12_offset + cnt - 10'd2;
            ram_wdata = decomp_out_data;
            d = cnt < 7'd65 ? 4'd10 : 4'd4;
            decomp_in_d10 = t_c_reg_u[63-cnt];
            decomp_in_d4 = t_c_reg_v[96-cnt];
            last_cycle = cnt == 7'd97;
        end
        Dec_encode_m: begin
            ram_raddr = ram_12_offset + cnt;
            m_reg_in[32-cnt] = comp_out_d1;
            last_cycle = cnt == 7'd32;
        end
    endcase
end
endmodule