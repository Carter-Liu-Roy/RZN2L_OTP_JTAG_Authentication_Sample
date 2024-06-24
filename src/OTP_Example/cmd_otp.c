/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/******************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "hal_data.h"
#include "cmd_otp.h"
#include "otp.h"
#include "common.h"

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
/* WORDLOCK status */
#define WORDLOCK_DISABLE         (0U)
#define WORDLOCK_ENABLE          (1U)

/* SCI/USB boot mode */
#define SCIUSB_BOOT_MODE_ENABLE  (0U)
#define SCIUSB_BOOT_MODE_DISABLE (1U)
#define SCIUSB_BOOT_DIS_OFFSET   (1U)

/******************************************************************************
 * @brief Write to OTP.
 *
 * @param[in]  otp_addr       Write address
 * @param[in]  data           Write data
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_DATA_FAIL   Data error
 * @retval RET_WRITE_FAIL  Write error
 ******************************************************************************/
uint8_t cmd_write_otp (uint16_t otp_addr, uint16_t data)
{
    uint8_t ret       = RET_SUCCESS;
    uint8_t wordlock  = WORDLOCK_DISABLE;
    otp_err_t otp_err = OTP_SUCCESS;
    
    /* Check address. */
    if ((COUNTER_AREA_START_ADDR <= otp_addr ) && (otp_addr <= COUNTER_AREA_END_ADDR))
    {
        /* Specify the address of the anti-rollback counter area.  */
    }
    else if ((USER_AREA_START_ADDR <= otp_addr ) && (otp_addr <= USER_AREA_END_ADDR))
    {
        /* Specify the address of the user area. */
        wordlock = WORDLOCK_ENABLE;
    }
#if defined(BSP_MCU_GROUP_RZN2L) || defined(BSP_MCU_GROUP_RZT2L)
    else if ((SHOSTIF_BOOT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= SHOSTIF_BOOT_AREA_END_ADDR))
    {
        /* Specify the address of the SHOSTIF boot mode area. */
        wordlock = WORDLOCK_ENABLE;
    }
#endif
#if defined(BSP_MCU_GROUP_RZN2L)
    else if ((PHOSTIF_BOOT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= PHOSTIF_BOOT_AREA_END_ADDR))
    {
        /* Specify the address of the PHOSTIF boot mode area. */
        wordlock = WORDLOCK_ENABLE;
    }
#endif
    else
    {
        /* Specify the address of the non-write area. */
        return RET_DATA_FAIL;
    }
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_WRITE_FAIL;
    }
    
    do
    {
        /* Write data. */
        otp_err = write_otp_data(otp_addr, data);
            
        if (OTP_SUCCESS != otp_err)
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        uint16_t read_data = 0U;
        
        /* Read the data from the write destination area and check the data. */
        otp_err = read_otp_data(otp_addr, &read_data);
        
        if (WORDLOCK_DISABLE == wordlock)
        {
            read_data &= data;
        }
        
        if ((OTP_SUCCESS != otp_err) || (data != read_data))
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
    } while (0);
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}

/******************************************************************************
 * @brief Read OTP. 
 *
 * @param[in]  otp_addr       Read address
 * @param[in]  p_data         Read data storage destination
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_DATA_FAIL   Data error
 * @retval RET_READ_FAIL   Read error
 ******************************************************************************/
uint8_t cmd_read_otp (uint16_t otp_addr, uint16_t *p_data)
{
    uint8_t ret       = RET_SUCCESS;
    otp_err_t otp_err = OTP_SUCCESS;
    
    /* Check address. */
    if ((COUNTER_AREA_START_ADDR <= otp_addr ) && (otp_addr <= COUNTER_AREA_END_ADDR))
    {
        /* Specify the address of the anti-rollback counter area. */
    }
    else if ((PART_NUM_ADDR == otp_addr) || (PRODUCT_VER_ADDR == otp_addr))
    {
        /* Specify the address of part number or product version. */
    }
#if defined(BSP_MCU_GROUP_RZT2M) || defined(BSP_MCU_GROUP_RZN2L)
    else if ((TX_CLOCK_CNT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= TX_CLOCK_CNT_AREA_END_ADDR))
    {
        /* Specify the address of the TX clock count area. */
    }
    else if ((RX_CLOCK_CNT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= RX_CLOCK_CNT_AREA_END_ADDR))
    {
        /* Specify the address of the RX clock count area. */
    }
#endif
    else if ((USER_AREA_START_ADDR <= otp_addr ) && (otp_addr <= USER_AREA_END_ADDR))
    {
        /* Specify the address of the user area. */
    }
#if defined(BSP_MCU_GROUP_RZN2L) || defined(BSP_MCU_GROUP_RZT2L)
    else if ((SHOSTIF_BOOT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= SHOSTIF_BOOT_AREA_END_ADDR))
    {
        /* Specify the address of the SHOSTIF boot mode area. */
    }
#endif
#if defined(BSP_MCU_GROUP_RZN2L)
    else if ((PHOSTIF_BOOT_AREA_START_ADDR <= otp_addr ) && (otp_addr <= PHOSTIF_BOOT_AREA_END_ADDR))
    {
        /* Specify the address of the PHOSTIF boot mode area. */
    }
#endif
    else
    {
        /* Specify the address of the non-read area. */
        return RET_DATA_FAIL;
    }
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_READ_FAIL;
    }
    
    /* Read data. */
    otp_err = read_otp_data(otp_addr, p_data);
    
    if (OTP_SUCCESS != otp_err)
    {
        ret = RET_READ_FAIL;
    }
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}

