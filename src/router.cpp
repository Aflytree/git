#include "global.h"
#include "top.h"
using namespace std;

#define CLC_REG 0x5e000
#define CLC_REG_MASK 0xFFE00
#define FIFO_START 0x5D880
#define FIFO_END 0x5D88F

uint8 RLUT[2 * 1024] = {0};

CRouter::CRouter()
{
    memset(&RC, 0, sizeof(RC));
    memset(&Data_Ctrl_Reg, 0, sizeof(Data_Ctrl_Reg));
}

CRouter::~CRouter()
{
}
#pragma GCC push_options
#pragma GCC optimize("O0")
void CRouter::pack_cr_queue(int clk, CCluster *pcluster, CChip *pchip, int local_cr, int phase_en_count)
{
    CrCbUphead pack;
    uint32 AddrXYInBank;
    uint32 RelativeAddrXY;
    pack.CR_ID = ((pcluster->rc.RC.RI.core_addr_OUT) >> 8) & 0x3;
    pack.CORE_INDEX = pcluster->rc.RC.RI.core_addr_OUT & 0xFF;
    pack.IN_CORE = pcluster->rc.RC.RI.mem_addr_OUT;
    pack.WM = pcluster->rc.RC.RI.WM_OUT;
    pack.data1 = pcluster->rc.RC.RI.RN_OUT1;
    pack.data2 = pcluster->rc.RC.RI.RN_OUT2;
    pack.data3 = pcluster->rc.RC.RI.RN_OUT3;
    pack.data4 = pcluster->rc.RC.RI.RN_OUT4;
    pack.data5 = pcluster->rc.RC.RI.RN_OUT5;
    pack.data6 = pcluster->rc.RC.RI.RN_OUT6;
    pack.data7 = pcluster->rc.RC.RI.RN_OUT7;
    pack.data8 = pcluster->rc.RC.RI.RN_OUT8;
    //每个CB有三条bus连到本地CR，每一列NOC2为一组，core按此分组分别对应cr上的三个fifo:CB_NOC2_0/1/2_Fifo
    AddrXYInBank = pcluster->AddrXY % CORE_NUM_CB;
    RelativeAddrXY = (AddrXYInBank - 8) % 40;
    if ((AddrXYInBank < 8) || (RelativeAddrXY < 16))
    {
        pchip->ChipRouter[local_cr].CB_NOC2_0_Fifo.push(pack);
    }
    else if (RelativeAddrXY < 32)
    {
        pchip->ChipRouter[local_cr].CB_NOC2_1_Fifo.push(pack);
    }
    else if (RelativeAddrXY < 40)
    {
        pchip->ChipRouter[local_cr].CB_NOC2_2_Fifo.push(pack);
    }
    else
    {
#if debug_en
        if ((core_start <= pcluster->AddrXY) && (pcluster->AddrXY < core_end) && dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d AddrX:%d  AddrXY error!!!\r\n", phase_en_count, clk, pcluster->AddrXY);
        }
#endif
    }
}

void CRouter::pack_msg_queue(CCluster *pcluster, TypeData RI_in_D0)
{
    FifoType message;
    message.data1 = RI_in_D0.data1;
    message.data2 = RI_in_D0.data2;
    if (pcluster->clc.msgQ.size() <= 32)
    {
        fprintf(pcluster->printf_fd, "************* message write to fifo  **************\n");
        pcluster->clc.msgQ.push(message);
        fprintf(pcluster->printf_fd, "message data: 0x%04x%04x\n", pcluster->clc.msgQ.front().data2, pcluster->clc.msgQ.front().data1);
    }
    else
    {
        fprintf(pcluster->printf_fd, "************* message fifo is overflow !!!! **************\n");
        exit(-1);
    }
    message.data1 = 0;
    message.data2 = 0;
}

void CRouter::RcReceiveDatafromNoC(CBoard *pboard, CChip *pchip, CCluster *pcluster, TypeData *pRIinD0, uint32 *pRIWM0, uint32 *pRtWen0, uint32 *pRtincoreA, uint32 *pRtinmemA, int *pclk, uint32 *Rt_c_dest, int C_Ready0, int phase_en)
{
    int j = pcluster->AddrXY;
    if (pchip->NoC_cfifo.get_core_ofifo_num(j) == 0)
    {
        *pRtWen0 = 0;
        *pRIWM0 = 0;
        pRIinD0->data1 = 0x0000;
        pRIinD0->data2 = 0x0000;
        pRIinD0->data3 = 0x0000;
        pRIinD0->data4 = 0x0000;
        pRIinD0->data5 = 0x0000;
        pRIinD0->data6 = 0x0000;
        pRIinD0->data7 = 0x0000;
        pRIinD0->data8 = 0x0000;

        NoC_ocpkg[j].init();
        //printf("core ofifo num j=%d\n",j);
    }
    else if (!pcluster->ram.RC.bFailed)
    {
        cout << dec << "RcReceiveDatafromNoC---AddrXY=(dec)" << pcluster->AddrXY << endl;
#if debug_en
        if ((core_start <= pcluster->AddrXY) && (pcluster->AddrXY < core_end) && dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "clk: %d  queue empty =  %d\n", *pclk, pcluster->packQ.empty());
        }
#endif

        //////////////////////
        axi_data axi_tmp;
        pchip->NoC_cfifo.noc2core(&axi_tmp, j, j + 1);
        NoC_ocpkg[j].get_core_pkg(axi_tmp);
        pRIinD0->data1 = NoC_ocpkg[j].data[0] & 0xffff;
        pRIinD0->data2 = (NoC_ocpkg[j].data[0] >> 16) & 0xffff;
        pRIinD0->data3 = NoC_ocpkg[j].data[1] & 0xffff;
        pRIinD0->data4 = (NoC_ocpkg[j].data[1] >> 16) & 0xffff;
        pRIinD0->data5 = NoC_ocpkg[j].data[2] & 0xffff;
        pRIinD0->data6 = (NoC_ocpkg[j].data[2] >> 16) & 0xffff;
        pRIinD0->data7 = NoC_ocpkg[j].data[3] & 0xffff;
        pRIinD0->data8 = (NoC_ocpkg[j].data[3] >> 16) & 0xffff;
        *pRtincoreA = 0;
        *pRtinmemA = NoC_ocpkg[j].addr & 0x7ffff;
        *Rt_c_dest = NoC_ocpkg[j].c_des;
        *pRIWM0 = NoC_ocpkg[j].wm;
        *pRtWen0 = NoC_ocpkg[j].vld;

        cout << hex << "receive from queue Data:0x" << hex << pRIinD0->data8 << pRIinD0->data7 << pRIinD0->data6 << pRIinD0->data5 << pRIinD0->data4 << pRIinD0->data3 << pRIinD0->data2 << pRIinD0->data1 << " pRtincoreA=" << *pRtincoreA << " pRtinmemA=" << *pRtinmemA << endl;
#if debug_en
        if ((core_start <= pcluster->AddrXY) && (pcluster->AddrXY < core_end) && dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "receive from queue Data:0x%04x%04x%04x%04x%04x%04x%04x%04x pRtincoreA:0x%x pRtinmemA %x Rt_Wen:%d RI_WM0:%d Rt_Ready:%d\n", pRIinD0->data1, pRIinD0->data2, pRIinD0->data3, pRIinD0->data4, pRIinD0->data5, pRIinD0->data6, pRIinD0->data7, pRIinD0->data8, *pRtincoreA, *pRtinmemA, *pRtWen0, *pRIWM0, RC.Rt_Ready);
            fprintf(pcluster->printf_fd, "clk: %d  queue empty =  %d\n", *pclk, pcluster->packQ.empty());
        }
#endif
    }
#ifdef DUMPROUTERMSG
    if (NoC_ocpkg[j].vld)
    {
        (pboard->n2c)++;
#if debug_en
        //n2c++;
        if ((core_start <= pcluster->AddrXY) && (pcluster->AddrXY < core_end) && dump_inner_en)
        {
            //fprintf(fr,"Get: Phase=%d,%d,SrcCore=%d,DstCore=%ld,addr=0x%x,data0=0x%x\n",phase_en,NoC_ocpkg[j].data[6],NoC_ocpkg[j].data[7],j,NoC_ocpkg[j].addr&0x7ffff,NoC_ocpkg[j].data[0]&0xffff);
            fprintf(fr, "Get: Phase=%d,%d,clk=%d,SrcCore=%d,DstCore=%ld,addr=0x%x,data0=0x%x%x,data1=0x%x%x\n", phase_en, NoC_ocpkg[j].data[6], *pclk, NoC_ocpkg[j].data[7], j, NoC_ocpkg[j].addr & 0x7ffff, NoC_ocpkg[j].data[1], NoC_ocpkg[j].data[0], NoC_ocpkg[j].data[3], NoC_ocpkg[j].data[2]);
        }
#endif
    }
#endif
}

