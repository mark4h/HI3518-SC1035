#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"

#include "hi_comm_isp.h"
#include "mpi_isp.h"
#include "hi_sns_ctrl.h"

// #include "mpi_ae.h"
// #include "mpi_awb.h"
// #include "mpi_af.h"

#include "SC1035_sensor_ctl.h"

#define MAX_FRM_CNT     256
#define MAX_FRM_WIDTH   4096        //如果这个值太小，图像很大的话存不了

pthread_t isp_pid;
static VO_DEV s_VoDev = 0;
static VO_CHN s_VoChn = 0;

/*AR0130 DC 12bit输入*/
VI_DEV_ATTR_S DEV_ATTR_AR0130_DC_720P_BASE =
{
    /*接口模式*/
    VI_MODE_DIGITAL_CAMERA,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_YUYV,

    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
	/*VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,*/

    /*timing信息，对应reg手册的如下配置*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,         0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*使用内部ISP*/
    VI_PATH_ISP,
    /*输入数据类型*/
    VI_DATA_TYPE_RGB
};

VI_DEV_ATTR_S stBayerAr0130Attr =
{
    /*接口模式*/
    VI_MODE_DIGITAL_CAMERA,
    /*1、2、4路工作模式*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /*逐行or隔行输入*/
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, 仅支持YUV格式*/
    VI_INPUT_DATA_YUYV,

    /*同步信息，对应reg手册的如下配置, --bt1120时序无效*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
	/*VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,*/

    /*timing信息，对应reg手册的如下配置*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,         0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*使用内部ISP*/
    VI_PATH_RAW,
    /*输入数据类型*/
    VI_DATA_TYPE_RGB
};

VI_CHN_ATTR_S stBayerChn720PAttr =
{
    /*crop_x crop_y, crop_w  crop_h*/
    {0,     0, 1280,   720},
    /*dest_w  dest_h  */
    {1280,   720 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /*   通道格式*/
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32FrameRate*/
    -1, -1 /*25, 25*/
};

#define DEV_ATTR           DEV_ATTR_AR0130_DC_720P_BASE
#define DEV_BAYER_ATTR     stBayerAr0130Attr
#define CHN_BAYER_ATTR     stBayerChn720PAttr

void sample_bayer_dump(VIDEO_FRAME_S * pVBuf, HI_U32 u32Nbit, FILE *pfd)
{
    unsigned int w, h;
    HI_U16* pVBufVirt_Y;
    HI_U16* pVBufVirt_C;
    HI_U8  au8Data[MAX_FRM_WIDTH];
    HI_U16 au16Data[MAX_FRM_WIDTH];
    HI_U32 phy_addr,size;
    HI_U8* pUserPageAddr[2];

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pVBuf->enPixelFormat)
    {
        printf("Err! Bayer data can't be 420\n");
        return;
    }

    size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;

    phy_addr = pVBuf->u32PhyAddr[0];

    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }

    pVBufVirt_Y = (HI_U16*)pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Raw data......u32Stride[0]: %d, width: %d\n", pVBuf->u32Stride[0], pVBuf->u32Width);
    fflush(stderr);
    for(h=0; h<pVBuf->u32Height; h++)
    {
        HI_U16 u16Data;
        for (w=0; w<pVBuf->u32Width; w++)
        {
            if (8 == u32Nbit)
            {
                au8Data[w] = (pVBufVirt_Y[h*pVBuf->u32Width + w] >> 8);
            }
            else if (10 == u32Nbit)
            {
                u16Data = (pVBufVirt_Y[h*pVBuf->u32Width + w] >> 6);
                au16Data[w] = u16Data;
            }
            else if (12 == u32Nbit)         //12bit 在高位
            {
                u16Data = (pVBufVirt_Y[h*pVBuf->u32Width + w] >> 4);
                au16Data[w] = u16Data;
            }
            else
            {
                printf("Err! Bayer data can't support %d bits!eg: 8bits;10bits;12bits.\n", u32Nbit);
                return;
            }
        }

        if (8 == u32Nbit)
        {
            fwrite(au8Data, pVBuf->u32Width, 1, pfd);
        }
        else
        {
            fwrite(au16Data, pVBuf->u32Width, 2, pfd);
        }

    }
    fflush(pfd);

    fprintf(stderr, "done u32TimeRef: %d!\n", pVBuf->u32TimeRef);
    fflush(stderr);

    HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
}

