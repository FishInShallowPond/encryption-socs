`include "../Hash/SHAKE_128.v"
//Parse
module A_generator(  input [0:272-1] M, //256+8+8 rho和当前采样的A矩阵中元素的坐标信息
                    input [9:0] ram_w_start_offset, 
                    input clk,rst, active,
                    output reg enw,
                    output reg [9:0] waddr,
                    output reg [95:0] dout,
                    output reg A_finsh);//每次输出1个RAM地址96b的数据，每次调用共输出32次，共256的系数

wire [11:0] d1,d2,d3,d4,d5,d6,d7,d8;
wire [11:0] i_0,i_1,i_2,i2_0,i2_1,i2_2,i3_0,i3_1,i3_2,i4_0,i4_1,i4_2;
wire [4032-1:0] b,c;
wire enable;
reg [1:0] cs,ns;
reg [9:0]counter;
reg [11:0] i;
reg [107:0] dout_pre; //12*9
reg [3:0] dout_pre_num;
reg d1_valid,d2_valid;

parameter s0 = 2'b00,
          s1 = 2'b01,
          s2 = 2'b10;

SHAKE_128 XOF(.M(M),.active(active),.clk(clk),.rst(rst),.finish(enable),.Z_rv(c));//输出4032b比特流

genvar i1;
generate
    for(i1=0; i1<504; i1=i1+1) begin
        assign b[4031-i1*8:4024-i1*8] = c[i1*8+7:i1*8];
    end
endgenerate

//每个通道每个周期代表一个8b的字节，数字代表比特的序数，循环32个周期
assign i_0 = i<<3;//i_0=i*8 0,24,48,72,...,744
assign i_1 = (i+12'd1)<<3;//i_1=(i+1)*8 8,32,56,80,...,752
assign i_2 = (i+12'd2)<<3;//i_2=(i+2)*8 16,40,64,88,...,760
// assign i2_0 = (i+(12'd3 << 5))<<3;//i2_0=(i+3*32)*8 768,792,816,840,...,1512
// assign i2_1 = (i+(12'd3 << 5)+12'd1)<<3;//i_1=(i+3*32+1)*8 776,800,824,848,...,1520
// assign i2_2 = (i+(12'd3 << 5)+12'd2)<<3;//i_2=(i+3*32+2)*8 784,808,830,856,...,1528
// assign i3_0 = (i+(12'd3 << 6))<<3;//i_0=(i+3*64)*8 1536,1560,1584,1608,...,2280
// assign i3_1 = (i+(12'd3 << 6)+12'd1)<<3;//i_1=(i+3*64+1)*8 1544,1568,1592,1616,...,2288
// assign i3_2 = (i+(12'd3 << 6)+12'd2)<<3;//i_2=(i+3*64+2)*8 1552,1576,1600,1624,...,2296
// assign i4_0 = (i+(12'd3 << 6)+(12'd3 << 5))<<3;//i_0=(i+3*96)*8 2304,2328,2352,2376,...,3048
// assign i4_1 = (i+(12'd3 << 6)+(12'd3 << 5)+12'd1)<<3;//i_1=(i+3*96+1)*8 2312,2336,2360,2384,...,3056
// assign i4_2 = (i+(12'd3 << 6)+(12'd3 << 5)+12'd2)<<3;//i_2=(i+3*96+2)*8 2320,2344,2368,2392,...,3064

//when i=0 => d1=a0, d2=a1
assign d1 = {4'd0, b[i_0 +:8]} + (({8'd0, b[i_1+:4]})<<8);//d1=b[i]+256(b[i+1] mod+ 16)
assign d2 = ({4'd0, b[i_1+:8]}>>4) + ({4'd0, b[i_2+:8]}<<4);//d2=b[i+1]/16 + b[i+2]*16

// //when i=0 => d3=a64, d4=a65
// assign d3 = {4'd0, b[i2_0 +:8]} + (({8'd0, b[i2_1+:4]})<<8);
// assign d4 = ({4'd0, b[i2_1+:8]}>>4) + ({4'd0, b[i2_2+:8]}<<4);

// //when i=0 => d5=a128, d4=a129
// assign d5 = {4'd0, b[i3_0 +:8]} + (({8'd0, b[i3_1+:4]})<<8);
// assign d6 = ({4'd0, b[i3_1+:8]}>>4) + ({4'd0, b[i3_2+:8]}<<4);

// //when i=0 => d7=a192, d8=a193
// assign d7 = {4'd0, b[i4_0 +:8]} + (({8'd0, b[i4_1+:4]})<<8);
// assign d8 = ({4'd0, b[i4_1+:8]}>>4) + ({4'd0, b[i4_2+:8]}<<4);

always@(posedge clk or posedge rst)begin
    if(rst)begin
        cs <= 2'd0;
        counter <= 10'd0;
        i <= 12'd0;
        A_finsh <= 1'b0;
        d1_valid <= 1'b0;
        d1_valid <= 1'b0;
        dout_pre <= 108'd0;
        dout_pre_num <= 4'd0;
    end
    else begin
        cs <= ns;
        //counter <= ((ns == s1)&&(cs != s0))? counter + 10'd1: counter;//counter:0~31
        i <= ((ns == s1)&&(cs != s0))? i+12'd3: i; //i=i+3

    end
end

always@(cs or enable or counter)begin
	//next state
	case(cs)
		s0: ns = (enable)? s1:s0;
		s1: ns =(counter < 8'd32)? s1:s2;
		s2: ns = s2;
		default: ns = s0;
	endcase	
end

//output
always@(cs or d1 or d2 or d3 or d4 or d5 or d6 or d7 or d8 or counter or ram_w_start_offset)begin
	case(cs)
		s0: begin
            enw = 1'b0;
            waddr = 8'd0;
            dout = 96'd0;
            A_finsh = 1'b0;
            d1_valid = 1'b0;
            d1_valid = 1'b0;
            dout_pre = 108'd0;
            dout_pre_num = 4'd0;           
        end           
		s1: begin
            //next clk write 8 coeff
            enw = 1'b0;
            
            // //addr 0 => coeff: 193 192 65 64 129 128 1 0
            // dout[0 +:12]=(d1 >= 12'd3329)? {1'b0,{d1[10:0]}}:d1;
            // dout[12 +:12]=(d2 >= 12'd3329)? {1'b0,{d2[10:0]}}:d2;
            // dout[24 +:12]=(d5 >= 12'd3329)? {1'b0,{d5[10:0]}}:d5;
            // dout[36 +:12]=(d6 >= 12'd3329)? {1'b0,{d6[10:0]}}:d6;
            // dout[48 +:12]=(d3 >= 12'd3329)? {1'b0,{d3[10:0]}}:d3;
            // dout[60 +:12]=(d4 >= 12'd3329)? {1'b0,{d4[10:0]}}:d4;
            // dout[72 +:12]=(d7 >= 12'd3329)? {1'b0,{d7[10:0]}}:d7;
            // dout[84 +:12]=(d8 >= 12'd3329)? {1'b0,{d8[10:0]}}:d8;
            case(dout_pre_num)
                4'd9: begin
                    dout = dout_pre[107:12];
                    enw = 1'b1;
                    counter = counter + 1;
                    dout_pre_num = dout_pre_num - 8;
                end
                4'd8: begin
                    dout = dout_pre[95:0];
                    enw = 1'b1;
                    counter = counter + 1;
                    dout_pre_num = dout_pre_num - 8;
                end
            endcase
            waddr = ram_w_start_offset + counter - 1;

            d1_valid = (d1 < 12'd3329) ? 1 : 0;
            d2_valid = (d2 < 12'd3329) ? 1 : 0;
            case({d1_valid,d2_valid})
                2'b11: begin
                    dout_pre = (dout_pre<<24) | (d1<<12) | d2;
                    dout_pre_num = dout_pre_num + 2;
                end
                2'b01: begin
                    dout_pre = (dout_pre<<12) | d2;
                    dout_pre_num = dout_pre_num + 1;
                end
                2'b10: begin
                    dout_pre = (dout_pre<<12) | d1;
                    dout_pre_num = dout_pre_num + 1;
                end
            endcase
        end                
        s2: begin
            enw = 1'b0;
            waddr = 8'd0;
            dout = 96'd0;
            A_finsh = 1'b1;      
            d1_valid = 1'b0;
            d1_valid = 1'b0;
            dout_pre = 108'd0;
            dout_pre_num = 4'd0;                
        end
		default:  begin
            enw = 1'b0;
            waddr = 8'd0;
            dout = 96'd0;
            A_finsh = 1'b0;           
            d1_valid = 1'b0;
            d1_valid = 1'b0;
            dout_pre = 108'd0;
            dout_pre_num = 4'd0;           
        end
        endcase
end

endmodule