void CRouter::RcReceiveData(CCluster *pcluster, TypeData *pRIinD0, uint32 *pRIWM0, uint32 *pRtWen0, uint32 *pRtincoreA, uint32 *pRtinmemA, int *pclk, uint32 *Rt_c_dest)
{
#ifdef no_noc                         //接收queue中传来的数据
    if (pcluster->packQ.empty() != 0) // 队列空
    {
        *pRtWen0 = 0;
        *pRIWM0 = 0;
        pRIinD0->data1 = 0x0000;
        pRIinD0->data2 = 0x0000;
        pRIinD0->data3 = 0x0000;
        pRIinD0->data4 = 0x0000;
        pRIinD0->data5 = 0x0000;
        pRIinD0->data6 = 0x0000;
        pRIinD0->data7 = 0x0000;
        pRIinD0->data8 = 0x0000;
    }
    else //队列不空
    {
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "clk: %d  packQ_fifo_size_pre %d ,queue empty =  %d\n", *pclk, pcluster->packQ.size(), pcluster->packQ.empty());
        }
#endif
        //取出queue数据给rc接收端
        pRIinD0->data1 = pcluster->packQ.front().data1;
        pRIinD0->data2 = pcluster->packQ.front().data2;
        pRIinD0->data3 = pcluster->packQ.front().data3;
        pRIinD0->data4 = pcluster->packQ.front().data4;
        pRIinD0->data5 = pcluster->packQ.front().data5;
        pRIinD0->data6 = pcluster->packQ.front().data6;
        pRIinD0->data7 = pcluster->packQ.front().data7;
        pRIinD0->data8 = pcluster->packQ.front().data8;
        *pRtincoreA = pcluster->packQ.front().core_addr_OUT;
        *pRtinmemA = pcluster->packQ.front().mem_addr_OUT;
        *Rt_c_dest = pcluster->packQ.front().c_dest_OUT;
        *pRIWM0 = pcluster->packQ.front().WM_OUT;
        *pRtWen0 = 1;
        RC.WM = *pRIWM0;

        pcluster->packQ.pop();
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "clk: %d  packQ_fifo_size %d ,receive from queue Data:0x%04x%04x%04x%04x%04x%04x%04x%04x pRtincoreA:0x%x pRtinmemA %x Rt_Wen:%d RI_WM0:%d Rt_Ready:%d\n", *pclk, pcluster->packQ.size(), pRIinD0->data1, pRIinD0->data2, pRIinD0->data3, pRIinD0->data4, pRIinD0->data5, pRIinD0->data6, pRIinD0->data7, pRIinD0->data8, *pRtincoreA, *pRtinmemA, *pRtWen0, *pRIWM0, RC.Rt_Ready);
            fprintf(pcluster->printf_fd, "clk: %d  queue empty =  %d\n", *pclk, pcluster->packQ.empty());
        }
#endif
    }
#else
    //读取NoC发来的数据
    *pRIWM0 = RC.RI.WM_IN;
    *pRtWen0 = RC.Rt_Wen;
    *pRtincoreA = RC.RI.core_addr_IN;
    *pRtinmemA = RC.RI.mem_addr_IN;
    *Rt_c_dest = RC.RI.c_dest_IN;
    pRIinD0->data1 = RC.RI.RN_IN1;
    pRIinD0->data2 = RC.RI.RN_IN2;
    pRIinD0->data3 = RC.RI.RN_IN3;
    pRIinD0->data4 = RC.RI.RN_IN4;
    pRIinD0->data5 = RC.RI.RN_IN5;
    pRIinD0->data6 = RC.RI.RN_IN6;
    pRIinD0->data7 = RC.RI.RN_IN7;
    pRIinD0->data8 = RC.RI.RN_IN8;
#endif
}
#pragma GCC pop_options
void CRouter::ClockEvent(int clk, int reset, CCluster *pcluster, CChip *pchip, CBoard *pboard, int phase_en_count)
{
    //Bus Interface
    //以下定义BI与其他接口模块的变量

    OUT uint64 RLUT_busy_CIF;
    IN uint64 BN_data;
    IN uint8 BRen_RLUT;
    IN uint8 BWen_RLUT;
    OUT uint8 RWM;      //3b
    OUT uint8 RI_RdWt;  //1b
    OUT uint8 RRen_Mem; //1b
    OUT uint8 RWen_Mem; //1b
    OUT uint8 RWen_MC;  //1b
    OUT uint8 RRen_CLC; //1b
    OUT uint8 RWen_CLC; //1b
    OUT uint64 RI_in_A0;
    OUT uint64 Xo[2];
    IN uint64 Xi[2];
    TypeData RI_in_D0;
    TypeData RI_out_D0;
    uint16 fifo_data[8];
    uint16 fifo_out_data[8];
    Router_data_fifo Router_data_fifo;
    //以下定义RI模块内部的变量
    uint32 RI_WM0;     //Data Merge输出
    uint32 C_Ready0;   //RC模块外部输入
    uint32 pRtincoreA; //Data Merge输出
    uint32 pRtinmemA;
    uint32 Rt_Wen0;       //RC写Memory内部信号
    uint32 Rt_c_dest = 0; //
    uint32 CLC_Ren;       //Data Ctrl输出给Addr Dec
    uint32 local_cr;      //核阵列对应的本地CR编号
    uint64 Addr_CLC_Ren, Addr_CLC_RLUT, Addr_bus_RLUT;
    uint64 Xo_D, CLC_Wen_rt = 0, RC_busy, Addr_RLUT = 0x5d000, Wen, Ren, Dout_FIFO;
    OUT uint64 Ren_Mem = 0, Wen_Mem, Ren_CLC = 0, Wen_CLC;

    uint8 RLUT_Send_Finish = 0;
    int RI_out_H0_WM_tmp;
    //uint64 RI_out_H0_WM ;
    struct
    {
        uint64 Core_Addr : 11;
        uint64 Mem_Addr : 19;
        uint64 C_dest : 3;
        uint64 SNN : 1;
    } RI_out_H0; //从Data Ctrl模块的输出

    //监测Ph_en_RC使能信号，Ph_en_RC是脉冲信号

    if ((0 == last_Ph_en_RC) && (1 == RC.Ph_en_RC))
    {
        if (!Ph_en_RC_Flag)
        {
            stop_rd_head_addr_flag = 0;
            stop_rd_head_addr_flag_dly = 0;

            stop_rd_head_addr_flag_dly2 = 0;
            stop_rd_head_addr_flag_dly3 = 0;
            stop_rd_head_addr_flag_close = 1;
            RI_out_H0_WM = 0;
            RI_out_H0_WM_tmp = 0;
        }
        Ph_en_RC_Flag = 1;
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC start\r\n", phase_en_count, clk);
            fprintf(pcluster->printf_fd, "Data_Ctrl_Reg M_RLUT:0x%x CN_LOOP:0x%x  CN_data:0x%x CN_loop1:0x%x CN_loop_step:0x%x ",
                    Data_Ctrl_Reg.M_RLUT, Data_Ctrl_Reg.CN_loop, Data_Ctrl_Reg.CN_data, Data_Ctrl_Reg.CN_loop1, Data_Ctrl_Reg.CN_loop_step);
        }
#endif
    }
    else
    {
        stop_rd_head_addr_flag_close = 0;
    }
    last_Ph_en_RC = RC.Ph_en_RC;
    if (pcluster->ram.RC.bFailed)
    {
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d AddrX:%d  ram.RC.bFailed!!!\r\n", phase_en_count, clk, pcluster->AddrXY);
        }
#endif
        RC.CIF_Wen = 0;
        return;
    }

    //RC根据memory和clc的busy标志位，向NoC的节点产生C_Ready信号
    //C_Ready0 = !(RC.Busy_Mem || RC.Busy_CLC);
    C_Ready0 = 1;
    RC.C_Ready = C_Ready0;
#ifdef no_noc
    RC.Rt_Ready = 1;
#endif
#ifdef ARTERISNOC
    RC.Rt_Ready = 1;
#endif

    RC.CIF_Wen = 0;

    if (RC.C_Ready == 0)
    {
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "Memory or CLC busy, return\r\n");
        }
#endif
        return;
    }

    if (RC.Rt_Wen == 0 && RC.Rt_Ready == 0)
    {
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "NoC node is not ready, return\r\n");
        }
#endif
        return;
    }

#ifdef no_noc //接收queue中传来的数据
    RcReceiveData(pcluster, &RI_in_D0, &RI_WM0, &Rt_Wen0, &pRtincoreA, &pRtinmemA, &clk, &Rt_c_dest);
#endif
#ifdef ARTERISNOC
    RcReceiveDatafromNoC(pboard, pchip, pcluster, &RI_in_D0, &RI_WM0, &Rt_Wen0, &pRtincoreA, &pRtinmemA, &clk, &Rt_c_dest, C_Ready0, phase_en_count);