/******************************************************************************
 * @brief Setup SCI/USB boot.
 *
 * @param[in]  mode           SCI/USB boot mode
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_READ_FAIL   Read error
 * @retval RET_WRITE_FAIL  Write error
 ******************************************************************************/
uint8_t cmd_set_sci_usb_boot (uint8_t mode)
{
    uint8_t  ret          = RET_SUCCESS;
    uint16_t current_mode = 0U;
    uint16_t set_mode     = (uint16_t)(mode << SCIUSB_BOOT_DIS_OFFSET);
    otp_err_t otp_err     = OTP_SUCCESS;
    
    /* Check if boot mode disabled is specified. */
    if (SCIUSB_BOOT_MODE_DISABLE == mode)
    {
        /* OTP power on. */
        otp_err = otp_power_on();
        
        if (OTP_SUCCESS != otp_err)
        {
            return RET_WRITE_FAIL;
        }
        
        do
        {
            /* Read SCU/USB boot setting value. */
            otp_err = read_otp_data(SCI_USB_BOOT_ADDR, &current_mode);
            
            if (OTP_SUCCESS != otp_err)
            {
                ret = RET_READ_FAIL;
                break;
            }
            
            if (set_mode <= current_mode)
            {
                break;
            }
            
            /* Write SCI/USB boot setting value. */
            otp_err = write_otp_data(SCI_USB_BOOT_ADDR, set_mode);
            
            if (OTP_SUCCESS != otp_err)
            {
                 ret = RET_WRITE_FAIL;
                 break;
            }
            
            /* Read the current setting value and check if the value is updated. */
            otp_err = read_otp_data(SCI_USB_BOOT_ADDR, &current_mode);
            
            if ((OTP_SUCCESS != otp_err) || (current_mode < set_mode))
            {
                ret = RET_WRITE_FAIL;
                break;
            }
            
        } while (0);
        
        /* OTP power off. */
        otp_power_off();
    }
    
    return ret;
}

/******************************************************************************
 * @brief Get SCI/USB boot information.
 *
 * @param[out]  p_mode         SCI/USB boot mode
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_READ_FAIL   Read error
 ******************************************************************************/
uint8_t cmd_get_sci_usb_boot (uint8_t *p_mode)
{
    uint8_t  ret          = RET_SUCCESS;
    uint16_t current_mode = 0U;
    otp_err_t otp_err     = OTP_SUCCESS;
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_READ_FAIL;
    }
    
    do
    {
        /* Read SCI/USB boot setting value. */
        otp_err = read_otp_data(SCI_USB_BOOT_ADDR, &current_mode);
        
        if (OTP_SUCCESS != otp_err)
        {
            ret = RET_READ_FAIL;
            break;
        }
        
        *p_mode = (uint8_t)(current_mode >> SCIUSB_BOOT_DIS_OFFSET);
        
    } while (0);
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}

/******************************************************************************
 * @brief Read unique ID.
 *
 * @param[out]  p_uid         Unique ID
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_READ_FAIL   Read error
 ******************************************************************************/
uint8_t cmd_get_unique_id(uint8_t *p_uid)
{
    uint8_t ret       = RET_SUCCESS;
    otp_err_t otp_err = OTP_SUCCESS;
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_READ_FAIL;
    }
    
    /* Read unique ID. */
    otp_err = read_otp_multiple_data(UID_ADDR, p_uid, UID_SIZE);
    
    if (OTP_SUCCESS != otp_err)
    {
        ret = RET_READ_FAIL;
    }
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}
