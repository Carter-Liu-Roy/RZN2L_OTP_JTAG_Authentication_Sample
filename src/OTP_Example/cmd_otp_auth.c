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
#include "cmd_otp_auth.h"
#include "otp.h"
#include "common.h"

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
/* JTAG authentication mode */
#define JTAG_MODE_NO_AUTH         (0U)
#define JTAG_MODE_AUTHLV1         (1U)
#define JTAG_MODE_AUTHLV2         (2U)
#define JTAG_MODE_PROHIBIT        (4U)

/* Authentication type */
#define TYPE_PLAIN                (0U)
#define TYPE_HASH                 (1U)

/* Authentication ID size */
#define SIZE_PLAIN_ID             (16U)
#define SIZE_HASH_ID              (32U)

/******************************************************************************
 * @brief Setup JTAG authentication.
 *
 * @param[in]  mode           Authentication mode
 * @param[in]  type           Authentication type
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_DATA_FAIL   Data error
 * @retval RET_WRITE_FAIL  Write error
 ******************************************************************************/
uint8_t cmd_set_jtag_auth (uint8_t mode, uint8_t type)
{
    uint8_t  ret          = RET_SUCCESS;
    uint16_t current_mode = 0U;
    uint16_t current_type = 0U;
    uint16_t set_mode     = mode;
    otp_err_t otp_err     = OTP_SUCCESS;
    bool mode_is_invalid  = false;
    
    /* Check authentication mode and type. */
    switch (mode)
    {
        case JTAG_MODE_AUTHLV1:
        case JTAG_MODE_AUTHLV2:
        case JTAG_MODE_PROHIBIT:
            break;
        
        /* Unknown mode. */
        default:
            mode_is_invalid = true;
    }
    
    if ((TYPE_PLAIN != type) || (true == mode_is_invalid))
    {
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
        /* Read the current setting value. (authentication type value) */
        /* The authentication type must be plain. */
        otp_err = read_otp_data(JTAG_AUTH_TYPE_ADDR, &current_type);
        
        if ((OTP_SUCCESS != otp_err) || (TYPE_PLAIN != current_type))
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        /* Read the current setting value. (authentication mode value) */
        /* Check if the specified value can be set. */
        otp_err = read_otp_data(JTAG_AUTH_MODE_ADDR, &current_mode);
        
        if ((OTP_SUCCESS != otp_err) || (current_mode >= set_mode))
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        /* Write the specified setting value. */
        otp_err = write_otp_data(JTAG_AUTH_MODE_ADDR, set_mode);
        
        if (OTP_SUCCESS != otp_err)
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        /* Read the current setting value and check if the value is updated. */
        otp_err = read_otp_data(JTAG_AUTH_MODE_ADDR, &current_mode);
        
        if ((OTP_SUCCESS != otp_err) || (current_mode < set_mode))
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
 * @brief Get JTAG authentication information.
 *
 * @param[out]  p_mode         Authentication mode
 * @param[out]  p_type         Authentication type
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_DATA_FAIL   Data error
 * @retval RET_READ_FAIL   Read error
 ******************************************************************************/
uint8_t cmd_get_jtag_auth (uint8_t *p_mode, uint8_t *p_type)
{
    uint8_t  ret          = RET_SUCCESS;
    uint16_t current_mode = 0U;
    uint16_t current_type = 0U;
    otp_err_t otp_err     = OTP_SUCCESS;
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_WRITE_FAIL;
    }
    
    do
    {
        /* Read authentication mode. */
        otp_err = read_otp_data(JTAG_AUTH_MODE_ADDR, &current_mode);
        
        if (OTP_SUCCESS != otp_err)
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        if (current_mode & JTAG_MODE_PROHIBIT)
        {
            /* Permanent prohibition. */
            *p_mode = JTAG_MODE_PROHIBIT;
        }
        else if (current_mode & JTAG_MODE_AUTHLV2)
        {
            /* Authentication lv2. */
            *p_mode = JTAG_MODE_AUTHLV2;
        }
        else if (current_mode & JTAG_MODE_AUTHLV1)
        {
            /* Authentication lv1. */
            *p_mode = JTAG_MODE_AUTHLV1;
        }
        else
        {
            /* No authentication. */
            *p_mode = JTAG_MODE_NO_AUTH;
        }
        
        /* Read authentication type. */
        otp_err = read_otp_data(JTAG_AUTH_TYPE_ADDR, &current_type);
        
        if (OTP_SUCCESS != otp_err)
        {
            ret = RET_WRITE_FAIL;
            break;
        }
        
        if (1U == current_type)
        {
            /* Hash. */
            *p_type = TYPE_HASH;
        }
        else
        {
            /* Plain. */
            *p_type = TYPE_PLAIN;
        }
        
    } while (0);
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}

/******************************************************************************
 * @brief Setup JTAG authentication ID.
 *
 * @param[in]  mode           Authentication mode
 * @param[in]  type           Authentication type
 * @param[in]  p_id           Authentication ID
 *
 * @retval RET_SUCCESS     Success
 * @retval RET_DATA_FAIL   Data error
 * @retval RET_WRITE_FAIL  Write error
 ******************************************************************************/
uint8_t cmd_set_jtag_auth_id (uint8_t mode, uint8_t type, uint8_t * const p_id)
{
    uint8_t  ret         = RET_SUCCESS;
    uint16_t addr        = 0U;
    otp_err_t otp_err    = OTP_SUCCESS;
    bool mode_is_invalid = false;
    
    /* Check where to write the authentication ID. */
    switch (mode)
    {
        case JTAG_MODE_AUTHLV1:
            addr = JTAG_AUTH_ID1_PLAIN_ADDR;
            break;
            
        case JTAG_MODE_AUTHLV2:
            addr = JTAG_AUTH_ID4_PLAIN_ADDR;
            break;
        
        /* Unknown mode. */
        default:
            mode_is_invalid = true;
    }
    
    /* Check authentication mode and type. */
    if ((TYPE_PLAIN != type) || (true == mode_is_invalid))
    {
        return RET_DATA_FAIL;
    }
    
    /* OTP power on. */
    otp_err = otp_power_on();
    
    if (OTP_SUCCESS != otp_err)
    {
        return RET_WRITE_FAIL;
    }
    
    /* Write authentication ID. */
    otp_err = write_otp_multiple_data(addr, p_id, SIZE_PLAIN_ID);
    
    if (OTP_SUCCESS != otp_err)
    {
        ret = RET_WRITE_FAIL;
    }
    
    /* OTP power off. */
    otp_power_off();
    
    return ret;
}