#endif
    //

    if (Data_Ctrl_Reg.RX_start)
    {
        rx_start_flag = 1;
    }
    if (rx_start_flag)
    {
        if (Data_Ctrl_Reg.N_received <= Data_Ctrl_Reg.N_received_done)
        {
            Data_Ctrl_Reg.RX_start = 0;
            Data_Ctrl_Reg.RX_busy = 0;
            Data_Ctrl_Reg.N_received_done = 0;
            Data_Ctrl_Reg.N_received = 0;
            rx_start_flag = 0;
            rx_valid = 0;
        }
        else
        {
            Data_Ctrl_Reg.RX_busy = 1;
            if (Rt_Wen0)
            {
                rx_valid = 1;
                if (RI_WM0 == 0)
                {
                    Data_Ctrl_Reg.N_received_done += 8;
                }
                else
                {
                    Data_Ctrl_Reg.N_received_done += RI_WM0;
                }
            }
            else
            {
                rx_valid = 0;
            }
        }
    }
    //
    CLC_Ren = Ph_en_RC_Flag && !Rt_Wen0;

    Addr_CLC_RLUT = Data_Ctrl_Reg.Addr_base_RLUT; //RC读取Memory4的RLUT地址
    //if(!Data_Send_Finish)
    //{
    if (Data_Ctrl_Reg.N_BC == 1)
    {
        Addr_RLUT = Addr_CLC_RLUT;
    }
    Addr_CLC_Ren = Data_Ctrl_Reg.Addr_base_Rd; //RC读取Memory普通数据地址

    if (Ph_en_RC_Flag && (RC.Ren_Mem || RC.Ren_CLC) && Rt_Wen0) //如果上一拍RC在读Memory，而这一拍RC要写数据，则RC发送过程被打断|| stop_rd_head_addr_flag_close
    {
        b_interupt = 1;
    }
    if ((Ph_en_RC_Flag == 1) && (!Rt_Wen0) && (i3 == 0) && (j3 == 0) && (k3 == 0))
    {
        stop_rd_head_addr_flag_pre = 0;
    }

    /*
     if(b_interupt && !Rt_Wen0)
     {
        stop_rd_head_addr_flag_inter =1;
     } 
     else
     {
         stop_rd_head_addr_flag_inter =0;

     }
     */
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d STARTAddr_CLC_RLUT %x, Addr_CLC_Ren %x Data_Ctrl_Reg.N_received %d N_received_done %d rx_start_flag %d RX_busy %d b_interupt %d Rt_Wen0 %d\r\n",
                phase_en_count, clk, Addr_CLC_RLUT, Addr_CLC_Ren, Data_Ctrl_Reg.N_received, Data_Ctrl_Reg.N_received_done, rx_start_flag, Data_Ctrl_Reg.RX_busy, b_interupt, Rt_Wen0);
    }
#endif
    //Data Ctrl
    if (0 == Ph_en_RC_Flag)
    {
        i1 = 0;
        j1 = 0;
        k1 = 0;
        i0 = 0;
        i2 = 0;
        n2 = 0;
        n1 = 0;
        i3 = 0;
        j3 = 0;
        k3 = 0;
        m3 = 0;
        n3 = 0;
        n4 = 0;
    }
    else if ((1 == RC.Rt_Ready) && !(Rt_Wen0) && (0 == b_interupt) && (!Data_Send_Finish_dly)) //这里产生RC读数据的地址
    {

#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d BD_S %d BD:%d KW:%d KH:%d Addr_base_Rd:0x%x i0 %d CN_data %d\r\n", phase_en_count, clk, Data_Ctrl_Reg.BD_S, Data_Ctrl_Reg.BD, Data_Ctrl_Reg.KW, Data_Ctrl_Reg.KH, Data_Ctrl_Reg.Addr_base_Rd, i0, Data_Ctrl_Reg.CN_data);
        }
#endif

        if (i1 >= (Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid)

        {

            j1++;
            i1 = 0;
        }

        if (j1 >= Data_Ctrl_Reg.KW)
        {
            k1++;
            j1 = 0;
        }

        //根据KH、KW、BD的配置，循环产生RC读Memory的地址
        if (k1 < Data_Ctrl_Reg.KH)
        {
            if (j1 < Data_Ctrl_Reg.KW)
            {
                if (i1 < (Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid)
                {
                    if (((Data_Ctrl_Reg.X0 + j1 * Data_Ctrl_Reg.K_dilation) < 0) || ((Data_Ctrl_Reg.Y0 + k1 * Data_Ctrl_Reg.K_dilation) < 0) || ((Data_Ctrl_Reg.X0 + j1 * Data_Ctrl_Reg.K_dilation) > Data_Ctrl_Reg.BW) || ((Data_Ctrl_Reg.Y0 + k1 * Data_Ctrl_Reg.K_dilation) > Data_Ctrl_Reg.BH))
                    {
                        //Addr_CLC_Ren = Data_Ctrl_Reg.Addr_all_0;
                        RC.Xo_l = 0;
                        RC.Xo_h = 0;
                        i1++;
                    }
                    else
                    {
                        if ((Data_Ctrl_Reg.BD_S - (i1 + 1) * N_data_valid) >= 0)
                        {
                            pk_data_valid = N_data_valid;
                        }
                        else
                        {
                            pk_data_valid = Data_Ctrl_Reg.BD_S % N_data_valid;
                        }
#if debug_en
                        if (dump_inner_en)
                        {

                            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc data:%x Addr_base_Rd:%d i1:%d j1:%d k1:%d BW %d BD %d pk_data_valid_pre %d n1 %d\r\n",
                                    phase_en_count, clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i1, j1, k1, Data_Ctrl_Reg.BW, Data_Ctrl_Reg.BD, pk_data_valid, n1);
                        }
#endif
                        Addr_CLC_Ren = Data_Ctrl_Reg.Addr_base_Rd + 2 * N_data_valid * i1 + j1 * Data_Ctrl_Reg.K_dilation * (Data_Ctrl_Reg.BD * 2) + k1 * Data_Ctrl_Reg.BW * Data_Ctrl_Reg.K_dilation * (Data_Ctrl_Reg.BD * 2) + 2 * (Data_Ctrl_Reg.X0 + Data_Ctrl_Reg.Y0 * Data_Ctrl_Reg.BW) * Data_Ctrl_Reg.BD;
                        i1++;
                    }
                }
            }
        }
        else
        {

            if ((n2 == Data_Ctrl_Reg.N_BC))
            {
                Data_Send_Finish = 1;
#if debug_en
                if (dump_inner_en)
                {
                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC Data_Send_FinishRAM Addr is from clc data:%d \r\n", Data_Send_Finish);
                }
#endif
            }
        }

#if debug_en
        if (dump_inner_en)
        {
            // fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc data:%x Addr_base_Rd:%d i1:%d j1:%d k1:%d BW %d BD %d pk_data_valid_pre %d n1 %d\r\n",
            //     phase_en_count,clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i1, j1, k1,Data_Ctrl_Reg.BW,Data_Ctrl_Reg.BD,pk_data_valid,n1);
        }
#endif
        //[zaf]
        if (1 == Data_Ctrl_Reg.N_BC)
        {
            if (n1 == ((Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid) * Data_Ctrl_Reg.KH * Data_Ctrl_Reg.KW - 1)
            {
                n1 = 0;
                i1 = 0;
                j1 = 0;
                k1 = 0;
                Data_Send_Finish = 1;
            }
            else
            {
                n1++;
            }
        }
        else
        {
            if (n1 == ((Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid) * Data_Ctrl_Reg.KH * Data_Ctrl_Reg.KW - 1)
            {
                n1 = 0;
                i1 = 0;
                j1 = 0;
                k1 = 0;
            }
            else
            {
                n1++;
            }
        }
    }
    else if (b_interupt && !Rt_Wen0) //RC读数据被打断，需要重新读数据
    {
        Addr_CLC_Ren = last_data_address;
        //b_interupt = 0;
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d Last Read Mem was interrupted, Read agin, Address:0x%x\n", phase_en_count, clk, Addr_CLC_Ren);
        }
#endif
    }

    if ((0 == Rt_Wen0) && (1 == Ph_en_RC_Flag)) //Risc-v启动的RC发送
    {
        RI_in_A0 = Addr_CLC_Ren;
        last_data_address = RI_in_A0;
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc addr:%x i1:%d j1:%d k1:%d\r\n", phase_en_count, clk, RI_in_A0, i1, j1, k1);
        }
#endif
    }
    else if (1 == Rt_Wen0) //RC收到来自NoC的数据包
    {
        RI_in_A0 = pRtinmemA; //Rt_in_A;
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC revieve data from noc, Addr:0x%x WM:%d Rt_Wen0:%d ", phase_en_count, clk, RI_in_A0, RI_WM0, Rt_Wen0);

            if (RI_WM0 == 1)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x\r\n", RI_in_D0.data1);
            }
            else if (2 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2);
            }
            else if (3 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3);
            }
            else if (4 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3, RI_in_D0.data4);
            }
            else if (5 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3, RI_in_D0.data4,
                        RI_in_D0.data5);
            }
            else if (6 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3, RI_in_D0.data4,
                        RI_in_D0.data5, RI_in_D0.data6);
            }
            else if (7 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3, RI_in_D0.data4,
                        RI_in_D0.data5, RI_in_D0.data6, RI_in_D0.data7);
            }
            else if (0 == RI_WM0)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x%04x\r\n", RI_in_D0.data1, RI_in_D0.data2, RI_in_D0.data3, RI_in_D0.data4,
                        RI_in_D0.data5, RI_in_D0.data6, RI_in_D0.data7, RI_in_D0.data8);
            }
        }
