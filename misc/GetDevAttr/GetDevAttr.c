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

// int VI_InitMpp()
// {
    // MPP_SYS_CONF_S stSysConf = {0};
    // VB_CONF_S stVbConf ={0};

    // HI_MPI_SYS_Exit();
    // HI_MPI_VB_Exit();
	
	// // need to set this based on availble memory otherwise HI_MPI_VB_Init failed
	// // try cat /proc/media-mem
    // stVbConf.astCommPool[0].u32BlkSize = 1280*920*2;
    // stVbConf.astCommPool[0].u32BlkCnt = 5; 

    // if ( HI_MPI_VB_SetConf(&stVbConf))
    // {
        // printf("HI_MPI_VB_SetConf failed!\n");
        // return HI_FAILURE;
    // }

    // if(HI_MPI_VB_Init())
    // {
        // printf("VI_InitMpp HI_MPI_VB_Init failed!\n");
        // return HI_SUCCESS;
    // }

    // stSysConf.u32AlignWidth = 16;
    // if ( HI_MPI_SYS_SetConf(&stSysConf))
    // {
        // printf("conf : system config failed!\n");
        // return HI_SUCCESS;
    // }

    // if (HI_MPI_SYS_Init())
    // {
        // printf("sys init failed!\n");
        // return HI_SUCCESS;
    // }
    // return HI_SUCCESS;
// }

HI_S32 main(int argc, char *argv[])
{	
    HI_S32 s32Ret = 0;

    // HI_MPI_SYS_Exit();
    // HI_MPI_VB_Exit();

    // s32Ret = VI_InitMpp();
    // if (HI_SUCCESS != s32Ret)
    // {
        // printf("Init Mpp failed!\n");
        // return s32Ret;
    // }

    //////////////////////////////////////////////////////////////////////////////
	
	VI_DEV_ATTR_S pstDevAttr = {0};
	
    VI_DEV ViDev = 0;
	VI_CHN ViChn = 0;
	
	s32Ret = HI_MPI_VI_GetDevAttr(ViDev, &pstDevAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDevAttr failed!\n");
        return s32Ret;
    }
	
	printf("enIntfMode = %d\n", pstDevAttr.enIntfMode);
	printf("enWorkMode = %d\n", pstDevAttr.enWorkMode);
	printf("au32CompMask[0] = %x\n", pstDevAttr.au32CompMask[0]);
	printf("au32CompMask[1] = %x\n", pstDevAttr.au32CompMask[1]);
	printf("enScanMode = %d\n", pstDevAttr.enScanMode);
	printf("s32AdChnId[0] = %d\n", pstDevAttr.s32AdChnId[0]);
	printf("s32AdChnId[1] = %d\n", pstDevAttr.s32AdChnId[1]);
	printf("s32AdChnId[2] = %d\n", pstDevAttr.s32AdChnId[2]);
	printf("s32AdChnId[3] = %d\n", pstDevAttr.s32AdChnId[3]);
	printf("enDataSeq = %d\n", pstDevAttr.enDataSeq);
	printf("enDataPath = %d\n", pstDevAttr.enDataPath);
	printf("enInputDataType = %d\n", pstDevAttr.enInputDataType);
	printf("bDataRev = %d\n", pstDevAttr.bDataRev);
	
	printf("\n");
	printf("enVsync = %d\n", pstDevAttr.stSynCfg.enVsync);
	printf("enVsyncNeg = %d\n", pstDevAttr.stSynCfg.enVsyncNeg);
	printf("enHsync = %d\n", pstDevAttr.stSynCfg.enHsync);
	printf("enHsyncNeg = %d\n", pstDevAttr.stSynCfg.enHsyncNeg);
	printf("enVsyncValid = %d\n", pstDevAttr.stSynCfg.enVsyncValid);
	printf("enVsyncValidNeg = %d\n", pstDevAttr.stSynCfg.enVsyncValidNeg);
	
	printf("\n");
	printf("stTimingBlank.u32HsyncHfb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32HsyncHfb);
	printf("stTimingBlank.u32HsyncAct = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32HsyncAct);
	printf("stTimingBlank.u32HsyncHbb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32HsyncHbb);
	printf("stTimingBlank.u32VsyncVfb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVfb);
	printf("stTimingBlank.u32VsyncVact = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVact);
	printf("stTimingBlank.u32VsyncVbb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVbb);
	printf("stTimingBlank.u32VsyncVbfb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVbfb);
	printf("stTimingBlank.u32VsyncVbact = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVbact);
	printf("stTimingBlank.u32VsyncVbbb = %u\n", pstDevAttr.stSynCfg.stTimingBlank.u32VsyncVbbb);
	
	printf("\n--------------------------------------------------------------------------\n\n");
	
	VI_CHN_ATTR_S pstChnAttr = {0};
	
	s32Ret = HI_MPI_VI_GetChnAttr(ViChn, &pstChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetChnAttr failed!\n");
        return s32Ret;
    }
	
	printf("stCapRect.s32X = %d\n", pstChnAttr.stCapRect.s32X);
	printf("stCapRect.s32Y = %d\n", pstChnAttr.stCapRect.s32Y);
	printf("stCapRect.u32Width = %u\n", pstChnAttr.stCapRect.u32Width);
	printf("stCapRect.u32Height = %u\n", pstChnAttr.stCapRect.u32Height);
	printf("stDestSize.u32Width = %u\n", pstChnAttr.stDestSize.u32Width);
	printf("stDestSize.u32Height = %u\n", pstChnAttr.stDestSize.u32Height);
	printf("enCapSel = %d\n", pstChnAttr.enCapSel);
	printf("enPixFormat = %d\n", pstChnAttr.enPixFormat);
	printf("bMirror = %d\n", pstChnAttr.bMirror);
	printf("bFlip = %d\n", pstChnAttr.bFlip);
	printf("bChromaResample = %d\n", pstChnAttr.bChromaResample);
	printf("s32SrcFrameRate = %d\n", pstChnAttr.s32SrcFrameRate);
	printf("s32FrameRate = %d\n", pstChnAttr.s32FrameRate);
	
	/////////////////////////////////////////////////////////////////////////////	
	
    // //VI_StopIsp();
    // HI_MPI_SYS_Exit();
    // HI_MPI_VB_Exit();

    return HI_SUCCESS;
}



