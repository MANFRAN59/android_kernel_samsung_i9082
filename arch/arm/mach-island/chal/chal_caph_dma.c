/*******************************************************************************************
Copyright 2010 Broadcom Corporation.  All rights reserved.

Unless you and Broadcom execute a separate written software license agreement governing use
of this software, this software is licensed to you under the terms of the GNU General Public
License version 2, available at http://www.gnu.org/copyleft/gpl.html (the "GPL").

Notwithstanding the above, under no circumstances may you combine this software in any way
with any other Broadcom software provided under a license other than the GPL, without
Broadcom's express prior written consent.
*******************************************************************************************/

/**
*
*  @file   chal_caph_dma.c
*
*  @brief  chal layer driver for caph dma block
*
****************************************************************************/

#include <chal/chal_caph_dma.h>
#include <mach/rdb/brcm_rdb_cph_aadmac.h>
#include <chal/chal_util.h>
#include <mach/rdb/brcm_rdb_map.h>

/*
 * ****************************************************************************
 *                         G L O B A L   S E C T I O N
 * ****************************************************************************
 */

/*
 * ****************************************************************************
 *  global variable definitions
 * ****************************************************************************
 */


/*
 * ****************************************************************************
 *                          L O C A L   S E C T I O N
 * ****************************************************************************
 */

/*
 * ****************************************************************************
 *  local macro declarations
 * ****************************************************************************
 */

/* Max number of chnnels supported by Hardware */
#define CHAL_CAPH_DMA_MAX_CHANNELS      16

/* Max number of Timestamp chnnels supported by Hardware */
#define CHAL_CAPH_DMA_MAX_TS_CHANNELS   4

/* Total Registers Size of each DMA channel  (CR1 + CR2 + SR1) */
#define CHAL_CAPH_DMA_CH_REG_SIZE       ((CPH_AADMAC_CH2_AADMAC_CR_1_OFFSET - CPH_AADMAC_CH1_AADMAC_CR_1_OFFSET)/sizeof(uint32_t))

/* Timestamp registers Size of each DMA channel (TS) */
#define CHAL_CAPH_DMA_CH_TS_REG_SIZE    ((CPH_AADMAC_CH2_AADMAC_TS_OFFSET-CPH_AADMAC_CH1_AADMAC_TS_OFFSET)/sizeof(uint32_t))


#define CPH_AADMAC_DMA_MAX_WRAP_SIZE          0x10000
#define CPH_AADMAC_DMA_CH1_2_MAX_WRAP_SIZE    0x1000000


/*
 * ****************************************************************************
 *  local typedef declarations
 * ****************************************************************************
 */
typedef struct
{
    uint32_t base;                                       /* Register Base address */
    _Bool   alloc_status[CHAL_CAPH_DMA_MAX_CHANNELS];   /* allocation status for each channel */
} chal_caph_dma_cb_t;

/*
 * ****************************************************************************
 *  local variable definitions
 * ****************************************************************************
 */

/* chal control block where all information is stored */
static  chal_caph_dma_cb_t   chal_caph_dma_cb;


/*
 * ****************************************************************************
 *  local function declarations
 * ****************************************************************************
 */



/*
 * ******************************************************************************
 *  local function definitions
 * ******************************************************************************
 */