#endif
    }

    //Ctrl_CIF
    RI_RdWt = !(Rt_Wen0 || CLC_Ren);
    //Xo = RC.Xo;
    Xo[0] = RC.Xo_l;
    Xo[1] = RC.Xo_h;

    if ((1 == CLC_Ren_delay) && !Rt_Wen0 && !Data_Send_Finish_dly2) //处理RC从Memory读取到的数据
    {
        RC.WM = 8; //pk_data_valid_dly;//fp16;//(Data_Ctrl_Reg.CN_data & 0x7);//wm
        RC.Ai = RI_in_A0;
        //将RC从Memory中读取到的数据填充到DATA_OUT
        RI_out_D0.data1 = Xo[0] & 0xFFFF;
        RI_out_D0.data2 = (Xo[0] >> 16) & 0xFFFF;
        RI_out_D0.data3 = (Xo[0] >> 32) & 0xFFFF;
        RI_out_D0.data4 = (Xo[0] >> 48) & 0xFFFF;
        RI_out_D0.data5 = (Xo[1]) & 0xFFFF;
        RI_out_D0.data6 = (Xo[1] >> 16) & 0xFFFF;
        RI_out_D0.data7 = (Xo[1] >> 32) & 0xFFFF;
        RI_out_D0.data8 = (Xo[1] >> 48) & 0xFFFF;
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC last RC.WM: %d RC.Ai %d\r\n", phase_en_count, clk, RC.WM, RC.Ai);
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC last read Memory Data:0x%04x%04x%04x%04x%04x%04x%04x%04x\r\n", phase_en_count, clk,
                    RI_out_D0.data1, RI_out_D0.data2, RI_out_D0.data3, RI_out_D0.data4, RI_out_D0.data5, RI_out_D0.data6, RI_out_D0.data7, RI_out_D0.data8);
        }
#endif
        //fifo
        fifo_data[0] = RI_out_D0.data1;
        fifo_data[1] = RI_out_D0.data2;
        fifo_data[2] = RI_out_D0.data3;
        fifo_data[3] = RI_out_D0.data4;
        fifo_data[4] = RI_out_D0.data5;
        fifo_data[5] = RI_out_D0.data6;
        fifo_data[6] = RI_out_D0.data7;
        fifo_data[7] = RI_out_D0.data8;
        int pk_data_valid_tmp = (pk_data_valid_dly == 0) ? 8 : pk_data_valid_dly;
        fprintf(pcluster->printf_fd, "[zaf]phase_en_count:%d,clk:%d,pk_data_valid_tmp ======================%d\n", phase_en_count, clk, pk_data_valid_tmp);
        for (int i = 0; i < pk_data_valid_tmp; i++)
        {
            Router_data_fifo.data = fifo_data[i];
            Router_data_fifoQ.push(Router_data_fifo);
        }
#if debug_en
        if (dump_inner_en)
        {
            int size = Router_data_fifoQ.size();
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC last fifo_data size %d pk_data_valid_tmp %d Data:0x%04x%04x%04x%04x%04x%04x%04x%04x\r\n", phase_en_count, clk, size, pk_data_valid_tmp,
                    fifo_data[0], fifo_data[1], fifo_data[2], fifo_data[3], fifo_data[4], fifo_data[5], fifo_data[6], fifo_data[7]);
        }
#endif
    }
    else //解析NOC发来的数据包，取出WM、存储地址、以及要写的数据
    {
#if (defined no_noc) || (defined ARTERISNOC)
        RC.WM = RI_WM0;
#else
        RC.WM = RC.RI.WM_IN;
#endif
        RC.Ai = RI_in_A0; //delet
        RC.Xi_l = RI_in_D0.data1 | (((uint64)(RI_in_D0.data2)) << 16) | (((uint64)(RI_in_D0.data3)) << 32) | (((uint64)(RI_in_D0.data4)) << 48);
        RC.Xi_h = RI_in_D0.data5 | (((uint64)(RI_in_D0.data6)) << 16) | (((uint64)(RI_in_D0.data7)) << 32) | (((uint64)(RI_in_D0.data8)) << 48);
    }

    //RC_busy = Rt_Wen0 || CLC_Ren;

#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d Addr_RLUTRLUT RLUT_Send_Finish n2 %d Addr_RLUT %x Addr_CLC_RLUT %x\r\n", phase_en_count, clk, n2, Addr_RLUT, Addr_CLC_RLUT);
    }
#endif

#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "Ph_en_RC_Flag:%d Rt_Wen0:%d RC.Ren_Mem:%d RC.Ren_CLC:%d Data_Ctrl_Reg.N_BC %d stop_rd_head_addr_flag %d stop_rd_head_addr_flag_dly %d stop_rd_head_addr_flag_inter %d\r\n",
                Ph_en_RC_Flag_delay, Rt_Wen0, RC.Ren_Mem, RC.Ren_CLC, Data_Ctrl_Reg.N_BC, stop_rd_head_addr_flag, stop_rd_head_addr_flag_dly2, stop_rd_head_addr_flag_inter);
    }
#endif
    Xo_D = RC.RLUT_Data;
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "Read From Mem4 Data:0x%016llx, M_RLUT:%d CN_data:%d CN_loop:%d CN_loop_step:%d CN_loop1:%d CN_loop_step1:%d i3:%d\r\n",
                Xo_D, Data_Ctrl_Reg.M_RLUT, Data_Ctrl_Reg.CN_data, Data_Ctrl_Reg.CN_loop, Data_Ctrl_Reg.CN_loop_step, Data_Ctrl_Reg.CN_loop1, Data_Ctrl_Reg.CN_loop_step, i3);
    }
#endif

    RI_out_H0.Core_Addr = (Xo_D >> 23) & 0x7FF;
    core_addr_reg = RI_out_H0.Core_Addr;
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "cccphase_en_count: %d,clk:%d , CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d ",
                phase_en_count, clk, RC.CIF_Wen, RC.C_Ready, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);
    }
