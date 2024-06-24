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
#include "otp.h"

/******************************************************************************
 * @brief OTP power on.
 *
 * @retval OTP_SUCCESS   Success
 * @retval OTP_ERROR     Failure
 ******************************************************************************/
otp_err_t otp_power_on(void)
{
    /* Confirm that CMD_RDY bit of the OTP access status register (OTPSTR.CMD_RDY) is 1. */
    if (0U == R_OTP->OTPSTR_b.CMD_RDY)
    {
        /* Confirm completion of the power-down process. */
        if (1U ==  R_OTP->OTPPWR_b.PWR)
        {
            return OTP_ERROR;
        }
    }
    
    /* Set the PWR and ACCL bits of the OTP Power Control Register. */
    R_OTP->OTPPWR_b.PWR  = 1U;
    R_OTP->OTPPWR_b.ACCL = 1U;
    
    return OTP_SUCCESS;
}

/******************************************************************************
 * @brief OTP power off.
 ******************************************************************************/
void otp_power_off(void)
{
    /* Set the PWR and ACCL bits to 0, and wait until CMD_RDY bit is set to 0. */
    R_OTP->OTPPWR_b.PWR  = 0U;
    R_OTP->OTPPWR_b.ACCL = 0U;
    
    while (1)
    {
        if (0U == R_OTP->OTPSTR_b.CMD_RDY)
        {
            break;
        }
    }
    
    return;
}

/******************************************************************************
 * @brief Write 16-bit of data to the OTP.
 *
 * @param[in]  otp_addr       Write address
 * @param[in]  data           Write data
 *
 * @retval OTP_SUCCESS   Success
 * @retval OTP_ERROR     Failure
 ******************************************************************************/
otp_err_t write_otp_data(uint16_t otp_addr, uint16_t data)
{
    otp_err_t ret = OTP_SUCCESS;
    
    /* Confirm that CMD_RDY bit of the OTP access status register (OTPSTR.CMD_RDY) is 1. */
    while (0U == R_OTP->OTPSTR_b.CMD_RDY);
    
    /* Set the PWR and ACCL bits of the OTP Power Control Register. */
    R_OTP->OTPPWR_b.PWR  = 1U;
    R_OTP->OTPPWR_b.ACCL = 1U;
    
    /* Set the write address to the OTP Write Address Register. */
    R_OTP->OTPADRWR_b.ADRWR = otp_addr;
    
    /* Set the data to be written to the OTP Write Data Register. */
    R_OTP->OTPDATAWR_b.DATAWR = data; 
    
    /* Set the STARWR bit of the OTP Write Command Register to 1. */
    R_OTP->OTPSTAWR_b.STAWR = 1U;
    
    /* Poll the STAWR bit untill changing to 0 in order to detect the completion of the write command acceptance. */
    while (1)
    {
        if (0U == R_OTP->OTPSTAWR_b.STAWR)
        {
            break;
        }
    }
    
    /* Poll the CMD_RDY bit untill changing to 1 in order to detect the completion of the write command. */
    while (1)
    {
        if (1U == R_OTP->OTPSTR_b.CMD_RDY)
        {
            break;
        }
    }
    
    /* Check OTP write error. */
    if ((0U != R_OTP->OTPSTR_b.ERR_WR) || (0U != R_OTP->OTPSTR_b.ERR_WP))
    {
        ret = OTP_ERROR;
    }
    
    if (0U != R_OTP->OTPSTR_b.ERR_RDY_WR)
    {
        ret = OTP_ERROR;
        R_OTP->OTPSTR_b.ERR_RDY_WR = 0U;
    }
    
    return ret;
}

/******************************************************************************
 * @brief Read 16-bit of data from OTP.
 *
 * @param[in]  otp_addr       Read address
 * @param[in]  p_data         Read data storage destination
 *
 * @retval OTP_SUCCESS   Success
 * @retval OTP_ERROR     Failure
 ******************************************************************************/