/****************************************************************************
*
*  Function Name: CHAL_HANDLE chal_caph_dma_init(uint32_t baseAddress)
*
*  Description: init CAPH DMA block
*
****************************************************************************/
CHAL_HANDLE chal_caph_dma_init(uint32_t baseAddress)
{
    uint8_t  ch;

    /* Go through all the channels and set them not allocated */
    for(ch=0; ch < CHAL_CAPH_DMA_MAX_CHANNELS; ch++)
    {
        chal_caph_dma_cb.alloc_status[ch] = FALSE;
    }

    /* Set the register base address to the caller supplied base address */
    chal_caph_dma_cb.base = baseAddress;

	return(CHAL_HANDLE)(&chal_caph_dma_cb);
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_deinit(CHAL_HANDLE handle)
*
*  Description: deinit CAPH DMA block
*
****************************************************************************/
void chal_caph_dma_deinit(CHAL_HANDLE handle)
{
    chal_caph_dma_cb_t  *pchal_cb = (chal_caph_dma_cb_t*)handle;
    uint8_t  ch;

    /* Go through all the channels and set them not allocated */
    for(ch=0; ch < CHAL_CAPH_DMA_MAX_CHANNELS; ch++)
    {
        pchal_cb->alloc_status[ch] = FALSE;
    }

    /* Reset the register base address */
    pchal_cb->base = 0;

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_enable(CHAL_HANDLE handle,
*			uint16_t channel)
*
*  Description: enable CAPH DMA channel
*
****************************************************************************/
void chal_caph_dma_enable(CHAL_HANDLE handle,
			uint16_t channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     reg_val;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, enable the channel */
            reg_val = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));
            reg_val |= CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_EN_MASK;
            BRCM_WRITE_REG_IDX(base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), reg_val);
        }

    }

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_disable(CHAL_HANDLE handle,
*			uint16_t channel)
*
*  Description: disable CAPH DMA channel
*
****************************************************************************/
void chal_caph_dma_disable(CHAL_HANDLE handle,
			uint16_t channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     reg_val;

    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, disable the channel */
            reg_val = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));
            reg_val &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_EN_MASK;
            BRCM_WRITE_REG_IDX(base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), reg_val);
        }

    }


	return;
}

/****************************************************************************
*
*  Function Name: CAPH_DMA_CHANNEL_e chal_caph_dma_alloc_channel(CHAL_HANDLE handle)
*
*  Description: allocate CAPH DMA channel
*
****************************************************************************/
CAPH_DMA_CHANNEL_e chal_caph_dma_alloc_channel(CHAL_HANDLE handle)
{
    chal_caph_dma_cb_t  *pchal_cb = (chal_caph_dma_cb_t*)handle;
    uint32_t              ch = 0;

    if(ch==0)
    {
        /* Look for a free (non-allocated) channel  */
        for(;ch<CHAL_CAPH_DMA_MAX_CHANNELS;ch++)
        {
            if(pchal_cb->alloc_status[ch] == FALSE)
            {
                /* Found one */
                break;
            }
        }
    }

    if(ch < CHAL_CAPH_DMA_MAX_CHANNELS)
    {
        /* Found a free channel */
        pchal_cb->alloc_status[ch] = TRUE;
    }

    /* Convert to CAPH_DMA_CHANNEL_e format and return */
    return (CAPH_DMA_CHANNEL_e)(1UL << ch);
}

/****************************************************************************
*
*  Function Name: CAPH_DMA_CHANNEL_e chal_caph_dma_alloc_given_channel(CHAL_HANDLE handle)
*
*  Description: allocate a given CAPH DMA channel
*
****************************************************************************/
CAPH_DMA_CHANNEL_e chal_caph_dma_alloc_given_channel(CHAL_HANDLE handle, CAPH_DMA_CHANNEL_e channel)
{
    chal_caph_dma_cb_t  *pchal_cb = (chal_caph_dma_cb_t*)handle;
    uint32_t              ch = 0;

    if(channel != CAPH_DMA_CH_VOID)
    {
        /* Look whether the given channel is allocated or not  */
        for(;ch<CHAL_CAPH_DMA_MAX_CHANNELS;ch++)
        {
            if((1UL << ch)&channel)
            {
                if(pchal_cb->alloc_status[ch] == FALSE)
                {
                    /* Requested channel is not allocated */
                    break;
                }
            }
        }

        /* Requested channel is already allocated */
    }
    else
    {
        /* Look for a free (non-allocated) channel  */
        for(;ch<CHAL_CAPH_DMA_MAX_CHANNELS;ch++)
        {
            if(pchal_cb->alloc_status[ch] == FALSE)
            {
                /* Found one */
                break;
            }
        }
    }

    if(ch < CHAL_CAPH_DMA_MAX_CHANNELS)
    {
        /* Found a free channel */
        pchal_cb->alloc_status[ch] = TRUE;
    }

    /* Convert to CAPH_DMA_CHANNEL_e format and return */
    return (CAPH_DMA_CHANNEL_e)(1UL << ch);
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_free_channel(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: free CAPH DMA channel
*
****************************************************************************/
void chal_caph_dma_free_channel(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    chal_caph_dma_cb_t  *pchal_cb = (chal_caph_dma_cb_t*)handle;
    uint8_t              index;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, make this channel free for next allocation */
            pchal_cb->alloc_status[index] = FALSE;
            break;
        }

    }

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_set_direction(CHAL_HANDLE handle,
*                   CAPH_DMA_CHANNEL_e channel,
*			CAPH_CFIFO_CHNL_DIRECTION_e direction)
*
*  Description: config CAPH DMA channel transfer direction
*
****************************************************************************/
void chal_caph_dma_set_direction(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel,
			CAPH_CFIFO_CHNL_DIRECTION_e direction)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = 0;

    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* Get the current configuration */
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* configure the direction of transfer */
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_IN_OUT_MASK;
            cr |= (direction << CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_IN_OUT_SHIFT);

            /* Apply the settings in the hardware */
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);
            break;
        }

    }

	return;
}