#endif

    RI_out_H0.C_dest = (Xo_D >> 1) & 0x7;
    RI_out_H0.Mem_Addr = ((Xo_D >> 4) & 0x7FFFF) + Data_Ctrl_Reg.Addr_var_D; //18bit  17~0

    if (0 == Ph_en_RC_Flag_delay)
    {
        i3 = 0;
        j3 = 0;
    }
    /*以下代码通过RLUT模式的配置，根据不同的模式修改包头里的地址*/
    else if ((!Rt_Wen0) && (1 == Ph_en_RC_Flag_delay) && (RC.Ren_Mem || RC.Ren_CLC) && (((!b_interupt) && (!stop_rd_head_addr_flag)))) //Here is calculate the header address
    {
        /*
        Xo_D = RC.RLUT_Data;
#if debug_en
        if(dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "Read From Mem4 Data:0x%016llx, M_RLUT:%d CN_data:%d CN_loop:%d CN_loop_step:%d CN_loop1:%d CN_loop_step1:%d i3:%d\r\n", 
                   Xo_D, Data_Ctrl_Reg.M_RLUT, Data_Ctrl_Reg.CN_data, Data_Ctrl_Reg.CN_loop, Data_Ctrl_Reg.CN_loop_step, Data_Ctrl_Reg.CN_loop1, Data_Ctrl_Reg.CN_loop_step ,i3);
        }
#endif

        RI_out_H0.Core_Addr = (Xo_D >> 23) & 0x7FF; 
        core_addr_reg = RI_out_H0.Core_Addr;
#if debug_en
                if(dump_inner_en)
                {
                   fprintf(pcluster->printf_fd, "cccphase_en_count: %d,clk:%d , CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d ", 
                   phase_en_count,clk, RC.CIF_Wen, RC.C_Ready, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);              
                }
#endif
      
        RI_out_H0.C_dest    = (Xo_D >> 1) & 0x7;
        RI_out_H0.Mem_Addr  = ((Xo_D >> 4) & 0x7FFFF) + Data_Ctrl_Reg.Addr_var_D;        //18bit  17~0
        */
        if ((i3 == 0) && (j3 == 0) && (k3 == 0)) //&&(Data_Ctrl_Reg.N_BC ==1))
        {
            mem_addr_des_dly = RI_out_H0.Mem_Addr;
            mem_addr_des = RI_out_H0.Mem_Addr;
        }
        int CN_data_num_cnt = 0;
        CN_data_num_cnt = ((Data_Ctrl_Reg.CN_data + N_data_valid - 1) / N_data_valid);
        /*
        if(Data_Ctrl_Reg.CN_data <Data_Ctrl_Reg.BD_S)
        {
            CN_data_num_cnt = ((Data_Ctrl_Reg.CN_data + N_data_valid -1)/N_data_valid);
        }
        else
        {
            CN_data_num_cnt = (Data_Ctrl_Reg.CN_data / Data_Ctrl_Reg.BD_S)*((Data_Ctrl_Reg.BD_S+ N_data_valid -1)/N_data_valid);
        }
        */
        if (0 == Data_Ctrl_Reg.M_RLUT) //Model 0直接输出地址，地址不改变
        {
            if (n3 == Data_Ctrl_Reg.N_BC - 1)
            {
                RI_out_H0_WM = Data_Ctrl_Reg.CN_data % N_data_valid;
                n3 = 0;
                RLUT_Send_Finish = 1;
            }
            else
            {
                n3++;
            }
        }
        else if (1 == Data_Ctrl_Reg.M_RLUT) //Model 1地址自动按CN_step增加
        {

#if debug_en
            if (dump_inner_en)
            {
                fprintf(pcluster->printf_fd, "Addr_RLUT:0x%x, Addr_CLC_RLUT:%x \r\n", Addr_RLUT, Addr_CLC_RLUT);
            }
#endif
            if (i3 >= CN_data_num_cnt) //(Data_Ctrl_Reg.CN_data + N_data_valid -1)/N_data_valid)
            {
                i3 = 0;
                j3++;
                //[zaf]
                if(Data_Ctrl_Reg.N_BC > 1)
                {
                    j3 = 0;
                }
                 if ((n2 == Data_Ctrl_Reg.N_BC)) //&&(n1 == 0))//((Data_Ctrl_Reg.BD_S +N_data_valid-1)/N_data_valid)*Data_Ctrl_Reg.KH*Data_Ctrl_Reg.KW))
                 {
                                  fprintf(pcluster->printf_fd, "[zafxxx N_BC]1phase_en_count: %d,clk:%d N_BC 2\r\n", phase_en_count, clk);
                            RLUT_Send_Finish = 1;
                            RLUT_Send_Finish_one_flag = 1;
                            n2 = 0;
                }
            }
        
            fprintf(pcluster->printf_fd, "[zaf]11 CN_data phase_en_count: %d,clk:%d  i3 %d j3 %d n1 %d CN_data_num_cnt %d\r\n", phase_en_count, clk, i3, j3, \
                    n1, CN_data_num_cnt);
            if (j3 < Data_Ctrl_Reg.CN_loop)
            {
                if (i3 < CN_data_num_cnt) //(Data_Ctrl_Reg.CN_data + N_data_valid -1)/N_data_valid)(Data_Ctrl_Reg.BD_S == Data_Ctrl_Reg.BD)&&
                {
                    RI_out_H0.Mem_Addr = RI_out_H0.Mem_Addr + 2 * i3 * RI_out_H0_WM_dly + j3 * Data_Ctrl_Reg.CN_loop_step;
                    mem_addr_des = RI_out_H0.Mem_Addr;
                    /*
                    if((Data_Ctrl_Reg.BD_S>=N_data_valid)&&(Data_Ctrl_Reg.BD_S%N_data_valid != 0))
                    {
                        if((i3 == 0)&&(j3 == 0))//&&(Data_Ctrl_Reg.N_BC ==1))
                        {
                            mem_addr_des= RI_out_H0.Mem_Addr;
                        }
                        else
                        {
                            mem_addr_des = mem_addr_des + 2*pk_data_valid_dly2 + j3*Data_Ctrl_Reg.CN_loop_step;// pcluster->clc.RC.Addr_offset;                           
                        }
                    }
                    else
                    {
                        RI_out_H0.Mem_Addr  = RI_out_H0.Mem_Addr + i3*2*pk_data_valid +(j3*Data_Ctrl_Reg.CN_loop_step);
                        mem_addr_des = RI_out_H0.Mem_Addr;
                    }
                    */
                    fprintf(pcluster->printf_fd, "[zaf] CN_data phase_en_count: %d,clk:%d  i3 %d j3 %d n1 %d\r\n", phase_en_count, clk, i3, j3, n1);
                    if ((Data_Ctrl_Reg.CN_data - (i3 + 1) * N_data_valid) >= 0)
                    {
                        RI_out_H0_WM = N_data_valid;
                    }
                    else
                    {
                        RI_out_H0_WM = Data_Ctrl_Reg.CN_data % N_data_valid;
                    }
#if debug_en
                    if (dump_inner_en)
                    {
                        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc des:%x Addr_base_Rd:%d i3:%d j3:%d k3:%d BW %d BD %d Mem_Addr %x pk_data_valid %d pk_data_valid_dly %d pk_data_valid_dly2 %d mem_addr_des %x RI_out_H0_WM %d\r\n",
                                phase_en_count, clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i3, j3, k3, Data_Ctrl_Reg.BW, Data_Ctrl_Reg.BD, RI_out_H0.Mem_Addr, pk_data_valid, pk_data_valid_dly, pk_data_valid_dly2, mem_addr_des, RI_out_H0_WM);
                    }
#endif

                    i3++;
                }
                RLUT_Send_Finish_one_flag = 0;
            }
            else
            {
#if debug_en
                if (dump_inner_en)
                {
                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RLUT_Send_Finish_Finish  n2 %d n1 %d\r\n", phase_en_count, clk, n2, n1);
                }
#endif
                RLUT_Send_Finish_one_flag = 1;

                if (Data_Ctrl_Reg.N_BC == 1)
                {
                    mem_addr_des = RI_out_H0.Mem_Addr;
                    i3 = 0;
                    RLUT_Send_Finish = 1;
                    j3 = 0;
                }
                else
                {
                    mem_addr_des = RI_out_H0.Mem_Addr;
                    i3 = 1;
                    if (Data_Ctrl_Reg.N_BC > 1)
                    {
                        if ((n2 == Data_Ctrl_Reg.N_BC)) //&&(n1 == 0))//((Data_Ctrl_Reg.BD_S +N_data_valid-1)/N_data_valid)*Data_Ctrl_Reg.KH*Data_Ctrl_Reg.KW))
                        {
                                  fprintf(pcluster->printf_fd, "[zafxxx N_BC]1phase_en_count: %d,clk:%d N_BC 2\r\n", phase_en_count, clk);
                            RLUT_Send_Finish = 1;
                            RLUT_Send_Finish_one_flag = 1;
                            n2 = 0;
                        }
                    }
                    else
                    {
                        fprintf(pcluster->printf_fd, "[zaf N_BC]1phase_en_count: %d,clk:%d N_BC 2\r\n", phase_en_count, clk);
                        if ((n2 == Data_Ctrl_Reg.N_BC)) //&&(n1 == 0))//((Data_Ctrl_Reg.BD_S +N_data_valid-1)/N_data_valid)*Data_Ctrl_Reg.KH*Data_Ctrl_Reg.KW))
                        {
                            RLUT_Send_Finish = 1;
                            fprintf(pcluster->printf_fd, "[zaf N_BC]phase_en_count: %d,clk:%d N_BC 2 n2 %d\r\n", phase_en_count, clk, n2);
                            n2 = 0;
                        }
                    }
                    j3 = 0;
                }
            }
        }
        else if (2 == Data_Ctrl_Reg.M_RLUT) //Model 2地址自动按CN_step且定期按CN_loop_step增加基地址
        {
            if (i3 >= CN_data_num_cnt) //(Data_Ctrl_Reg.CN_data + N_data_valid -1)/N_data_valid)
            {
                i3 = 0;
                j3++;
            }

            if (j3 >= Data_Ctrl_Reg.CN_loop)
            {
                j3 = 0;
                k3++;
            }

            if (k3 < Data_Ctrl_Reg.CN_loop1)
            {
                if (j3 < Data_Ctrl_Reg.CN_loop)
                {
                    if (i3 < CN_data_num_cnt) //(Data_Ctrl_Reg.CN_data + N_data_valid -1)/N_data_valid)(Data_Ctrl_Reg.BD_S == Data_Ctrl_Reg.BD)&&
                    {
                        if ((Data_Ctrl_Reg.CN_data - (i3 + 1) * N_data_valid) >= 0)
                        {
                            RI_out_H0_WM = N_data_valid;
                        }
                        else
                        {
                            RI_out_H0_WM = Data_Ctrl_Reg.CN_data % N_data_valid;
                        }
#if debug_en
                        if (dump_inner_en)
                        {
                            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc des000:%x Addr_base_Rd:%d i3:%d j3:%d k3:%d BW %d BD %d Mem_Addr %x pk_data_valid %d mem_addr_des %x n3 %d nRI_out_H0_WM %d\r\n",
                                    phase_en_count, clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i3, j3, k3, Data_Ctrl_Reg.BW, Data_Ctrl_Reg.BD, RI_out_H0.Mem_Addr, pk_data_valid_dly2, mem_addr_des, n3, RI_out_H0_WM);
                        }
#endif
                        // cout<<" phase_en_count "<<phase_en_count<<" coreid "<<pcluster->AddrXY<<"RI_out_H0_WM "<<RI_out_H0_WM<<" CN_data "<< Data_Ctrl_Reg.CN_data<<" i3 "<<i3<<endl;
                        RI_out_H0.Mem_Addr = RI_out_H0.Mem_Addr + 2 * i3 * RI_out_H0_WM_dly + j3 * Data_Ctrl_Reg.CN_loop_step + k3 * Data_Ctrl_Reg.CN_loop_step1;
                        mem_addr_des = RI_out_H0.Mem_Addr;
                        /*
                        if((Data_Ctrl_Reg.BD_S>=N_data_valid)&&(Data_Ctrl_Reg.BD_S%N_data_valid != 0))
                        {
                           
                            if((i3 == 0)&&(j3 == 0)&&(k3 == 0))//&&(Data_Ctrl_Reg.N_BC ==1))
                            {
                                mem_addr_des= RI_out_H0.Mem_Addr;
#if debug_en
                                if(dump_inner_en)
                                {
                                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc des000:%x Addr_base_Rd:%d i3:%d j3:%d k3:%d BW %d BD %d Mem_Addr %x pk_data_valid %d mem_addr_des %x n3 %d\r\n",
                                        phase_en_count,clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i3, j3, k3,Data_Ctrl_Reg.BW,Data_Ctrl_Reg.BD,RI_out_H0.Mem_Addr,pk_data_valid_dly2,mem_addr_des,n3);
                                }
#endif 
                            }
                            else
                            {
                                mem_addr_des = mem_addr_des + 2*pk_data_valid_dly2 + j3*Data_Ctrl_Reg.CN_loop_step + k3*Data_Ctrl_Reg.CN_loop_step1;// pcluster->clc.RC.Addr_offset;                           
#if debug_en
                                if(dump_inner_en)
                                {
                                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc des111:%x Addr_base_Rd:%d i3:%d j3:%d k3:%d BW %d BD %d Mem_Addr %x pk_data_valid %d mem_addr_des %x n3 %d\r\n",
                                        phase_en_count,clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i3, j3, k3,Data_Ctrl_Reg.BW,Data_Ctrl_Reg.BD,RI_out_H0.Mem_Addr,pk_data_valid_dly2,mem_addr_des,n3);
                                }
#endif                                
                            }
                        }
                        else
                        {
                            RI_out_H0.Mem_Addr = RI_out_H0.Mem_Addr + 2*i3*pk_data_valid + j3*Data_Ctrl_Reg.CN_loop_step + k3*Data_Ctrl_Reg.CN_loop_step1;
							mem_addr_des = RI_out_H0.Mem_Addr;
#if debug_en
                            if(dump_inner_en)
                            {
                                fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC RAM Addr is from clc des222:%x Addr_base_Rd:%d i3:%d j3:%d k3:%d BW %d BD %d Mem_Addr %x pk_data_valid %d mem_addr_des %x n3 %d\r\n",
                                    phase_en_count,clk, RI_in_A0, Data_Ctrl_Reg.Addr_base_Rd, i3, j3, k3,Data_Ctrl_Reg.BW,Data_Ctrl_Reg.BD,RI_out_H0.Mem_Addr,pk_data_valid_dly2,mem_addr_des,n3);
                            }
#endif 
                        
                    }
                       */
                        i3++;
                    }
                }
                RLUT_Send_Finish_one_flag = 0;
            }
            else
            {
#if debug_en
                if (dump_inner_en)
                {
                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RLUT_Send_Finish_Finish  n2 %d n1 %d\r\n", phase_en_count, clk, n2, n1);
                }
#endif
                RLUT_Send_Finish_one_flag = 1;

                if (Data_Ctrl_Reg.N_BC == 1)
                {
                    mem_addr_des = RI_out_H0.Mem_Addr;
                    i3 = 0;
                    RLUT_Send_Finish = 1;
                    j3 = 0;
                }
                else
                {

                    mem_addr_des = RI_out_H0.Mem_Addr;
                    i3 = 1;

                    if ((Data_Ctrl_Reg.N_BC > 1) && ((Data_Ctrl_Reg.BD_S <= N_data_valid)))
                    {
                        if ((n2 == Data_Ctrl_Reg.N_BC + 1)) //&&(n1 == 0))//((Data_Ctrl_Reg.BD_S +N_data_valid-1)/N_data_valid)*Data_Ctrl_Reg.KH*Data_Ctrl_Reg.KW))
                        {
                            RLUT_Send_Finish = 1;
                            n2 = 0;
                        }
                    }
                    else
                    {
                        if ((n2 == Data_Ctrl_Reg.N_BC)) //&&(n1 == 0))//((Data_Ctrl_Reg.BD_S +N_data_valid-1)/N_data_valid)*Data_Ctrl_Reg.KH*Data_Ctrl_Reg.KW))
                        {
                            RLUT_Send_Finish = 1;
                            n2 = 0;
                        }
                    }
                }
                k3 = 0;
            }
        }
    }
    else
    {
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "x:%d %s %d\r\n", pcluster->AddrXY, __FUNCTION__, __LINE__);
        }