otp_err_t read_otp_data(uint16_t otp_addr, uint16_t *p_data)
{
    otp_err_t ret = OTP_SUCCESS;
    
    /* Confirm that CMD_RDY bit of the OTP access status register (OTPSTR.CMD_RDY) is 1. */
    while (0U == R_OTP->OTPSTR_b.CMD_RDY);
    
    /* Set the PWR and ACCL bits of the OTP Power Control Register. */
    R_OTP->OTPPWR_b.PWR  = 1U;
    R_OTP->OTPPWR_b.ACCL = 1U;
    
    /* Set the read address to the OTP Read Address Register. */
    R_OTP->OTPADRRD_b.ADRRD = otp_addr;
    
    /* Read the OTP Read Data Register. */
    *p_data = R_OTP->OTPDATARD_b.DATARD;
    
    /* Poll the CMD_RDY bit untill changing to 1 in order to detect the completion of the write command. */
    while (1)
    {
        if (1U == R_OTP->OTPSTR_b.CMD_RDY)
        {
            break;
        }
    }
    
    /* Check OTP read error. */
    if (0U != R_OTP->OTPSTR_b.ERR_RP)
    {
        ret = OTP_ERROR;
    }
    
    if (0U != R_OTP->OTPSTR_b.ERR_RDY_RD)
    {
        ret = OTP_ERROR;
        R_OTP->OTPSTR_b.ERR_RDY_RD = 0U;
    }
    
    return ret;
}

/******************************************************************************
 * @brief Write to the OTP area in 16-bit units.
 *
 * @param[in]  addr           Write destination address 
 * @param[in]  p_data         Data
 * @param[in]  data_len       Data length
 *
 * @retval OTP_SUCCESS   Success
 * @retval OTP_ERROR     Failure
 ******************************************************************************/
otp_err_t write_otp_multiple_data(uint16_t addr, uint8_t * const p_data, uint8_t data_len)
{
    otp_err_t ret    = OTP_SUCCESS;
    uint8_t comp_len = 0U;
    
    /* Check if the size is in 16-bit units. */
    if (0U != (data_len % 2U))
    {
        return OTP_ERROR;
    }
    
    /* Write the data in 16-bit units. */
    while (comp_len < data_len)
    {
        uint16_t write_data = 0U;
        memcpy(&write_data, p_data + comp_len, OTP_WRITE_SIZE);
        
        /* Write data. */
        uint8_t write_ret = write_otp_data(addr, write_data);
        
        if (0U != write_ret)
        {
            ret = OTP_ERROR;
            break;
        }
        
        comp_len += OTP_WRITE_SIZE;
        addr++;
    }
    
    return ret;
}

/******************************************************************************
 * @brief Read to the OTP area in 16-bit units.
 *
 * @param[in]  addr           Read destination address 
 * @param[in]  p_data         Data
 * @param[in]  data_len       Data length
 *
 * @retval OTP_SUCCESS   Success
 * @retval OTP_ERROR     Failure
 ******************************************************************************/
otp_err_t read_otp_multiple_data(uint16_t addr, uint8_t *p_data, uint8_t data_len)
{
    otp_err_t ret    = OTP_SUCCESS;
    uint8_t comp_len = 0U;
    
    /* Check if the size is in 16-bit units. */
    if (0U != (data_len % 2U))
    {
        return OTP_ERROR;
    }
    
    /* Read the data in 16-bit units. */
    while (comp_len < data_len)
    {
        uint16_t read_data = 0U;
        
        /* Read data. */
        uint8_t read_ret = read_otp_data(addr, &read_data);
        
        if (0U != read_ret)
        {
            ret = OTP_ERROR;
            break;
        }
        
        memcpy(p_data + comp_len, &read_data, OTP_WRITE_SIZE);
        comp_len += OTP_WRITE_SIZE;
        addr++;
    }
    
    return ret;
}
