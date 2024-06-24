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
#ifndef __OTP_H__
#define __OTP_H__

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
/* Write size at one time */
#define OTP_WRITE_SIZE                 (0x02U)

/* OTP address for User area */
#if defined(BSP_MCU_GROUP_RZT2M) || defined(BSP_MCU_GROUP_RZN2L)
/* Start address for RZ/T2M, RZ/N2L */
#define USER_AREA_START_ADDR           ((uint16_t)0x01BDU)
#elif defined(BSP_MCU_GROUP_RZT2L)
/* Start address for RZ/T2L */
#define USER_AREA_START_ADDR           ((uint16_t)0x01B9U)
#endif
#define USER_AREA_END_ADDR             ((uint16_t)0x01CFU)

/* OTP address for Anti-rollback counter area */
#define COUNTER_AREA_START_ADDR        ((uint16_t)0x0159U)
#define COUNTER_AREA_END_ADDR          ((uint16_t)0x016CU)

/* OTP address for Part number */
#define PART_NUM_ADDR                  ((uint16_t)0x01A7U)

/* OTP address for Product Version */
#define PRODUCT_VER_ADDR               ((uint16_t)0x01A9U)

/* OTP address for RGMII TX clock delay OSC mode clock count area */
#define TX_CLOCK_CNT_AREA_START_ADDR   ((uint16_t)0x01B1U)
#if defined(BSP_MCU_GROUP_RZT2M)
/* End address for RZ/T2M */
#define TX_CLOCK_CNT_AREA_END_ADDR     ((uint16_t)0x01B2U)
#elif defined(BSP_MCU_GROUP_RZN2L)
/* End address for RZ/N2L */
#define TX_CLOCK_CNT_AREA_END_ADDR     ((uint16_t)0x01B1U) 
#endif

/* OTP address for RGMII RX clock delay OSC mode clock count area */
#define RX_CLOCK_CNT_AREA_START_ADDR   ((uint16_t)0x01B3U)
#if defined(BSP_MCU_GROUP_RZT2M)
/* End address for RZ/T2M */
#define RX_CLOCK_CNT_AREA_END_ADDR     ((uint16_t)0x01B4U)
#elif defined(BSP_MCU_GROUP_RZN2L)
/* End address for RZ/N2L */
#define RX_CLOCK_CNT_AREA_END_ADDR     ((uint16_t)0x01B3U)
#endif

#if defined(BSP_MCU_GROUP_RZN2L) || defined(BSP_MCU_GROUP_RZT2L)
/* SHOSTIF boot mode area */
#define SHOSTIF_BOOT_AREA_START_ADDR   ((uint16_t)0x01B5U)
#define SHOSTIF_BOOT_AREA_END_ADDR     ((uint16_t)0x01B8U)
#endif

#if defined(BSP_MCU_GROUP_RZN2L)
/* PHOSTIF boot mode area */
#define PHOSTIF_BOOT_AREA_START_ADDR   ((uint16_t)0x01B9U)
#define PHOSTIF_BOOT_AREA_END_ADDR     ((uint16_t)0x01BCU)
#endif

/* OTP address for JTAG authentication */
#define JTAG_AUTH_MODE_ADDR            ((uint16_t)0x0091U)
#define JTAG_AUTH_TYPE_ADDR            ((uint16_t)0x0092U)
#define JTAG_AUTH_ID1_PLAIN_ADDR       ((uint16_t)0x0093U)
#define JTAG_AUTH_ID4_PLAIN_ADDR       ((uint16_t)0x009BU)
#define JTAG_AUTH_ID1_HASH_ADDR        ((uint16_t)0x016DU)
#define JTAG_AUTH_ID4_HASH_ADDR        ((uint16_t)0x017DU)

/* OTP address for SCI/USB boot */
#define SCI_USB_BOOT_ADDR              ((uint16_t)0x018DU)

/* OTP address for Unique ID */
#define UID_ADDR                       ((uint16_t)0x0089U)

/* OTP error code */
typedef enum e_otp_err
{
    OTP_SUCCESS     = 0,
    OTP_ERROR       = 1,
} otp_err_t;

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/
otp_err_t otp_power_on(void);
void otp_power_off(void);
otp_err_t write_otp_data(uint16_t otp_addr, uint16_t data);
otp_err_t read_otp_data(uint16_t otp_addr, uint16_t *p_data);
otp_err_t write_otp_multiple_data(uint16_t addr, uint8_t * const p_data, uint8_t data_len);
otp_err_t read_otp_multiple_data(uint16_t addr, uint8_t *p_data, uint8_t data_len);

#endif /* __OTP_H__ */