#endif
    }

    RC_busy = 0;
    if ((!Rt_Wen0) && (1 == Ph_en_RC_Flag))
    {
        int group_cnt = 0;
        if (Data_Ctrl_Reg.N_BC == 1)
        {
            group_cnt = ((Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid) * Data_Ctrl_Reg.KH * Data_Ctrl_Reg.KW - 1;
        }
        else
        {
            group_cnt = ((Data_Ctrl_Reg.BD_S + N_data_valid - 1) / N_data_valid) * Data_Ctrl_Reg.KH * Data_Ctrl_Reg.KW;
        }
        
        fprintf(pcluster->printf_fd, "[zaf]phase_en_count: %d,clk:%d  RLUT_Send_Finish_one_flag  n2 %d n1 %d\r\n", phase_en_count, clk, n2, n1);

        if ((n2 == Data_Ctrl_Reg.N_BC - 1) && RLUT_Send_Finish_one_flag) 
        {
            Addr_RLUT = Addr_CLC_RLUT + 8 * n2;
            //[zaf]06171503
            n2++;
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d  num1 n2==========  n2 %d n1 %d\r\n", phase_en_count, clk, n2, n1);
#if debug_en
            if (dump_inner_en)
            {
                fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RLUT_Send_Finish111  n1 %d n2 %d\r\n", phase_en_count, clk, n1, n2);
            }
#endif
        }
        else
        {
#if debug_en
            if (dump_inner_en)
            {
                fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RLUT_Send_Finish222  n2 %d n1 %d\r\n", phase_en_count, clk, n2, n1);
            }
#endif
            if (Data_Ctrl_Reg.N_BC > 1)
            {
                if (n1 == 0)
                {
                    if (!b_interupt || Rt_Wen0)
                    {
                        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d num2 n2==========  n2 %d n1 %d Rt_Wen0 %d b_interrupt %d \r\n", \
                            phase_en_count, clk, n2, n1, Rt_Wen0, b_interupt);
                        Addr_RLUT = Addr_CLC_RLUT + 8 * n2;
                        n2++;
                    }
                    else
                    {
                        Addr_RLUT = Addr_CLC_RLUT + 8 * (n2 - 1);
                    }
                }
                else
                {
                    Addr_RLUT = Addr_CLC_RLUT + 8 * n2;
                }
            }
        }
    }

    if (!RI_out_H0.C_dest) //多播将去lut中读取包头
    {
        RC.RLUT_Addr = RI_out_H0.C_dest;
    }
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC.RLUT_Addr %x,Ren_Mem %d Ren_CLC %d CLC_Ren %d RI_in_A0 %x C_Mode_Merge %d MC_Mode %d Ph_en_RC_Flag_delay %d CLC_Wen_rt %d RC.Rt_Ready %d N_BC %d Rt_Wen0 %d n2 %d n1 %d\r\n",
                phase_en_count, clk, RC.RLUT_Addr, RC.Ren_Mem, RC.Ren_CLC, CLC_Ren, RI_in_A0, Data_Ctrl_Reg.C_Mode_Merge, Data_Ctrl_Reg.MC_Mode, Ph_en_RC_Flag_delay, CLC_Wen_rt, RC.Rt_Ready, Data_Ctrl_Reg.N_BC, Rt_Wen0, n2, n1);
    }

#endif
    //

    RI_out_H0_WM_tmp = (RI_out_H0_WM == 0) ? 8 : RI_out_H0_WM;
    if ((Router_data_fifoQ.size() < RI_out_H0_WM_tmp) && (!stop_rd_head_addr_flag_close))
    {
        stop_rd_head_addr_flag = 1;
        //cout<<" phase_en_count "<<phase_en_count<<" coreid "<<pcluster->AddrXY<<"RI_out_H0_WM "<<RI_out_H0_WM<<" Router_data_fifoQ.size() "<< Router_data_fifoQ.size()<<" i3 "<<i3<<endl;
    }
    else
    {
        stop_rd_head_addr_flag = 0;
    }
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RI_out_H0_WM %d,Router_data_fifoQ.size %d stop_rd_head_addr_flag %d core_addr_OUT:0x%x  mem_addr_OUT:0x%x mem_addr_des:0x%x\r\n", phase_en_count, clk, RI_out_H0_WM_tmp, Router_data_fifoQ.size(), stop_rd_head_addr_flag, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RI_out_H0.Mem_Addr);
    }