/****************************************************************************
*
*  Function Name: void chal_caph_dma_set_cfifo(CHAL_HANDLE handle,
*                   CAPH_DMA_CHANNEL_e channel,
*			CAPH_CFIFO_e            cfifo_id)
*
*  Description: config CAPH DMA channel cfifo channel id
*
****************************************************************************/
void chal_caph_dma_set_cfifo(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel,
			CAPH_CFIFO_e       cfifo_id)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = 0;
    uint8_t      cfifo_ch;

    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* configure the CFIFO channel */
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_FIFO_CH_MASK;

            /* Find the cfifo channel we are looking for */
            for(cfifo_ch = 0; cfifo_ch < CHAL_CAPH_DMA_MAX_CHANNELS; cfifo_ch++)
            {
                if((1UL << cfifo_ch)&cfifo_id)
                {
                    cr |= (cfifo_ch << CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_FIFO_CH_SHIFT);
                    break;
                }
            }

            /* Apply the settings in the hardware */
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);
            break;
        }

    }

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_set_tsize(CHAL_HANDLE handle,
*                   CAPH_DMA_CHANNEL_e channel,
*			uint8_t              tsize)
*
*  Description: config CAPH DMA channel transfer size
*
****************************************************************************/
void chal_caph_dma_set_tsize(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel,
			uint8_t             tsize)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = 0;

    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Convert the tsize to the register value,
                        tSIZE should be in number of 32bit transfers -1 */
            if(tsize > sizeof(uint32_t))
            {
                tsize = (tsize/sizeof(uint32_t)) - 1;
            }
            else
            {
                tsize = 0;
            }

            /* Configure the transfer size per request */
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_TSIZE_PER_REQ_MASK;
            cr |= (tsize << CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_TSIZE_PER_REQ_SHIFT);

            /* Apply the settings in the hardware */
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);
            break;
        }

    }

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_set_buffer(CHAL_HANDLE handle,
*                   CAPH_DMA_CHANNEL_e channel,
*			uint32_t             address,
*                   uint32_t             size)
*
*  Description: config CAPH DMA channel buffer parameters (address and size)
*
****************************************************************************/
void chal_caph_dma_set_buffer(CHAL_HANDLE handle,
            CAPH_DMA_CHANNEL_e  channel,
            uint32_t             address,
            uint32_t             size)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = 0;
    uint32_t     maxs = CPH_AADMAC_DMA_MAX_WRAP_SIZE;
    uint32_t     extds = 0;

    if(channel == CAPH_DMA_CH1 || channel == CAPH_DMA_CH2)
    {
        maxs = CPH_AADMAC_DMA_CH1_2_MAX_WRAP_SIZE;

    }

    if(size > maxs)
    {
        size = maxs;
    }

    /* Set the extended size */
    extds = size >> 16;
    size &= CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_WRAP_MASK;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* Set the DMA buffer Address */
            address &= CPH_AADMAC_CH1_AADMAC_CR_1_CH1_AADMAC_BASE_MASK;
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_1, (index*CHAL_CAPH_DMA_CH_REG_SIZE), address);


            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Configure the size of buffer  */
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_WRAP_MASK;
            cr |= (size << CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_WRAP_SHIFT);

            /* Apply the settings in the hardware */
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);

            if(channel == CAPH_DMA_CH1 || channel == CAPH_DMA_CH2)
            {
                cr = BRCM_READ_REG( base,  CPH_AADMAC_CH1_2_EXTENDED_WRAP);
                if(channel == CAPH_DMA_CH1)
                {
                    cr &= (~CPH_AADMAC_CH1_2_EXTENDED_WRAP_CH1_AADMAC_WRAP_HIGH_MASK);
                    cr |= (extds << CPH_AADMAC_CH1_2_EXTENDED_WRAP_CH1_AADMAC_WRAP_HIGH_SHIFT);
                }
                else
                {
                    cr &= (~CPH_AADMAC_CH1_2_EXTENDED_WRAP_CH2_AADMAC_WRAP_HIGH_MASK);
                    cr |= (extds << CPH_AADMAC_CH1_2_EXTENDED_WRAP_CH2_AADMAC_WRAP_HIGH_SHIFT);
                }
                /* Apply the settings in the hardware */
                BRCM_WRITE_REG( base,  CPH_AADMAC_CH1_2_EXTENDED_WRAP, cr);
            }
            break;
        }

    }

	return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_set_ddrfifo_status(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel,
*			CAPH_DMA_CHNL_FIFO_STATUS_e status)
*
*  Description: set CAPH DMA ddr fifo status
*
****************************************************************************/
void chal_caph_dma_set_ddrfifo_status(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel,
			CAPH_DMA_CHNL_FIFO_STATUS_e status)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = 0;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, Set the DDR fifo status */
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));
            if(status == CAPH_READY_NONE)
            {
                cr &= (~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_LOW_MASK);
                cr &= (~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_HIGH_MASK);
            }
            else
            {
                cr |= (status << CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_LOW_SHIFT);
            }
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);
            break;
        }
    }

    return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_clr_ddrfifo_status(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel,
*			CAPH_DMA_CHNL_FIFO_STATUS_e status)
*
*  Description: set CAPH DMA ddr fifo status
*
****************************************************************************/
void chal_caph_dma_clr_ddrfifo_status(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel,
			CAPH_DMA_CHNL_FIFO_STATUS_e status)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     sr = 0;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, Set the DDR fifo status */
            sr = (~status) & CAPH_READY_HIGHLOW;
            sr <<= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_HW_READY_LOW_SHIFT;
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_SR_1, (index*CHAL_CAPH_DMA_CH_REG_SIZE), sr);
            break;
        }

    }

    return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_clr_channel_fifo(CHAL_HANDLE handle,
*			uint16_t fifo)
*
*  Description: clear dma channel internal fifo
*
****************************************************************************/
void chal_caph_dma_clr_channel_fifo(CHAL_HANDLE handle,
			uint16_t channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr;


    /* Find the FIFOs we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the Channel we are looking for, Disable the FIFO */
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Send Reset Pulse to the Hardware. First make sure it is 0, set to 1, then clear to 0 */
            /* Clear Reset */
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_FIFO_RST_MASK;
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);

            /* Start Reset  process on Hardware*/
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));
            cr |= CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_FIFO_RST_MASK;
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);


            /* Clear Reset */
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));
            cr &= ~CPH_AADMAC_CH1_AADMAC_CR_2_CH1_AADMAC_FIFO_RST_MASK;
            BRCM_WRITE_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE), cr);
        }

    }

    return;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_read_ddrfifo_sw_status(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: read CAPH DMA ddr fifo sw status
*
****************************************************************************/
CAPH_DMA_CHNL_FIFO_STATUS_e chal_caph_dma_read_ddrfifo_sw_status(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     cr = (uint32_t)CAPH_READY_NONE;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, Get the channel status */
            cr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_CR_2, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Retrieve the DDR FIFO staus information from status */
            cr &= (CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_HIGH_MASK|CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_LOW_MASK);
            cr >>= CPH_AADMAC_CH1_AADMAC_CR_2_CH1_SW_READY_LOW_SHIFT;
            break;
        }
    }

    return (CAPH_DMA_CHNL_FIFO_STATUS_e)cr;
}