HI_S32 VI_DumpBayer(VI_CHN ViChn, HI_U32 u32Nbit, HI_U32 u32Cnt)
{
    int i, j;
    VI_FRAME_INFO_S stFrame;
    VI_FRAME_INFO_S astFrame[MAX_FRM_CNT];
    HI_CHAR szYuvName[128];
    FILE *pfd;
	HI_S32 s32Ret = 0;

    if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
    {
        printf("HI_MPI_VI_SetFrameDepth err, vi chn %d \n", ViChn);
        return -1;
    }

    //usleep(5000);

	// s32Ret = HI_MPI_VI_GetFrameTimeOut(ViChn, &stFrame.stViFrmInfo, -1);
	s32Ret = HI_MPI_VI_GetFrame(ViChn, &stFrame.stViFrmInfo);
	
    if (s32Ret)
    {
        printf("HI_MPI_VI_GetFrame err, vi chn %d, ret value %x\n", ViChn, s32Ret);
		printf("HI_ERR_VI_BUF_EMPTY = %x\n", HI_ERR_VI_BUF_EMPTY);
        return -1;
    }

    /* make file name */
    sprintf(szYuvName, "./vi_chn_%d_%d_%d_%d_%dbits.raw", ViChn,
        stFrame.stViFrmInfo.stVFrame.u32Width, stFrame.stViFrmInfo.stVFrame.u32Height, u32Cnt, u32Nbit);
    printf("Dump YUV frame of vi chn %d  to file: \"%s\"\n", ViChn, szYuvName);
    HI_MPI_VI_ReleaseFrame(ViChn, &stFrame.stViFrmInfo);

    /* open file */
    pfd = fopen(szYuvName, "wb");
    if (NULL == pfd)
    {
        return -1;
    }

	printf("Getting all frames\n");
	
    /* get VI frame  */
    for (i=0; i<u32Cnt; i++)
    {
        if (HI_MPI_VI_GetFrame(ViChn, &astFrame[i].stViFrmInfo)<0)
        {
            printf("get vi chn %d frame err\n", ViChn);
            printf("only get %d frame\n", i);
            break;
        }
    }
	
	printf("Saving all frames\n");

    for(j=0; j<i; j++)
    {
        /* save VI frame to file */
        sample_bayer_dump(&astFrame[j].stViFrmInfo.stVFrame, u32Nbit, pfd);

        /* release frame after using */
        HI_MPI_VI_ReleaseFrame(ViChn, &astFrame[j].stViFrmInfo);
    }
	
	printf("All frames processed\n");

    fclose(pfd);    
	
	printf("File closed\n");

    return 0;
}

int VI_InitMpp()
{
    MPP_SYS_CONF_S stSysConf = {0};
    VB_CONF_S stVbConf ={0};

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
	
	// need to set this based on availble memory otherwise HI_MPI_VB_Init failed
	// try cat /proc/media-mem
    stVbConf.astCommPool[0].u32BlkSize = 1280*920*2;
    stVbConf.astCommPool[0].u32BlkCnt = 5; 

    if ( HI_MPI_VB_SetConf(&stVbConf))
    {
        printf("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    if(HI_MPI_VB_Init())
    {
        printf("VI_InitMpp HI_MPI_VB_Init failed!\n");
        return HI_SUCCESS;
    }

    stSysConf.u32AlignWidth = 16;
    if ( HI_MPI_SYS_SetConf(&stSysConf))
    {
        printf("conf : system config failed!\n");
        return HI_SUCCESS;
    }

    if (HI_MPI_SYS_Init())
    {
        printf("sys init failed!\n");
        return HI_SUCCESS;
    }
    return HI_SUCCESS;
}

HI_S32 VI_StartBayerData(VI_DEV_ATTR_S *pstDevAttr, VI_DEV_ATTR_S *pstBayerDevAttr,
    VI_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret = 0;
    VI_DEV ViDev = 0;
    VI_CHN ViChn = 0;
	
	sensor_init();

    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, pstBayerDevAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDevAttr failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_VI_EnableDev(ViDev);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_EnableDev failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_VI_SetChnAttr(ViChn, pstChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetChnAttr failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_EnableChn failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_S32 main(int argc, char *argv[])
{
    HI_S32 s32Ret = 0;
    HI_U32 u32Nbit = 8;
    HI_U32 u32FrmCnt = 1;

    printf("\nNOTICE: This tool only can be used for TESTING !!!\n");
    if(argc < 2)
    {
        printf("usage: ./vi_bayerdump [nbit] [frmcnt]. sample: ./vi_dump 12 1\n");
        printf("[nbit]----------Raw data:8bit;10bit;12bit\r\n");
        printf("[frmcnt]----------the frame number \r\n\n\n");

        return s32Ret;
    }

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    s32Ret = VI_InitMpp();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Init Mpp failed!\n");
        return s32Ret;
    }

    if (argc > 1)
    {
        u32Nbit = atoi(argv[1]);    /* nbit of Raw data:8bit;10bit;12bit */
    }
    if (argc > 2)
    {
        u32FrmCnt = atoi(argv[2]);/* the frame number */
    }


    s32Ret = VI_StartBayerData(&DEV_ATTR, &DEV_BAYER_ATTR, &CHN_BAYER_ATTR);
    if (HI_SUCCESS != s32Ret)
    {
        printf("VI_StartBayerData failed!\n");
        return s32Ret;
    }

    s32Ret = VI_DumpBayer(0, u32Nbit, u32FrmCnt);
    if (HI_SUCCESS != s32Ret)
    {
        printf("VI_StartBayerData failed!\n");
        return s32Ret;
    }
	
	printf("Shutting down\n");
	
    //sleep(2);
    //VI_StopIsp();
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    return HI_SUCCESS;
}