#endif

    //
    if (0 == RC.Rt_Ready) //如果Router忙
    {
        CLC_Wen_rt = 0;
    }
    else //如果Router不忙，则将数据发送给NoC
    {
        if ((Ph_en_RC_Flag_delay && (RC.Ren_Mem || RC.Ren_CLC)) && (!Rt_Wen0) && (!stop_rd_head_addr_flag))
        {
            if ((Rt_c_dest_delay) == 0) //不是多播则用要读取的数据和RLUT 中的包头
            {
                // cout<<" phase_en_count "<<phase_en_count<<" coreid "<<pcluster->AddrXY<<" RI_out_H0.Core_Addr "<<RI_out_H0.Core_Addr<<" Router_data_fifoQ is not empty pre!"<< "RI_out_H0_WM_tmp "<<RI_out_H0_WM_tmp <<" size "<<Router_data_fifoQ.size()<<endl;
                for (int i = 0; i < RI_out_H0_WM_tmp; i++)
                {
                    if (!Router_data_fifoQ.empty())
                    {
                        fifo_out_data[i] = Router_data_fifoQ.front().data;
                        Router_data_fifoQ.pop();
                        fprintf(pcluster->printf_fd, "[zaf]phase_en_count:%d,clk:%d size = %d, RI_out_H0_WM_tmp =%d\r\n", phase_en_count, clk, Router_data_fifoQ.size(), RI_out_H0_WM_tmp);
                        //cout<<" [zaf]phase_en_count "<<phase_en_count<<" coreid "<<pcluster->AddrXY<<" Router_data_fifoQ is not empty !"<< "size "<<Router_data_fifoQ.size()<<endl;
                    }
                    else
                    {
                        //    cout<<" phase_en_count "<<phase_en_count<<" coreid "<<pcluster->AddrXY<< " erro:Router_data_fifoQ is empty!"<< "RI_out_H0_WM_tmp "<<Router_data_fifoQ.size()<<endl;
                        // exit(1);
                    }
                }
                int size = Router_data_fifoQ.size();
#if debug_en
                if (dump_inner_en)
                {
                    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC.RLUT_Addr %x,Ren_Mem %d Ren_CLC %d CLC_Ren %d RI_in_A0 %x C_Mode_Merge %d MC_Mode %d Ph_en_RC_Flag_delay %d CLC_Wen_rt %d RC.Rt_Ready %d N_BC %d Rt_Wen0 %d n2 %d n1 %d left_size %d \r\n",
                            phase_en_count, clk, RC.RLUT_Addr, RC.Ren_Mem, RC.Ren_CLC, CLC_Ren, RI_in_A0, Data_Ctrl_Reg.C_Mode_Merge, Data_Ctrl_Reg.MC_Mode, Ph_en_RC_Flag_delay, CLC_Wen_rt, RC.Rt_Ready, Data_Ctrl_Reg.N_BC, Rt_Wen0, n2, n1, size);
                }

#endif
                RC.RI.RN_OUT1 = fifo_out_data[0];
                RC.RI.RN_OUT2 = fifo_out_data[1];
                RC.RI.RN_OUT3 = fifo_out_data[2];
                RC.RI.RN_OUT4 = fifo_out_data[3];
                RC.RI.RN_OUT5 = fifo_out_data[4];
                RC.RI.RN_OUT6 = fifo_out_data[5];
                RC.RI.RN_OUT7 = fifo_out_data[6];
                RC.RI.RN_OUT8 = fifo_out_data[7];
                /*
                RC.RI.RN_OUT1 = RI_out_D0.data1;
                RC.RI.RN_OUT2 = RI_out_D0.data2;
                RC.RI.RN_OUT3 = RI_out_D0.data3;
                RC.RI.RN_OUT4 = RI_out_D0.data4;
                RC.RI.RN_OUT5 = RI_out_D0.data5;
                RC.RI.RN_OUT6 = RI_out_D0.data6;
                RC.RI.RN_OUT7 = RI_out_D0.data7;
                RC.RI.RN_OUT8 = RI_out_D0.data8;
                */
                RC.RI.core_addr_OUT = core_addr_reg; //RI_out_H0.Core_Addr;
#if debug_en
                if (dump_inner_en)
                {
                  //  fprintf(pcluster->printf_fd, "[zaf]phase_en_count: %d,clk:%d , CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d \n",
                    //        phase_en_count, clk, RC.CIF_Wen, RC.C_Ready, RI_out_H0.Core_Addr, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);
                }
#endif
                RC.RI.mem_addr_OUT = mem_addr_des; //RI_out_H0.Mem_Addr;
                                                   //fifo
                RC.RI.WM_OUT = RI_out_H0_WM;       //pk_data_valid_dly;//RI_out_H0_WM;
                RC.RI.c_dest_OUT = RI_out_H0.C_dest;
                CLC_Wen_rt = 1;
            }
        }
        else
        {
            CLC_Wen_rt = 0;
        }
    }

#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "[zaf][out end]phase_en_count: %d,clk:%d , CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d \n",
                phase_en_count, clk, RC.CIF_Wen, RC.C_Ready, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);
    }
#endif

    //使能CIF_Wen信号，这个信号为RC与NOC节点之间的发送使能信号
    if ((RC.Ren_Mem || RC.Ren_CLC) && (!Rt_Wen0) && Ph_en_RC_Flag_delay) //&&(!stop_rd_head_addr_flag)&&Ph_en_RC_Flag_delay)
    {
        if ((Data_Ctrl_Reg.MC_Mode & 0x1) == 1)
        {
            // RC.CIF_Wen = MC_Wen;
        }
        else
        {
            if (RLUT_Send_Finish) //(Data_Send_Finish || RLUT_Send_Finish)
            {
                RC.CIF_Wen = 0;
            }
            else
            {
                if (0 == Data_Ctrl_Reg.C_Mode_Merge) //正常模式
                {
                    RC.CIF_Wen = CLC_Wen_rt;
                }
                else if (((1 == (Xo_D & (0x1))) && (1 == Data_Ctrl_Reg.C_Merge)) || ((0 == (Xo_D & (0x1))) && (0 == Data_Ctrl_Reg.C_Merge))) //snn模式
                {
                    RC.CIF_Wen = CLC_Wen_rt;
                }
                else
                {
                    RC.CIF_Wen = 0;
                }
            }
        }
    }
#ifdef no_noc
    //cout<<"RC.RLUT_Data"<<RC.RLUT_Data<<"\n"<<endl;
    //数据包需要发往其他chip router

    //	if(((((RC.RI.core_addr_OUT) >> 10) & 0x1) == 1)&&(CLC_Wen_rt == 1)&&(!stop_rd_head_addr_flag))

    if (((((core_addr_reg) >> 10) & 0x1) == 1) && (CLC_Wen_rt == 1)) //&&(!stop_rd_head_addr_flag))
    {
        //不发送到noc
        cout << "core_id_rcRC.RI.core_addr_OUT " << RC.RI.core_addr_OUT << endl;
        RC.CIF_Wen = 0;
        cout << "\n****************OUT Chip!!!***********************\n"<< endl;
        //打包数据到本cluster对应的本地chip router
        local_cr = pcluster->AddrXY / CORE_NUM_CB;
        pack_cr_queue(clk, pcluster, pchip, local_cr, phase_en_count);
#if debug_en
        if (dump_inner_en)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d OUT Chip!!!send data to noc, CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d ",
                    phase_en_count, clk, RC.CIF_Wen, RC.C_Ready, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);

            if (RC.RI.WM_OUT == 1)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x\r\n", RC.RI.RN_OUT1);
            }
            else if (2 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2);
            }
            else if (3 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3);
            }
            else if (4 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4);
            }
            else if (5 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5);
            }
            else if (6 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6);
            }
            else if (7 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6, RC.RI.RN_OUT7);
            }
            else if (0 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6, RC.RI.RN_OUT7, RC.RI.RN_OUT8);
            }
        }
#endif
    }
#endif

    //message
    if ((RI_in_A0 >= FIFO_START) && (RI_in_A0 <= FIFO_END)) //判断信号是否写FIFO地址(0x5D880~0x5D88F)，是的话把router接收数据的低32位写入msg FIFO中，由clc取走
    {
        pack_msg_queue(pcluster, RI_in_D0);
    }

    //读取fifo数据
    if (pcluster->clc.msgQ.empty() == 0)
    {
        RC.Msg_rev_RC = 1;
        Dout_FIFO = pcluster->clc.msgQ.front().data1 | ((pcluster->clc.msgQ.front().data2) << 16);
        pcluster->clc.msgQ.pop();
    }
    else
    {
        Dout_FIFO = 0;
        RC.Msg_rev_RC = 0;
    }
    RC.Din_CLC_RC = Dout_FIFO;

    //区分读写CLC/Mem

    Ren_Mem = CLC_Ren & !((RI_in_A0 & CLC_REG_MASK) == CLC_REG);
    Wen_Mem = Rt_Wen0 & !((RI_in_A0 & CLC_REG_MASK) == CLC_REG) & (RC.Msg_rev_RC == 0);
    Ren_CLC = CLC_Ren & ((RI_in_A0 & CLC_REG_MASK) == CLC_REG);
    Wen_CLC = Rt_Wen0 & ((RI_in_A0 & CLC_REG_MASK) == CLC_REG);

    RRen_Mem = Ren_Mem; //&& !RC.Busy_Mem;
    RWen_Mem = Wen_Mem; //&& !RC.Busy_Mem;
                        // RWen_MC  = Wen_MC  && !RC.Busy_MC;
    RRen_CLC = Ren_CLC && !RC.Busy_CLC;
    RWen_CLC = Wen_CLC && !RC.Busy_CLC;

    RLUT_busy_CIF = RC_busy;

    RC.Ren_Mem = RRen_Mem; //对Mem的有效读请求信号
    RC.Wen_Mem = RWen_Mem; //对Mem的有效写请求信号
    RC.Ren_CLC = RRen_CLC; //对CLC的有效读请求信号
    RC.Wen_CLC = RWen_CLC; //对CLC的有效写请求信号
    //RC.WM = 8;

    //RLUT地址给RC对外接口