/****************************************************************************
*
*  Function Name: void chal_caph_dma_read_ddrfifo_status(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: read CAPH DMA ddr fifo status
*
****************************************************************************/
CAPH_DMA_CHNL_FIFO_STATUS_e chal_caph_dma_read_ddrfifo_status(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     sr = (uint32_t)CAPH_READY_NONE;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, Get the channel status */
            sr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_SR_1, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Retrieve the DDR FIFO staus information from status */
            sr &= (CPH_AADMAC_CH1_AADMAC_SR_1_CH1_HW_READY_HIGH_MASK|CPH_AADMAC_CH1_AADMAC_SR_1_CH1_HW_READY_LOW_MASK);
            sr >>= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_HW_READY_LOW_SHIFT;
            break;
        }
    }

    return (CAPH_DMA_CHNL_FIFO_STATUS_e)sr;
}

/****************************************************************************
*
*  Function Name: uint8_t chal_caph_dma_read_reqcount(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: read CAPH DMA request count
*
****************************************************************************/
uint8_t chal_caph_dma_read_reqcount(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     sr = 0;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* found the channel we are looking for, Get the channel status */
            sr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_SR_1, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Retrieve the request count information from status */
            sr &= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_AADMAC_REQUEST_COUNT_MASK;
            sr >>= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_AADMAC_REQUEST_COUNT_SHIFT;
            break;
        }

    }

    return (uint8_t)sr;
}

