module compress (
    input clk,
    input rst,
    input [3:0] d,     
    input [95:0] in_data,
    output [7:0] out_data_d1,
    output [31:0] out_data_d4,
    output [79:0] out_data_d10
);

// input reg
integer i;
reg [11:0] in_reg [0:7];
reg [16:0] in_reg4 [0:7];
reg [19:0] in_reg10 [0:7];
reg [9:0] out_reg10 [0:7];
// reg [23:0] mul_out [0:7];
reg [12:0] subtrahend4 = {8'd208 , 5'd0};
reg [11:0] subtrahend10 = {4'd13 , 8'd0};




always @(posedge clk or posedge rst) begin
    if(rst) for(i=0; i<8; i=i+1) in_reg[i] <= 12'd0;
    else begin//输入8个系数
        in_reg[0] <= in_data[11:00];
        in_reg[1] <= in_data[23:12];
        in_reg[2] <= in_data[35:24];
        in_reg[3] <= in_data[47:36];
        in_reg[4] <= in_data[59:48];
        in_reg[5] <= in_data[71:60];
        in_reg[6] <= in_data[83:72];
        in_reg[7] <= in_data[95:84];

        in_reg4[0] <= {5'b0,in_data[11:00]+12'd103};
        in_reg4[1] <= {5'b0,in_data[23:12]+12'd103};
        in_reg4[2] <= {5'b0,in_data[35:24]+12'd103};
        in_reg4[3] <= {5'b0,in_data[47:36]+12'd103};
        in_reg4[4] <= {5'b0,in_data[59:48]+12'd103};
        in_reg4[5] <= {5'b0,in_data[71:60]+12'd103};
        in_reg4[6] <= {5'b0,in_data[83:72]+12'd103};
        in_reg4[7] <= {5'b0,in_data[95:84]+12'd103};

        in_reg10[0] <= {8'b0,in_data[11:00]};
        in_reg10[1] <= {8'b0,in_data[23:12]};
        in_reg10[2] <= {8'b0,in_data[35:24]};
        in_reg10[3] <= {8'b0,in_data[47:36]};
        in_reg10[4] <= {8'b0,in_data[59:48]};
        in_reg10[5] <= {8'b0,in_data[71:60]};
        in_reg10[6] <= {8'b0,in_data[83:72]};
        in_reg10[7] <= {8'b0,in_data[95:84]};
    end
end

// always @(*) begin
//     for(i=0; i<8; i=i+1) mul_out[i] = in_reg[i] * 12'd2519; // 2519 = (2<<22)/3329 = (16<<19)/3329 = (1024<<13)/3329
// end


integer j;
integer k;
genvar i1;
generate
    for(i1=0;i1<8;i1=i1+1)
        begin
            always @(in_reg4[i1]) begin
                    for(j=0;j<5;j=j+1)
                        begin
                            in_reg4[i1] = {in_reg4[i1][15:0],1'b0};
                            if(in_reg4[i1] >= subtrahend4)
                                in_reg4[i1] = in_reg4[i1] - subtrahend4 + 1'b1;
                        end
            end

            always @(in_reg10[i1]) begin
                    for(k=0;k<8;k=k+1)
                        begin
                            in_reg10[i1] = {in_reg10[i1][18:0],1'b0};
                            if(in_reg10[i1] > subtrahend10)
                                in_reg10[i1] = in_reg10[i1] - subtrahend10 + 1'b1;
                        end
                    if(in_reg10[i1][7:0]>223)
                        case(in_reg10[i1][11:8])
                            4'd13:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd12:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd11:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd10:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd9:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd8:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd7:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd6:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd5:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd4:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd3:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd2:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            4'd1:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            default:out_reg10[i1] = {in_reg10[i1][7:0],2'd0};
                        endcase
                    else if(in_reg10[i1][7:0]>159)
                        case(in_reg10[i1][11:8])
                            4'd13:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd12:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd11:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd10:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd9:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd8:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd7:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd6:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd5:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd4:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd3:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd2:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            4'd1:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            default:out_reg10[i1] = {in_reg10[i1][7:0],2'd0};
                        endcase
                    else if(in_reg10[i1][7:0]>95)
                        case(in_reg10[i1][11:8])
                            4'd13:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd12:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd11:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd10:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd9:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd8:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd7:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd6:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd5:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd4:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd3:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd2:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            4'd1:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            default:out_reg10[i1] = {in_reg10[i1][7:0],2'd0};
                        endcase
                    else if(in_reg10[i1][7:0]>31)
                        case(in_reg10[i1][11:8])
                            4'd13:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd12:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd11:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd10:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd9:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd8:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd7:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd6:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd5:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd4:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd3:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd2:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd1:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            default:out_reg10[i1] = {in_reg10[i1][7:0],2'd0};
                        endcase
                    else
                        case(in_reg10[i1][11:8])
                            4'd13:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd12:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd4;
                            4'd11:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd10:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd9:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd3;
                            4'd8:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd7:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd6:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd5:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd2;
                            4'd4:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd3:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd2:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd1;
                            4'd1:out_reg10[i1] = {in_reg10[i1][7:0],2'd0} + 3'd0;
                            default:out_reg10[i1] = {in_reg10[i1][7:0],2'd0};
                        endcase
            end
        end
endgenerate

    



//assign out_data_d1 = {  mul_out[7][22], mul_out[6][22], mul_out[5][22], mul_out[4][22], 
//                        mul_out[3][22], mul_out[2][22], mul_out[1][22], mul_out[0][22] } ; // mul_out >> 22
//直接右移是向下取整，比较大小是四舍五入
//0~3328映射到0~1，[0,0.5)为0，[0.5,1.5)为1，[1.5,2)为0；3329对应的0.5点和1.5点为832.25和2496.75
assign out_data_d1[7] = in_reg[7] > 12'd832 && in_reg[7] < 12'd2497;
assign out_data_d1[6] = in_reg[6] > 12'd832 && in_reg[6] < 12'd2497;
assign out_data_d1[5] = in_reg[5] > 12'd832 && in_reg[5] < 12'd2497;
assign out_data_d1[4] = in_reg[4] > 12'd832 && in_reg[4] < 12'd2497;
assign out_data_d1[3] = in_reg[3] > 12'd832 && in_reg[3] < 12'd2497;
assign out_data_d1[2] = in_reg[2] > 12'd832 && in_reg[2] < 12'd2497;
assign out_data_d1[1] = in_reg[1] > 12'd832 && in_reg[1] < 12'd2497;
assign out_data_d1[0] = in_reg[0] > 12'd832 && in_reg[0] < 12'd2497;

// assign out_data_d4 = {  mul_out[7][22:19], mul_out[6][22:19], mul_out[5][22:19], mul_out[4][22:19], 
//                         mul_out[3][22:19], mul_out[2][22:19], mul_out[1][22:19], mul_out[0][22:19] } ; // mul_out >> 19

assign out_data_d4 = {  in_reg4[7][3:0], in_reg4[6][3:0], in_reg4[5][3:0], in_reg4[4][3:0], 
                        in_reg4[3][3:0], in_reg4[2][3:0], in_reg4[1][3:0], in_reg4[0][3:0] } ;


// assign out_data_d10 = { mul_out[7][22:13], mul_out[6][22:13], mul_out[5][22:13], mul_out[4][22:13], 
//                         mul_out[3][22:13], mul_out[2][22:13], mul_out[1][22:13], mul_out[0][22:13] } ; // mul_out >> 13

assign out_data_d10 = { out_reg10[7], out_reg10[6], out_reg10[5], out_reg10[4], 
                        out_reg10[3], out_reg10[2], out_reg10[1], out_reg10[0] } ;

endmodule