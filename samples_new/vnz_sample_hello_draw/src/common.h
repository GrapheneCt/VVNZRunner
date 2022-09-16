#ifndef _COMMON_H
#define _COMMON_H

#define FB_WIDTH			960
#define FB_HEIGHT			544
#define FB_COUNT			2

#define VNZ_CMD_RENDER			1

#define SCE_CTRL_SELECT				(1<<0)			/*J SELECT???           */
/*E SELECT button          */
#define SCE_CTRL_L3					(1<<1)			/*J L3???               */
													/*E L3 button              */
#define SCE_CTRL_R3					(1<<2)			/*J R3???               */
													/*E R3 button              */
#define SCE_CTRL_START				(1<<3)			/*J START???            */
													/*E START button           */
#define SCE_CTRL_UP					(1<<4)			/*J ???? ????      */
													/*E direction button UP    */
#define SCE_CTRL_RIGHT				(1<<5)			/*J ???? ????      */
													/*E direction button RIGHT */
#define SCE_CTRL_DOWN				(1<<6)			/*J ???? ????      */
													/*E direction button DOWN  */
#define SCE_CTRL_LEFT				(1<<7)			/*J ???? ????      */
													/*E direction button LEFT  */
#define SCE_CTRL_L					(1<<8)			/*J L???                */
													/*E L button               */
#define SCE_CTRL_R					(1<<9)			/*J R???                */
													/*E R button               */
#define SCE_CTRL_L2					(1<<8)			/*J L2???               */
													/*E L2 button              */
#define SCE_CTRL_R2					(1<<9)			/*J R2???               */
													/*E R2 button              */
#define SCE_CTRL_L1					(1<<10)			/*J L1???               */
													/*E L1 button              */
#define SCE_CTRL_R1					(1<<11)			/*J R1???               */
													/*E R1 button              */
#define SCE_CTRL_TRIANGLE			(1<<12)			/*J ????               */
													/*E TRIANGLE button        */
#define SCE_CTRL_CIRCLE				(1<<13)			/*J ????               */
													/*E CIRCLE button          */
#define SCE_CTRL_CROSS				(1<<14)			/*J ????               */
													/*E CROSS button           */
#define SCE_CTRL_SQUARE				(1<<15)			/*J ????               */
													/*E SQUARE button          */
#define SCE_CTRL_INTERCEPTED		(1<<16)			/*J ???????         */
													/*E input intercepted      */

typedef struct SceCtrlData {
	unsigned long long	timeStamp;
	unsigned int		buttons;
	unsigned char		lx;
	unsigned char		ly;
	unsigned char		rx;
	unsigned char		ry;
	unsigned char		rsrv[16];
} SceCtrlData;

typedef struct VnzRenderArg {
	int cmd;
	int fbidx;
	void *fbmem[FB_COUNT];
	SceCtrlData ctrl[2];
} VnzRenderArg;

#endif /* _COMMON_H */