/****************************************************************************
*
*  Function Name: uint16_t chal_caph_dma_read_currmempointer(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: read CAPH DMA current memory pointer
*
****************************************************************************/
uint16_t chal_caph_dma_read_currmempointer(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     sr = 0;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* Get the channel status */
            sr = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_SR_1, (index*CHAL_CAPH_DMA_CH_REG_SIZE));

            /* Extract Current Memory pointer information from the status */
            sr &= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_AADMAC_CURRENT_MEM_PNTR_MASK;
            sr >>= CPH_AADMAC_CH1_AADMAC_SR_1_CH1_AADMAC_CURRENT_MEM_PNTR_SHIFT;
            break;
        }

    }

    /* return the Current DMA Memory Pointer */
    return (uint16_t)sr;
}

/****************************************************************************
*
*  Function Name: uint32_t chal_caph_dma_read_timestamp(CHAL_HANDLE handle,
*			CAPH_DMA_CHANNEL_e channel)
*
*  Description: read CAPH DMA channel timestamp
*
****************************************************************************/
uint32_t chal_caph_dma_read_timestamp(CHAL_HANDLE handle,
			CAPH_DMA_CHANNEL_e channel)
{
    uint32_t     base = ((chal_caph_dma_cb_t*)handle)->base;
    uint8_t      index;
    uint32_t     ts = 0;


    /* Find the channel we are looking for */
    for(index = 0; index < CHAL_CAPH_DMA_MAX_TS_CHANNELS; index++)
    {
        if((1UL << index)&channel)
        {
            /* Read the timestamp corresponding to the selected channel */
            ts = BRCM_READ_REG_IDX( base,  CPH_AADMAC_CH1_AADMAC_TS, (index*CHAL_CAPH_DMA_CH_TS_REG_SIZE));
            break;
        }

    }

    /* return the timestamp */
    return ts;
}

#if defined( __KERNEL__ )

#include <linux/module.h>

EXPORT_SYMBOL(chal_caph_dma_init);
EXPORT_SYMBOL(chal_caph_dma_deinit);
EXPORT_SYMBOL(chal_caph_dma_enable);
EXPORT_SYMBOL(chal_caph_dma_disable);
EXPORT_SYMBOL(chal_caph_dma_alloc_channel);
EXPORT_SYMBOL(chal_caph_dma_alloc_given_channel);
EXPORT_SYMBOL(chal_caph_dma_free_channel);
EXPORT_SYMBOL(chal_caph_dma_set_direction);
EXPORT_SYMBOL(chal_caph_dma_set_cfifo);
EXPORT_SYMBOL(chal_caph_dma_set_tsize);
EXPORT_SYMBOL(chal_caph_dma_set_buffer);
EXPORT_SYMBOL(chal_caph_dma_set_ddrfifo_status);
EXPORT_SYMBOL(chal_caph_dma_clr_ddrfifo_status);
EXPORT_SYMBOL(chal_caph_dma_clr_channel_fifo);
EXPORT_SYMBOL(chal_caph_dma_read_ddrfifo_sw_status);
EXPORT_SYMBOL(chal_caph_dma_read_ddrfifo_status);
EXPORT_SYMBOL(chal_caph_dma_read_reqcount);
EXPORT_SYMBOL(chal_caph_dma_read_currmempointer);
EXPORT_SYMBOL(chal_caph_dma_read_timestamp);

#endif

