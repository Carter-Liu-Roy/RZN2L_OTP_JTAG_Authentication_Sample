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
#ifndef __DEVICE_SETUP_H__
#define __DEVICE_SETUP_H__

/******************************************************************************
 * Typedef definitions
 ******************************************************************************/
/* Packet format, WRITE_FLASH Command */
typedef struct
{
    uint8_t    address[4];
    uint8_t    data[0];
} cmd_write_flash_t;

/* Packet format, WRITE_OTP Command */
typedef struct
{
    uint8_t    address[2];
    uint8_t    data[2];
} cmd_write_otp_t;

/* Packet format, READ_OTP Command */
typedef struct
{
    uint8_t    address[2];
} cmd_read_otp_t;

/* Packet format, SET_JAUTH Command */
typedef struct
{
    uint8_t    mode;
    uint8_t    type;
} cmd_set_jauth_t;

/* Packet format, SET_JAUTHID Command */
typedef struct
{
    uint8_t    mode;
    uint8_t    type;
    uint8_t    id[0];
} cmd_set_jauthid_t;

/* Packet format, SET_SCIUSB Command */
typedef struct
{
    uint8_t    mode;
} cmd_set_sciusb_t;

/* Packet format, Command header */
typedef struct
{
    uint8_t    type;
    uint8_t    code;
    uint8_t    payload_size[4];
} head_t;

/* Packet format */
typedef struct
{
    head_t head;
    union {
        cmd_write_flash_t    wflash;
        cmd_write_otp_t      wotp;
        cmd_read_otp_t       rotp;
        cmd_set_jauth_t      jauth;
        cmd_set_jauthid_t    jauthid;
        cmd_set_sciusb_t     sciusb;
    } cmd;
} packet_t;

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/
void device_setup(void);

#endif /* __DEVICE_SETUP_H__ */
