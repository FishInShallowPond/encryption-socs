module dataordering(
    input clk,
    input rst,
    input active,
    input [2:0] mode,
    input [9:0] ram_r_offset,
    input [9:0] ram_w_offset,
    input [95:0] ram_rdata,
    output reg ram_wen,
    output reg [9:0] ram_raddr,
    output reg [9:0] ram_waddr,
    output reg [95:0] ram_wdata,
    output reg DATAORDERING_finish
);

reg [7:0] cnt;
reg [95:0] u_v_buffer [63:0];
reg [2:0] mode_reg;

//DO mode define
parameter INIT = 3'd0;
parameter PRENTT = 3'd1;
parameter POSTNTT = 3'd2;
parameter PREINTT = 3'd3;
parameter POSTINTT = 3'd4;



always @(posedge clk or posedge rst) begin
    if(rst) begin
        cnt <= 8'd0;
        ram_wen <= 1'd0;
        ram_wdata <= 96'd0;
        DATAORDERING_finish <= 1'd0;
        mode_reg <= INIT;
    end
    else begin
        if(active)  begin
            mode_reg <= mode;
            cnt <= 8'd0;
        end
        else if(mode_reg != INIT) begin
            cnt <= cnt + 8'd1;
        end
    end
end

always @(*) begin

    case(mode_reg)
        PRENTT : begin
            ram_raddr = ram_r_offset + cnt;
            ram_waddr = ram_w_offset + cnt - 8'd64;
            if(cnt<8'd64)
                u_v_buffer[cnt] = ram_rdata;
            else begin
                ram_wen = 1'd1;
                case(cnt)
                    8'd064: ram_wdata = {u_v_buffer[24][83:72],u_v_buffer[24][95:84],u_v_buffer[08][83:72],u_v_buffer[08][95:84],u_v_buffer[16][83:72],u_v_buffer[16][95:84],u_v_buffer[00][83:72],u_v_buffer[00][95:84]};
                    8'd065: ram_wdata = {u_v_buffer[24][59:48],u_v_buffer[24][71:60],u_v_buffer[08][59:48],u_v_buffer[08][71:60],u_v_buffer[16][59:48],u_v_buffer[16][71:60],u_v_buffer[00][59:48],u_v_buffer[00][71:60]};
                    8'd066: ram_wdata = {u_v_buffer[24][35:24],u_v_buffer[24][47:36],u_v_buffer[08][35:24],u_v_buffer[08][47:36],u_v_buffer[16][35:24],u_v_buffer[16][47:36],u_v_buffer[00][35:24],u_v_buffer[00][47:36]};
                    8'd067: ram_wdata = {u_v_buffer[24][11:00],u_v_buffer[24][23:12],u_v_buffer[08][11:00],u_v_buffer[08][23:12],u_v_buffer[16][11:00],u_v_buffer[16][23:12],u_v_buffer[00][11:00],u_v_buffer[00][23:12]};
                    8'd068: ram_wdata = {u_v_buffer[25][83:72],u_v_buffer[25][95:84],u_v_buffer[09][83:72],u_v_buffer[09][95:84],u_v_buffer[17][83:72],u_v_buffer[17][95:84],u_v_buffer[01][83:72],u_v_buffer[01][95:84]};
                    8'd069: ram_wdata = {u_v_buffer[25][59:48],u_v_buffer[25][71:60],u_v_buffer[09][59:48],u_v_buffer[09][71:60],u_v_buffer[17][59:48],u_v_buffer[17][71:60],u_v_buffer[01][59:48],u_v_buffer[01][71:60]};
                    8'd070: ram_wdata = {u_v_buffer[25][35:24],u_v_buffer[25][47:36],u_v_buffer[09][35:24],u_v_buffer[09][47:36],u_v_buffer[17][35:24],u_v_buffer[17][47:36],u_v_buffer[01][35:24],u_v_buffer[01][47:36]};
                    8'd071: ram_wdata = {u_v_buffer[25][11:00],u_v_buffer[25][23:12],u_v_buffer[09][11:00],u_v_buffer[09][23:12],u_v_buffer[17][11:00],u_v_buffer[17][23:12],u_v_buffer[01][11:00],u_v_buffer[01][23:12]};
                    8'd072: ram_wdata = {u_v_buffer[26][83:72],u_v_buffer[26][95:84],u_v_buffer[10][83:72],u_v_buffer[10][95:84],u_v_buffer[18][83:72],u_v_buffer[18][95:84],u_v_buffer[02][83:72],u_v_buffer[02][95:84]};
                    8'd073: ram_wdata = {u_v_buffer[26][59:48],u_v_buffer[26][71:60],u_v_buffer[10][59:48],u_v_buffer[10][71:60],u_v_buffer[18][59:48],u_v_buffer[18][71:60],u_v_buffer[02][59:48],u_v_buffer[02][71:60]};
                    8'd074: ram_wdata = {u_v_buffer[26][35:24],u_v_buffer[26][47:36],u_v_buffer[10][35:24],u_v_buffer[10][47:36],u_v_buffer[18][35:24],u_v_buffer[18][47:36],u_v_buffer[02][35:24],u_v_buffer[02][47:36]};
                    8'd075: ram_wdata = {u_v_buffer[26][11:00],u_v_buffer[26][23:12],u_v_buffer[10][11:00],u_v_buffer[10][23:12],u_v_buffer[18][11:00],u_v_buffer[18][23:12],u_v_buffer[02][11:00],u_v_buffer[02][23:12]};
                    8'd076: ram_wdata = {u_v_buffer[27][83:72],u_v_buffer[27][95:84],u_v_buffer[11][83:72],u_v_buffer[11][95:84],u_v_buffer[19][83:72],u_v_buffer[19][95:84],u_v_buffer[03][83:72],u_v_buffer[03][95:84]};
                    8'd077: ram_wdata = {u_v_buffer[27][59:48],u_v_buffer[27][71:60],u_v_buffer[11][59:48],u_v_buffer[11][71:60],u_v_buffer[19][59:48],u_v_buffer[19][71:60],u_v_buffer[03][59:48],u_v_buffer[03][71:60]};
                    8'd078: ram_wdata = {u_v_buffer[27][35:24],u_v_buffer[27][47:36],u_v_buffer[11][35:24],u_v_buffer[11][47:36],u_v_buffer[19][35:24],u_v_buffer[19][47:36],u_v_buffer[03][35:24],u_v_buffer[03][47:36]};
                    8'd079: ram_wdata = {u_v_buffer[27][11:00],u_v_buffer[27][23:12],u_v_buffer[11][11:00],u_v_buffer[11][23:12],u_v_buffer[19][11:00],u_v_buffer[19][23:12],u_v_buffer[03][11:00],u_v_buffer[03][23:12]};
                    8'd080: ram_wdata = {u_v_buffer[28][83:72],u_v_buffer[28][95:84],u_v_buffer[12][83:72],u_v_buffer[12][95:84],u_v_buffer[20][83:72],u_v_buffer[20][95:84],u_v_buffer[04][83:72],u_v_buffer[04][95:84]};
                    8'd081: ram_wdata = {u_v_buffer[28][59:48],u_v_buffer[28][71:60],u_v_buffer[12][59:48],u_v_buffer[12][71:60],u_v_buffer[20][59:48],u_v_buffer[20][71:60],u_v_buffer[04][59:48],u_v_buffer[04][71:60]};
                    8'd082: ram_wdata = {u_v_buffer[28][35:24],u_v_buffer[28][47:36],u_v_buffer[12][35:24],u_v_buffer[12][47:36],u_v_buffer[20][35:24],u_v_buffer[20][47:36],u_v_buffer[04][35:24],u_v_buffer[04][47:36]};
                    8'd083: ram_wdata = {u_v_buffer[28][11:00],u_v_buffer[28][23:12],u_v_buffer[12][11:00],u_v_buffer[12][23:12],u_v_buffer[20][11:00],u_v_buffer[20][23:12],u_v_buffer[04][11:00],u_v_buffer[04][23:12]};
                    8'd084: ram_wdata = {u_v_buffer[29][83:72],u_v_buffer[29][95:84],u_v_buffer[13][83:72],u_v_buffer[13][95:84],u_v_buffer[21][83:72],u_v_buffer[21][95:84],u_v_buffer[05][83:72],u_v_buffer[05][95:84]};
                    8'd085: ram_wdata = {u_v_buffer[29][59:48],u_v_buffer[29][71:60],u_v_buffer[13][59:48],u_v_buffer[13][71:60],u_v_buffer[21][59:48],u_v_buffer[21][71:60],u_v_buffer[05][59:48],u_v_buffer[05][71:60]};
                    8'd086: ram_wdata = {u_v_buffer[29][35:24],u_v_buffer[29][47:36],u_v_buffer[13][35:24],u_v_buffer[13][47:36],u_v_buffer[21][35:24],u_v_buffer[21][47:36],u_v_buffer[05][35:24],u_v_buffer[05][47:36]};
                    8'd087: ram_wdata = {u_v_buffer[29][11:00],u_v_buffer[29][23:12],u_v_buffer[13][11:00],u_v_buffer[13][23:12],u_v_buffer[21][11:00],u_v_buffer[21][23:12],u_v_buffer[05][11:00],u_v_buffer[05][23:12]};
                    8'd088: ram_wdata = {u_v_buffer[30][83:72],u_v_buffer[30][95:84],u_v_buffer[14][83:72],u_v_buffer[14][95:84],u_v_buffer[22][83:72],u_v_buffer[22][95:84],u_v_buffer[06][83:72],u_v_buffer[06][95:84]};
                    8'd089: ram_wdata = {u_v_buffer[30][59:48],u_v_buffer[30][71:60],u_v_buffer[14][59:48],u_v_buffer[14][71:60],u_v_buffer[22][59:48],u_v_buffer[22][71:60],u_v_buffer[06][59:48],u_v_buffer[06][71:60]};
                    8'd090: ram_wdata = {u_v_buffer[30][35:24],u_v_buffer[30][47:36],u_v_buffer[14][35:24],u_v_buffer[14][47:36],u_v_buffer[22][35:24],u_v_buffer[22][47:36],u_v_buffer[06][35:24],u_v_buffer[06][47:36]};
                    8'd091: ram_wdata = {u_v_buffer[30][11:00],u_v_buffer[30][23:12],u_v_buffer[14][11:00],u_v_buffer[14][23:12],u_v_buffer[22][11:00],u_v_buffer[22][23:12],u_v_buffer[06][11:00],u_v_buffer[06][23:12]};
                    8'd092: ram_wdata = {u_v_buffer[31][83:72],u_v_buffer[31][95:84],u_v_buffer[15][83:72],u_v_buffer[15][95:84],u_v_buffer[23][83:72],u_v_buffer[23][95:84],u_v_buffer[07][83:72],u_v_buffer[07][95:84]};
                    8'd093: ram_wdata = {u_v_buffer[31][59:48],u_v_buffer[31][71:60],u_v_buffer[15][59:48],u_v_buffer[15][71:60],u_v_buffer[23][59:48],u_v_buffer[23][71:60],u_v_buffer[07][59:48],u_v_buffer[07][71:60]};
                    8'd094: ram_wdata = {u_v_buffer[31][35:24],u_v_buffer[31][47:36],u_v_buffer[15][35:24],u_v_buffer[15][47:36],u_v_buffer[23][35:24],u_v_buffer[23][47:36],u_v_buffer[07][35:24],u_v_buffer[07][47:36]};
                    8'd095: ram_wdata = {u_v_buffer[31][11:00],u_v_buffer[31][23:12],u_v_buffer[15][11:00],u_v_buffer[15][23:12],u_v_buffer[23][11:00],u_v_buffer[23][23:12],u_v_buffer[07][11:00],u_v_buffer[07][23:12]};

                    8'd096: ram_wdata = {u_v_buffer[56][83:72],u_v_buffer[56][95:84],u_v_buffer[40][83:72],u_v_buffer[40][95:84],u_v_buffer[48][83:72],u_v_buffer[48][95:84],u_v_buffer[32][83:72],u_v_buffer[32][95:84]};
                    8'd097: ram_wdata = {u_v_buffer[56][59:48],u_v_buffer[56][71:60],u_v_buffer[40][59:48],u_v_buffer[40][71:60],u_v_buffer[48][59:48],u_v_buffer[48][71:60],u_v_buffer[32][59:48],u_v_buffer[32][71:60]};
                    8'd098: ram_wdata = {u_v_buffer[56][35:24],u_v_buffer[56][47:36],u_v_buffer[40][35:24],u_v_buffer[40][47:36],u_v_buffer[48][35:24],u_v_buffer[48][47:36],u_v_buffer[32][35:24],u_v_buffer[32][47:36]};
                    8'd099: ram_wdata = {u_v_buffer[56][11:00],u_v_buffer[56][23:12],u_v_buffer[40][11:00],u_v_buffer[40][23:12],u_v_buffer[48][11:00],u_v_buffer[48][23:12],u_v_buffer[32][11:00],u_v_buffer[32][23:12]};
                    8'd100: ram_wdata = {u_v_buffer[57][83:72],u_v_buffer[57][95:84],u_v_buffer[41][83:72],u_v_buffer[41][95:84],u_v_buffer[49][83:72],u_v_buffer[49][95:84],u_v_buffer[33][83:72],u_v_buffer[33][95:84]};
                    8'd101: ram_wdata = {u_v_buffer[57][59:48],u_v_buffer[57][71:60],u_v_buffer[41][59:48],u_v_buffer[41][71:60],u_v_buffer[49][59:48],u_v_buffer[49][71:60],u_v_buffer[33][59:48],u_v_buffer[33][71:60]};
                    8'd102: ram_wdata = {u_v_buffer[57][35:24],u_v_buffer[57][47:36],u_v_buffer[41][35:24],u_v_buffer[41][47:36],u_v_buffer[49][35:24],u_v_buffer[49][47:36],u_v_buffer[33][35:24],u_v_buffer[33][47:36]};
                    8'd103: ram_wdata = {u_v_buffer[57][11:00],u_v_buffer[57][23:12],u_v_buffer[41][11:00],u_v_buffer[41][23:12],u_v_buffer[49][11:00],u_v_buffer[49][23:12],u_v_buffer[33][11:00],u_v_buffer[33][23:12]};
                    8'd104: ram_wdata = {u_v_buffer[58][83:72],u_v_buffer[58][95:84],u_v_buffer[42][83:72],u_v_buffer[42][95:84],u_v_buffer[50][83:72],u_v_buffer[50][95:84],u_v_buffer[34][83:72],u_v_buffer[34][95:84]};
                    8'd105: ram_wdata = {u_v_buffer[58][59:48],u_v_buffer[58][71:60],u_v_buffer[42][59:48],u_v_buffer[42][71:60],u_v_buffer[50][59:48],u_v_buffer[50][71:60],u_v_buffer[34][59:48],u_v_buffer[34][71:60]};
                    8'd106: ram_wdata = {u_v_buffer[58][35:24],u_v_buffer[58][47:36],u_v_buffer[42][35:24],u_v_buffer[42][47:36],u_v_buffer[50][35:24],u_v_buffer[50][47:36],u_v_buffer[34][35:24],u_v_buffer[34][47:36]};
                    8'd107: ram_wdata = {u_v_buffer[58][11:00],u_v_buffer[58][23:12],u_v_buffer[42][11:00],u_v_buffer[42][23:12],u_v_buffer[50][11:00],u_v_buffer[50][23:12],u_v_buffer[34][11:00],u_v_buffer[34][23:12]};
                    8'd108: ram_wdata = {u_v_buffer[59][83:72],u_v_buffer[59][95:84],u_v_buffer[43][83:72],u_v_buffer[43][95:84],u_v_buffer[51][83:72],u_v_buffer[51][95:84],u_v_buffer[35][83:72],u_v_buffer[35][95:84]};
                    8'd109: ram_wdata = {u_v_buffer[59][59:48],u_v_buffer[59][71:60],u_v_buffer[43][59:48],u_v_buffer[43][71:60],u_v_buffer[51][59:48],u_v_buffer[51][71:60],u_v_buffer[35][59:48],u_v_buffer[35][71:60]};
                    8'd110: ram_wdata = {u_v_buffer[59][35:24],u_v_buffer[59][47:36],u_v_buffer[43][35:24],u_v_buffer[43][47:36],u_v_buffer[51][35:24],u_v_buffer[51][47:36],u_v_buffer[35][35:24],u_v_buffer[35][47:36]};
                    8'd111: ram_wdata = {u_v_buffer[59][11:00],u_v_buffer[59][23:12],u_v_buffer[43][11:00],u_v_buffer[43][23:12],u_v_buffer[51][11:00],u_v_buffer[51][23:12],u_v_buffer[35][11:00],u_v_buffer[35][23:12]};
                    8'd112: ram_wdata = {u_v_buffer[60][83:72],u_v_buffer[60][95:84],u_v_buffer[44][83:72],u_v_buffer[44][95:84],u_v_buffer[52][83:72],u_v_buffer[52][95:84],u_v_buffer[36][83:72],u_v_buffer[36][95:84]};
                    8'd113: ram_wdata = {u_v_buffer[60][59:48],u_v_buffer[60][71:60],u_v_buffer[44][59:48],u_v_buffer[44][71:60],u_v_buffer[52][59:48],u_v_buffer[52][71:60],u_v_buffer[36][59:48],u_v_buffer[36][71:60]};
                    8'd114: ram_wdata = {u_v_buffer[60][35:24],u_v_buffer[60][47:36],u_v_buffer[44][35:24],u_v_buffer[44][47:36],u_v_buffer[52][35:24],u_v_buffer[52][47:36],u_v_buffer[36][35:24],u_v_buffer[36][47:36]};
                    8'd115: ram_wdata = {u_v_buffer[60][11:00],u_v_buffer[60][23:12],u_v_buffer[44][11:00],u_v_buffer[44][23:12],u_v_buffer[52][11:00],u_v_buffer[52][23:12],u_v_buffer[36][11:00],u_v_buffer[36][23:12]};
                    8'd116: ram_wdata = {u_v_buffer[61][83:72],u_v_buffer[61][95:84],u_v_buffer[45][83:72],u_v_buffer[45][95:84],u_v_buffer[53][83:72],u_v_buffer[53][95:84],u_v_buffer[37][83:72],u_v_buffer[37][95:84]};
                    8'd117: ram_wdata = {u_v_buffer[61][59:48],u_v_buffer[61][71:60],u_v_buffer[45][59:48],u_v_buffer[45][71:60],u_v_buffer[53][59:48],u_v_buffer[53][71:60],u_v_buffer[37][59:48],u_v_buffer[37][71:60]};
                    8'd118: ram_wdata = {u_v_buffer[61][35:24],u_v_buffer[61][47:36],u_v_buffer[45][35:24],u_v_buffer[45][47:36],u_v_buffer[53][35:24],u_v_buffer[53][47:36],u_v_buffer[37][35:24],u_v_buffer[37][47:36]};
                    8'd119: ram_wdata = {u_v_buffer[61][11:00],u_v_buffer[61][23:12],u_v_buffer[45][11:00],u_v_buffer[45][23:12],u_v_buffer[53][11:00],u_v_buffer[53][23:12],u_v_buffer[37][11:00],u_v_buffer[37][23:12]};
                    8'd120: ram_wdata = {u_v_buffer[62][83:72],u_v_buffer[62][95:84],u_v_buffer[46][83:72],u_v_buffer[46][95:84],u_v_buffer[54][83:72],u_v_buffer[54][95:84],u_v_buffer[38][83:72],u_v_buffer[38][95:84]};
                    8'd121: ram_wdata = {u_v_buffer[62][59:48],u_v_buffer[62][71:60],u_v_buffer[46][59:48],u_v_buffer[46][71:60],u_v_buffer[54][59:48],u_v_buffer[54][71:60],u_v_buffer[38][59:48],u_v_buffer[38][71:60]};
                    8'd122: ram_wdata = {u_v_buffer[62][35:24],u_v_buffer[62][47:36],u_v_buffer[46][35:24],u_v_buffer[46][47:36],u_v_buffer[54][35:24],u_v_buffer[54][47:36],u_v_buffer[38][35:24],u_v_buffer[38][47:36]};
                    8'd123: ram_wdata = {u_v_buffer[62][11:00],u_v_buffer[62][23:12],u_v_buffer[46][11:00],u_v_buffer[46][23:12],u_v_buffer[54][11:00],u_v_buffer[54][23:12],u_v_buffer[38][11:00],u_v_buffer[38][23:12]};
                    8'd124: ram_wdata = {u_v_buffer[63][83:72],u_v_buffer[63][95:84],u_v_buffer[47][83:72],u_v_buffer[47][95:84],u_v_buffer[55][83:72],u_v_buffer[55][95:84],u_v_buffer[39][83:72],u_v_buffer[39][95:84]};
                    8'd125: ram_wdata = {u_v_buffer[63][59:48],u_v_buffer[63][71:60],u_v_buffer[47][59:48],u_v_buffer[47][71:60],u_v_buffer[55][59:48],u_v_buffer[55][71:60],u_v_buffer[39][59:48],u_v_buffer[39][71:60]};
                    8'd126: ram_wdata = {u_v_buffer[63][35:24],u_v_buffer[63][47:36],u_v_buffer[47][35:24],u_v_buffer[47][47:36],u_v_buffer[55][35:24],u_v_buffer[55][47:36],u_v_buffer[39][35:24],u_v_buffer[39][47:36]};
                    8'd127: ram_wdata = {u_v_buffer[63][11:00],u_v_buffer[63][23:12],u_v_buffer[47][11:00],u_v_buffer[47][23:12],u_v_buffer[55][11:00],u_v_buffer[55][23:12],u_v_buffer[39][11:00],u_v_buffer[39][23:12]};
                endcase
            end
            if(cnt==8'd128) begin
                DATAORDERING_finish = 1'd1;
                ram_wen = 1'd0;
                mode_reg = INIT;
            end
        end

        POSTNTT : begin
                ram_raddr = ram_r_offset + cnt;
                ram_waddr = ram_w_offset + cnt;
                ram_wen = 1'd1;
                if(cnt<8'd64) begin
                    ram_wdata = {ram_rdata[11:0],ram_rdata[23:12],ram_rdata[35:24],ram_rdata[47:36],ram_rdata[59:48],ram_rdata[71:60],ram_rdata[83:72],ram_rdata[95:84]};
                end
                else if(cnt==8'd64) begin
                DATAORDERING_finish = 1'd1;
                ram_wen = 1'd0;
                mode_reg = INIT;
                end
            end

        POSTINTT : begin
            ram_raddr = ram_r_offset + cnt;
            ram_waddr = ram_w_offset + cnt - 8'd32;
            if(cnt<8'd32)
                u_v_buffer[cnt] = ram_rdata;
            else begin
                ram_wen = 1'd1;
                case(cnt)
                    8'd032: ram_wdata = {u_v_buffer[00][11:00],u_v_buffer[00][23:12],u_v_buffer[01][11:00],u_v_buffer[01][23:12],u_v_buffer[02][11:00],u_v_buffer[02][23:12],u_v_buffer[03][11:00],u_v_buffer[03][23:12]};
                    8'd033: ram_wdata = {u_v_buffer[04][11:00],u_v_buffer[04][23:12],u_v_buffer[05][11:00],u_v_buffer[05][23:12],u_v_buffer[06][11:00],u_v_buffer[06][23:12],u_v_buffer[07][11:00],u_v_buffer[07][23:12]};
                    8'd034: ram_wdata = {u_v_buffer[08][11:00],u_v_buffer[08][23:12],u_v_buffer[09][11:00],u_v_buffer[09][23:12],u_v_buffer[10][11:00],u_v_buffer[10][23:12],u_v_buffer[11][11:00],u_v_buffer[11][23:12]};
                    8'd035: ram_wdata = {u_v_buffer[12][11:00],u_v_buffer[12][23:12],u_v_buffer[13][11:00],u_v_buffer[13][23:12],u_v_buffer[14][11:00],u_v_buffer[14][23:12],u_v_buffer[15][11:00],u_v_buffer[15][23:12]};
                    8'd036: ram_wdata = {u_v_buffer[16][11:00],u_v_buffer[16][23:12],u_v_buffer[17][11:00],u_v_buffer[17][23:12],u_v_buffer[18][11:00],u_v_buffer[18][23:12],u_v_buffer[19][11:00],u_v_buffer[19][23:12]};
                    8'd037: ram_wdata = {u_v_buffer[20][11:00],u_v_buffer[20][23:12],u_v_buffer[21][11:00],u_v_buffer[21][23:12],u_v_buffer[22][11:00],u_v_buffer[22][23:12],u_v_buffer[23][11:00],u_v_buffer[23][23:12]};
                    8'd038: ram_wdata = {u_v_buffer[24][11:00],u_v_buffer[24][23:12],u_v_buffer[25][11:00],u_v_buffer[25][23:12],u_v_buffer[26][11:00],u_v_buffer[26][23:12],u_v_buffer[27][11:00],u_v_buffer[27][23:12]};
                    8'd039: ram_wdata = {u_v_buffer[28][11:00],u_v_buffer[28][23:12],u_v_buffer[29][11:00],u_v_buffer[29][23:12],u_v_buffer[30][11:00],u_v_buffer[30][23:12],u_v_buffer[31][11:00],u_v_buffer[31][23:12]};

                    8'd040: ram_wdata = {u_v_buffer[00][59:48],u_v_buffer[00][71:60],u_v_buffer[01][59:48],u_v_buffer[01][71:60],u_v_buffer[02][59:48],u_v_buffer[02][71:60],u_v_buffer[03][59:48],u_v_buffer[03][71:60]};
                    8'd041: ram_wdata = {u_v_buffer[04][59:48],u_v_buffer[04][71:60],u_v_buffer[05][59:48],u_v_buffer[05][71:60],u_v_buffer[06][59:48],u_v_buffer[06][71:60],u_v_buffer[07][59:48],u_v_buffer[07][71:60]};
                    8'd042: ram_wdata = {u_v_buffer[08][59:48],u_v_buffer[08][71:60],u_v_buffer[09][59:48],u_v_buffer[09][71:60],u_v_buffer[10][59:48],u_v_buffer[10][71:60],u_v_buffer[11][59:48],u_v_buffer[11][71:60]};
                    8'd043: ram_wdata = {u_v_buffer[12][59:48],u_v_buffer[12][71:60],u_v_buffer[13][59:48],u_v_buffer[13][71:60],u_v_buffer[14][59:48],u_v_buffer[14][71:60],u_v_buffer[15][59:48],u_v_buffer[15][71:60]};
                    8'd044: ram_wdata = {u_v_buffer[16][59:48],u_v_buffer[16][71:60],u_v_buffer[17][59:48],u_v_buffer[17][71:60],u_v_buffer[18][59:48],u_v_buffer[18][71:60],u_v_buffer[19][59:48],u_v_buffer[19][71:60]};
                    8'd045: ram_wdata = {u_v_buffer[20][59:48],u_v_buffer[20][71:60],u_v_buffer[21][59:48],u_v_buffer[21][71:60],u_v_buffer[22][59:48],u_v_buffer[22][71:60],u_v_buffer[23][59:48],u_v_buffer[23][71:60]};
                    8'd046: ram_wdata = {u_v_buffer[24][59:48],u_v_buffer[24][71:60],u_v_buffer[25][59:48],u_v_buffer[25][71:60],u_v_buffer[26][59:48],u_v_buffer[26][71:60],u_v_buffer[27][59:48],u_v_buffer[27][71:60]};
                    8'd047: ram_wdata = {u_v_buffer[28][59:48],u_v_buffer[28][71:60],u_v_buffer[29][59:48],u_v_buffer[29][71:60],u_v_buffer[30][59:48],u_v_buffer[30][71:60],u_v_buffer[31][59:48],u_v_buffer[31][71:60]};

                    8'd048: ram_wdata = {u_v_buffer[00][35:24],u_v_buffer[00][47:36],u_v_buffer[01][35:24],u_v_buffer[01][47:36],u_v_buffer[02][35:24],u_v_buffer[02][47:36],u_v_buffer[03][35:24],u_v_buffer[03][47:36]};
                    8'd049: ram_wdata = {u_v_buffer[04][35:24],u_v_buffer[04][47:36],u_v_buffer[05][35:24],u_v_buffer[05][47:36],u_v_buffer[06][35:24],u_v_buffer[06][47:36],u_v_buffer[07][35:24],u_v_buffer[07][47:36]};
                    8'd050: ram_wdata = {u_v_buffer[08][35:24],u_v_buffer[08][47:36],u_v_buffer[09][35:24],u_v_buffer[09][47:36],u_v_buffer[10][35:24],u_v_buffer[10][47:36],u_v_buffer[11][35:24],u_v_buffer[11][47:36]};
                    8'd051: ram_wdata = {u_v_buffer[12][35:24],u_v_buffer[12][47:36],u_v_buffer[13][35:24],u_v_buffer[13][47:36],u_v_buffer[14][35:24],u_v_buffer[14][47:36],u_v_buffer[15][35:24],u_v_buffer[15][47:36]};
                    8'd052: ram_wdata = {u_v_buffer[16][35:24],u_v_buffer[16][47:36],u_v_buffer[17][35:24],u_v_buffer[17][47:36],u_v_buffer[18][35:24],u_v_buffer[18][47:36],u_v_buffer[19][35:24],u_v_buffer[19][47:36]};
                    8'd053: ram_wdata = {u_v_buffer[20][35:24],u_v_buffer[20][47:36],u_v_buffer[21][35:24],u_v_buffer[21][47:36],u_v_buffer[22][35:24],u_v_buffer[22][47:36],u_v_buffer[23][35:24],u_v_buffer[23][47:36]};
                    8'd054: ram_wdata = {u_v_buffer[24][35:24],u_v_buffer[24][47:36],u_v_buffer[25][35:24],u_v_buffer[25][47:36],u_v_buffer[26][35:24],u_v_buffer[26][47:36],u_v_buffer[27][35:24],u_v_buffer[27][47:36]};
                    8'd055: ram_wdata = {u_v_buffer[28][35:24],u_v_buffer[28][47:36],u_v_buffer[29][35:24],u_v_buffer[29][47:36],u_v_buffer[30][35:24],u_v_buffer[30][47:36],u_v_buffer[31][35:24],u_v_buffer[31][47:36]};

                    8'd056: ram_wdata = {u_v_buffer[00][83:72],u_v_buffer[00][95:84],u_v_buffer[01][83:72],u_v_buffer[01][95:84],u_v_buffer[02][83:72],u_v_buffer[02][95:84],u_v_buffer[03][83:72],u_v_buffer[03][95:84]};
                    8'd057: ram_wdata = {u_v_buffer[04][83:72],u_v_buffer[04][95:84],u_v_buffer[05][83:72],u_v_buffer[05][95:84],u_v_buffer[06][83:72],u_v_buffer[06][95:84],u_v_buffer[07][83:72],u_v_buffer[07][95:84]};
                    8'd058: ram_wdata = {u_v_buffer[08][83:72],u_v_buffer[08][95:84],u_v_buffer[09][83:72],u_v_buffer[09][95:84],u_v_buffer[10][83:72],u_v_buffer[10][95:84],u_v_buffer[11][83:72],u_v_buffer[11][95:84]};
                    8'd059: ram_wdata = {u_v_buffer[12][83:72],u_v_buffer[12][95:84],u_v_buffer[13][83:72],u_v_buffer[13][95:84],u_v_buffer[14][83:72],u_v_buffer[14][95:84],u_v_buffer[15][83:72],u_v_buffer[15][95:84]};
                    8'd060: ram_wdata = {u_v_buffer[16][83:72],u_v_buffer[16][95:84],u_v_buffer[17][83:72],u_v_buffer[17][95:84],u_v_buffer[18][83:72],u_v_buffer[18][95:84],u_v_buffer[19][83:72],u_v_buffer[19][95:84]};
                    8'd061: ram_wdata = {u_v_buffer[20][83:72],u_v_buffer[20][95:84],u_v_buffer[21][83:72],u_v_buffer[21][95:84],u_v_buffer[22][83:72],u_v_buffer[22][95:84],u_v_buffer[23][83:72],u_v_buffer[23][95:84]};
                    8'd062: ram_wdata = {u_v_buffer[24][83:72],u_v_buffer[24][95:84],u_v_buffer[25][83:72],u_v_buffer[25][95:84],u_v_buffer[26][83:72],u_v_buffer[26][95:84],u_v_buffer[27][83:72],u_v_buffer[27][95:84]};
                    8'd063: ram_wdata = {u_v_buffer[28][83:72],u_v_buffer[28][95:84],u_v_buffer[29][83:72],u_v_buffer[29][95:84],u_v_buffer[30][83:72],u_v_buffer[30][95:84],u_v_buffer[31][83:72],u_v_buffer[31][95:84]};                                        
                endcase
            end
            if(cnt==8'd64) begin
                DATAORDERING_finish = 1'd1;
                ram_wen = 1'd0;
                mode_reg = INIT;
            end
        end
    endcase

end
endmodule