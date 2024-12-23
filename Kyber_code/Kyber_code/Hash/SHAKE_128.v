`include "../Hash/KECCAK_p.v"
//XOF
module SHAKE_128(input [0:272-1] M,
                input active, //Let stage go to s1 
                input clk, rst,
                output reg finish,
                output [0:4032-1] Z_rv);

parameter d_SIZE = 4032,//256*12=4032
          c_SIZE = 256,
          r_SIZE = 1344, //r=b-c
          M_SIZE = 272; //256+8+8
parameter j = 1066;//j=r_SIZE - (M_SIZE+4+2)

wire [0:272-1] M_rv;
wire [0:(M_SIZE+4)-1] N;
wire [0:((M_SIZE+4)+1+j+1)-1] P;
wire [4:0] nr;
wire [0:1599] str_out;
wire [2:0] cnt;//counter+1
wire [12:0] counter_r_r_SIZE;//counter_r-r_SIZE
reg [0:4032-1] Z;
reg [2:0] cs,ns;
reg string_val;
reg [0:1599] str, str_temp, str_temp2;//str 上次迭代函数的输出值；str_temp 迭代函数的输出值与本次吸入数据的异或值，是进入迭代函数的输入值；str_temp2 本轮迭代函数的输出值
reg [2:0] counter;  //0~n-1
reg [4:0] counter_f;//count rnds of KECCAK_p
reg [12:0] counter_r;//count times of Z=Z||Trunc_r(S)
reg [0:(r_SIZE*3)-1] Z_temp;// r*3 bits (r*3 = 4032) 存储3个输出的r

genvar g;
generate
    for ( g = 0; g < 34; g=g+1 ) begin
        assign M_rv[8*g+0] = M[8*g+7];
        assign M_rv[8*g+1] = M[8*g+6];
        assign M_rv[8*g+2] = M[8*g+5];
        assign M_rv[8*g+3] = M[8*g+4];
        assign M_rv[8*g+4] = M[8*g+3];
        assign M_rv[8*g+5] = M[8*g+2];
        assign M_rv[8*g+6] = M[8*g+1];
        assign M_rv[8*g+7] = M[8*g+0];
    end 
endgenerate

generate
    for ( g = 0; g < 504; g=g+1 ) begin
        assign Z_rv[8*g+0] = Z[8*g+7];
        assign Z_rv[8*g+1] = Z[8*g+6];
        assign Z_rv[8*g+2] = Z[8*g+5];
        assign Z_rv[8*g+3] = Z[8*g+4];
        assign Z_rv[8*g+4] = Z[8*g+3];
        assign Z_rv[8*g+5] = Z[8*g+2];
        assign Z_rv[8*g+6] = Z[8*g+1];
        assign Z_rv[8*g+7] = Z[8*g+0];
    end 
endgenerate

assign N ={M_rv,4'b1111}; //N=M||1111
assign nr = 5'd24;   //nr=24

//pad10*1(r,len(N)) M||1111||10*1 P是完成填充后的输入
assign P = {N,1'b1,{j{1'b0}},1'b1};
//
parameter n = 1;//n=len(P)/r = 1
assign cnt = counter + 3'd1;//counter+1
assign counter_r_r_SIZE = counter_r-r_SIZE;

parameter s0 = 3'b000,
          s1 = 3'b001,
          s2 = 3'b010,
          s3 = 3'b011,
          s4 = 3'b100,
          s5 = 3'b101,
          s6 = 3'b110,
          s7 = 3'b111;
//keccak迭代函数 执行一次就是执行24圈操作
KECCAK_p keccak_p( .S(str_temp),
                   .nr(nr),
                   .string_val(string_val),
                   .clk(clk), .rst(rst),
                   .S_out(str_out));

always@(posedge clk or posedge rst)begin
        if(rst)begin
            cs <= s0;
            str_temp <= 1600'd0;
            str_temp2 <= 1600'd0;
            counter <= 3'd0;
            counter_f <= 5'd0;
            counter_r <= 13'd0;
            Z_temp <= {r_SIZE*3{1'b0}};

        end
        else begin
            cs <= ns;
            //str_temp= S^(Pi||c(0))
            if(ns == s1)begin
                str_temp <= str^{P[counter+:r_SIZE],{c_SIZE{1'b0}}};//counter只是用来记录吸水阶段进行的次数，感觉这里应该写成r_SIZE*counter
            end
            else if(ns == s4) begin
                str_temp <= str_temp2;//挤压阶段迭代函数输出值直接作为下一迭代函数的输入值
            end
            else begin
                str_temp <= str_temp;
            end
            
            
            //let S equal to str_out after do a S=f(S^(Pi||c(0))) and S=f(S)
            str_temp2 <= (cs == s2 || cs == s5)? str_out:str_temp2;

            counter <= (cs == s2)? counter+3'd1 : counter;
            counter_r <= (ns == s3 || ns==s6)? counter_r+r_SIZE:counter_r;//+r_SIZE
            
            //counter_f
            if((cs == s1)||(cs == s4))begin
                counter_f <= counter_f+5'd1;//迭代函数执行的圈数
            end
            else if((ns == s1) || (ns==s4))begin
                counter_f <= 5'd0;
            end
            else begin
                counter_f <= counter_f;
            end

            //Z=Z||Trunc_r(S)
            if(cs == s3 || cs == s6)begin
                Z_temp[counter_r_r_SIZE +:r_SIZE] <= str_temp2[0:r_SIZE-1];//每次挤压阶段的迭代函数执行完毕后将本次输出的r位输出数据串联到Z_temp后
            end
            else begin
                Z_temp[counter_r_r_SIZE +:r_SIZE] <= Z_temp[counter_r_r_SIZE +:r_SIZE];
            end

        end

end
            


always@(cs or active or counter_f or cnt or counter_r)begin
	//next stage
	case(cs)
		s0: ns = (active)? s1:s0;
		s1: ns = (counter_f== 5'd25) ? s2:s1;//S=f(S^(Pi||c(0))) 吸水阶段完成一次24圈的迭代函数
		s2: ns = (cnt == n) ? s3:s1; //counter is from 0 to n-1 吸水阶段输入数据被分成的组数
        s3: ns = (counter_r >= d_SIZE)?s7:s4;//determine if d<=|Z| 吸水阶段完成后所输出的数据是否满足所需的长度
        s4: ns = (counter_f == 5'd25) ?s5:s4;//S=f(S) 挤压阶段完成一次24圈的迭代函数
        s5: ns = s6;
        s6: ns = (counter_r >= d_SIZE)?s7:s4;//determine if d<=|Z| 每轮挤压后所输出的数据是否满足所需的长度
        s7: ns = s7;//output
		default: ns = s0;
	endcase	
end

always@(cs or counter or str_temp2)begin
    case(cs)
		s0: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = 1600'd0;
                finish = 1'b0;
            end    
		s1: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1 keccak迭代函数使能
                finish = 1'b0;
                if(counter==3'd0)begin
                    str = 1600'd0; //首次进入迭代函数时上轮迭代函数的输出值赋0
                end
                else begin
                    str = str_temp2;
                end
            end
		s2: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s3: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
        s4: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s5: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b1;//string_val=1
                str = str_temp2;
                finish = 1'b0;
            end
        s6: begin
                Z = Z_temp[0:d_SIZE-1];
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
        s7: begin
                Z = Z_temp[0:d_SIZE-1]; //Z=Trunc_d(Z)
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b1; //finish = 1
            end
		default: begin
                Z = {d_SIZE{1'd0}};
                string_val = 1'b0;
                str = str_temp2;
                finish = 1'b0;
            end
    endcase
end

endmodule