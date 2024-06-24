/***********************************************************************************************************************
 * Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

#include "hal_data.h"
#include "cmd_otp.h"
#include "cmd_otp_auth.h"
#include "common.h"
#include "device_setup.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event) BSP_PLACE_IN_SECTION(".warm_start");

extern bsp_leds_t g_bsp_leds;

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
/* Buffer address of send / received packets  */
#define PACKET_BUFFER_ADDR      ((uint32_t)0x30000000UL)
/* SCI setting value  */
#define SCI_UART_BAUDRATE       (115200U)
#define SCI_BUND_RATE_ERR       (5000U)

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/
static volatile uint32_t s_g_sci_send_packet_complete     = 0U;  // Send packet completion flag 
static volatile uint32_t s_g_sci_receive_packet_complete  = 0U;  // Receive packet completion flag 
static volatile uint32_t s_g_usb_receive_packet_complete  = 0U;  // Receive packet completion flag 

static void sci_uart_set_baud(void);
static void handle_module_error(fsp_err_t fsp_err);

uint8_t debug_control = 0;
uint16_t debug_otp_addr, debug_otp_data;
uint8_t jauth_mode, jauth_type, uuid[16];
uint8_t jauth_id[16]={0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

/*
Step to set Jtag authentication password:
1. set debug_control = 3, set mode = 1 type = 0, to add a password to Jtag
2.  set debug_control = 5, and run   else if(debug_control == 5) to write the authentication password to jauth_id.
And you can write mode = 8 to Permanent prohibition of JTAG connection(Please take care of this usage, if set, it will never recover)
*/

/*******************************************************************************************************************//**
 * @brief  Jlink authentication password setting  example application
 *
 * 
 *
 **********************************************************************************************************************/
void hal_entry (void)
{
    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
    /* Set the blink frequency (must be <= bsp_delay_units */
    const uint32_t freq_in_hz = 2;
    uint8_t   return_code     = 0U;
    /* Calculate the delay in terms of bsp_delay_units */
    const uint32_t delay = bsp_delay_units / freq_in_hz;
    fsp_err_t fsp_err;
    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;
    /* Turn off LEDs */
    for (uint32_t i = 0; i < leds.led_count; i++)
    {
        R_BSP_PinClear(BSP_IO_REGION_SAFE, (bsp_io_port_pin_t) leds.p_leds[i]);
    }
    /* Initializes the module. */
    fsp_err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    handle_module_error(fsp_err);
    sci_uart_set_baud();
    /* Enable interrupt. */
    __asm volatile ("cpsie i");
    
    while (1)
    {
        /* Toggle board LEDs */
        for (uint32_t i = 0; i < leds.led_count; i++)
        {
            R_BSP_PinToggle(BSP_IO_REGION_SAFE, (bsp_io_port_pin_t) leds.p_leds[i]);
        }
        /* Delay */
        R_BSP_SoftwareDelay(delay, bsp_delay_units);
        /* Check if SCI reception is complete. */
        if (1U == s_g_sci_receive_packet_complete)
        {
            /* Execute command. add your own code here*/
        }
        if(debug_control == 1){
          debug_control = 0;
          return_code = cmd_write_otp(debug_otp_addr, debug_otp_data);   
        }
        else if(debug_control == 2){
          debug_control = 0;
          return_code = cmd_read_otp(debug_otp_addr, &debug_otp_data);
        }    
        else if(debug_control == 3){
          debug_control = 0;
          return_code = cmd_set_jtag_auth(jauth_mode, jauth_type);//set mode = 1 type = 0, to add a password to Jtag      
        }
        else if(debug_control == 4){
          debug_control = 0;
          return_code = cmd_get_jtag_auth(&jauth_mode, &jauth_type);   
        }
        else if(debug_control == 5){
          debug_control = 0;
          return_code = cmd_set_jtag_auth_id(jauth_mode, jauth_type, jauth_id);
          //set id = 0x5555555555555555aaaaaaaaaaaaaaaa
        }
        else if(debug_control == 6){
          debug_control = 0;
          return_code = cmd_get_unique_id(uuid);//get uuid  
        }
        else;
        
        if(return_code == 0)
          ;  
    }
}

/******************************************************************************
 * @brief Module error handler.
 *
 * @param[in]  fsp_err        FSP module return code.
 ******************************************************************************/
static void handle_module_error (fsp_err_t fsp_err)
{
    /* If an error occurs in each module, the setup process is stopped. */
    if (FSP_SUCCESS != fsp_err)
    {
        while (1);
    }
}


/******************************************************************************
 * @brief Set the band for SCI communication.
 ******************************************************************************/
static void sci_uart_set_baud (void)
{
    baud_setting_t baud_setting;
    uint32_t       baud_rate                 = SCI_UART_BAUDRATE;
    bool           enable_bitrate_modulation = false;
    uint32_t       error_rate_x_1000         = SCI_BUND_RATE_ERR;
    fsp_err_t      fsp_err;
    
    fsp_err = R_SCI_UART_BaudCalculate(baud_rate, enable_bitrate_modulation, error_rate_x_1000, &baud_setting);
    handle_module_error(fsp_err);
    fsp_err = R_SCI_UART_BaudSet(&g_uart0_ctrl, (void *)&baud_setting);
    handle_module_error(fsp_err);
}

/******************************************************************************
 * @brief SCI UART module callback function.
 *
 * @param[in]  p_args         Callback information.
 ******************************************************************************/
void sci_uart_callback (uart_callback_args_t *p_args)
{    
    /* Handle the UART event. */
    switch (p_args->event)
    {
        /* Receive complete. */
        case UART_EVENT_RX_COMPLETE:  
            break;      
        /* Transmit complete. */
        case UART_EVENT_TX_COMPLETE:
            break;
        default:
            break;
    }
}
/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
        /* Pre clock initialization */
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