#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "Rt_c_destCCCC %d,Rt_c_dest_delay %d\r\n", Rt_c_dest, Rt_c_dest_delay);
    }
#endif

    if ((Rt_c_dest) == 0) //不是多播则用要读取的数据和RLUT 中的包头
    {
        RC.RLUT_Addr = Addr_RLUT;
    }
    else
    {
        RC.RLUT_Addr = mem4_base + (208 + Rt_c_dest) * 8;
    }
    if ((Rt_c_dest_delay) != 0)
    {
        RC.RI.RN_OUT1 = RI_in_D0_delay.data1;
        RC.RI.RN_OUT2 = RI_in_D0_delay.data2;
        RC.RI.RN_OUT3 = RI_in_D0_delay.data3;
        RC.RI.RN_OUT4 = RI_in_D0_delay.data4;
        RC.RI.RN_OUT5 = RI_in_D0_delay.data5;
        RC.RI.RN_OUT6 = RI_in_D0_delay.data6;
        RC.RI.RN_OUT7 = RI_in_D0_delay.data7;
        RC.RI.RN_OUT8 = RI_in_D0_delay.data8;

        RC.RI.core_addr_OUT = (RC.RLUT_Data >> 23) & 0x7FF;
        RC.RI.mem_addr_OUT = RtinmemA_delay; //(RC.RLUT_Data >> 4) & 0x7FFFF;
        RC.RI.WM_OUT = RI_WM0;
        RC.RI.c_dest_OUT = (RC.RLUT_Data >> 1) & 0x7;
        RC.CIF_Wen = 1;
    }

    RI_in_D0_delay = RI_in_D0;
    RtinmemA_delay = pRtinmemA;
    Rt_c_dest_delay = Rt_c_dest;
    Ph_en_RC_Flag_delay = Ph_en_RC_Flag;
    CLC_Ren_delay = CLC_Ren;
    if ((Ph_en_RC_Flag == 1) && (!Rt_Wen0) && (i3 == 0) && (j3 == 0) && (k3 == 0))
    {
        stop_rd_head_addr_flag = stop_rd_head_addr_flag_pre;
    }

    if ((!Rt_Wen0) && (!b_interupt))
    {
        pk_data_valid_dly2 = pk_data_valid_dly;
        pk_data_valid_dly = pk_data_valid;
        mem_addr_des_dly = mem_addr_des;
        RI_out_H0_WM_dly = RI_out_H0_WM;
        Data_Send_Finish_dly2 = Data_Send_Finish_dly;
        Data_Send_Finish_dly = Data_Send_Finish;
        stop_rd_head_addr_flag_dly3 = stop_rd_head_addr_flag_dly2;
        stop_rd_head_addr_flag_dly2 = stop_rd_head_addr_flag_dly;
        stop_rd_head_addr_flag_dly = stop_rd_head_addr_flag;
    }
    if (b_interupt && !Rt_Wen0)
    {
        b_interupt = 0;
        if (stop_rd_head_addr_flag_dly == 1)
        {
            stop_rd_head_addr_flag = 1;
        }
        else
        {
            stop_rd_head_addr_flag = 0;
        }
    }

#if debug_en
    if (dump_inner_en)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d RC.RLUT_Addr %x,Ren_Mem %d Ren_CLC %d CLC_Ren %d RI_in_A0 %x C_Mode_Merge %d MC_Mode %d Ph_en_RC_Flag %d CLC_Wen_rt %d RC.Rt_Ready %d N_BC %d Rt_Wen0 %d\r\n",
                phase_en_count, clk, RC.RLUT_Addr, RC.Ren_Mem, RC.Ren_CLC, CLC_Ren, RI_in_A0, Data_Ctrl_Reg.C_Mode_Merge, Data_Ctrl_Reg.MC_Mode, Ph_en_RC_Flag_delay, CLC_Wen_rt, RC.Rt_Ready, Data_Ctrl_Reg.N_BC, Rt_Wen0);
    }

#endif

    //清除中断状态寄存器
    if (pcluster->clc.REG_RC.Int_clr.regval.Int_send_clr != 0)
    {
        pcluster->clc.REG_RC.Int_st.regval.Int_send = 0;
        pcluster->clc.REG_RC.Int_clr.regval.Int_send_clr = 0;
    }
    if (pcluster->clc.REG_RC.Int_clr.regval.Int_msg_clr != 0)
    {
        pcluster->clc.REG_RC.Int_st.regval.Int_msg = 0;
        pcluster->clc.REG_RC.Int_clr.regval.Int_msg_clr = 0;
    }
    if (pcluster->clc.REG_RC.Int_clr.regval.Int_RX_clr != 0)
    {
        pcluster->clc.REG_RC.Int_st.regval.Int_RX = 0;
        pcluster->clc.REG_RC.Int_clr.regval.Int_RX_clr = 0;
    }
    if (pcluster->clc.REG_RC.Int_clr.regval.Int_all_clr != 0)
    {
        memset(&pcluster->clc.REG_RC.Int_st.regval, 0, sizeof(pcluster->clc.REG_RC.Int_st.regval));
        memset(&pcluster->clc.REG_RC.Int_clr.regval, 0, sizeof(pcluster->clc.REG_RC.Int_clr.regval));
    }
    //发送结束，清除CLC发送使能Ph_en_RC_Flag
    if (RLUT_Send_Finish) //||Router_data_fifoQ.empty())//(Data_Send_Finish || RLUT_Send_Finish)
    {
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d rlut_______________send_finish\r\n", phase_en_count, clk);
        Data_Send_Finish = 0;
        Data_Send_Finish_dly = 0;
        Ph_en_RC_Flag = 0;
        Ph_en_RC_Flag_delay = 0;
        pcluster->clc.REG_RC.Int_st.regval.Int_send = 1; //发送完成中断信号
#if debug_en
        if (dump_inner_en)
        {
            if (Data_Send_Finish != RLUT_Send_Finish)
            {
                int size = Router_data_fifoQ.size();
                fprintf(pcluster->printf_fd, "Warning, Please check your Configure, Data send %s, but RLUT send %s\r\n",
                        Data_Send_Finish ? "Finished" : "UnFinished",
                        RLUT_Send_Finish ? "Finished" : "UnFinished");
                fprintf(pcluster->printf_fd, "Data_Ctrl_Reg M_RLUT:0x%x CN_LOOP:0x%x  CN_data:0x%x CN_loop:0x%x CN_loop_step:0x%x size %d\r\n",
                        Data_Ctrl_Reg.M_RLUT, Data_Ctrl_Reg.CN_loop, Data_Ctrl_Reg.CN_data, Data_Ctrl_Reg.CN_loop, Data_Ctrl_Reg.CN_loop_step, size);
            }
            else
            {
                fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d rc send finished\r\n", phase_en_count, clk);
            }
        }
#endif
        int size = Router_data_fifoQ.size();
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d size0 ============================ %d\r\n", phase_en_count, clk, size);
        while (!Router_data_fifoQ.empty())
            Router_data_fifoQ.pop();
        size = Router_data_fifoQ.size();
        fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d size1 ============================ %d\r\n", phase_en_count, clk, size);
    }
    int size = Router_data_fifoQ.size();
    fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d size3 ============================ %d\r\n", phase_en_count, clk, size);
#if debug_en
    if (dump_inner_en)
    {
        //打印发送数据信息
        if (1 == RC.CIF_Wen)
        {
            fprintf(pcluster->printf_fd, "phase_en_count: %d,clk:%d send data to noc, CIF_Wen:%d C_Ready:%d core_addr_OUT:0x%x  mem_addr_OUT:0x%x  c_dest_OUT:0x%x WM_OUT:%d ",
                    phase_en_count, clk, RC.CIF_Wen, RC.C_Ready, RC.RI.core_addr_OUT, RC.RI.mem_addr_OUT, RC.RI.c_dest_OUT, RC.RI.WM_OUT);

            if (RC.RI.WM_OUT == 1)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x\r\n", RC.RI.RN_OUT1);
            }
            else if (2 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2);
            }
            else if (3 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3);
            }
            else if (4 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4);
            }
            else if (5 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5);
            }
            else if (6 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6);
            }
            else if (7 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6, RC.RI.RN_OUT7);
            }
            else if (0 == RC.RI.WM_OUT)
            {
                fprintf(pcluster->printf_fd, "data:0x%04x%04x%04x%04x%04x%04x%04x%04x\r\n", RC.RI.RN_OUT1, RC.RI.RN_OUT2, RC.RI.RN_OUT3, RC.RI.RN_OUT4,
                        RC.RI.RN_OUT5, RC.RI.RN_OUT6, RC.RI.RN_OUT7, RC.RI.RN_OUT8);
            }
        }
    }
#endif
}

void CRouter::RouterDump(void)
{
    printf("\r\n\r\nStart Dump Mem0\r\n");
    for (int i = 0; i < sizeof(RLUT); i++)
    {
        if (i % 16 == 0)
        {
            printf("\r\n%08x:", i);
        }
        printf("%02X ", RLUT[i]);
    }
}
