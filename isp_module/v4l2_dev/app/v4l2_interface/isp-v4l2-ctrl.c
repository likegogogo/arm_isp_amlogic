/*
*
* SPDX-License-Identifier: GPL-2.0
*
* Copyright (C) 2011-2018 ARM or its affiliates
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2.
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>

#include "acamera_logger.h"

#include "isp-v4l2-common.h"
#include "isp-v4l2-ctrl.h"
#include "fw-interface.h"

static int isp_v4l2_ctrl_check_valid( struct v4l2_ctrl *ctrl )
{
    if ( ctrl->is_int == 1 ) {
        if ( ctrl->val < ctrl->minimum || ctrl->val > ctrl->maximum )
            return -EINVAL;
    }

    return 0;
}

static int isp_v4l2_ctrl_s_ctrl_standard( struct v4l2_ctrl *ctrl )
{
    int ret = 0;
    LOG( LOG_ERR, "Control - id:0x%x, val:%d, is_int:%d, min:%d, max:%d.\n",
         ctrl->id, ctrl->val, ctrl->is_int, ctrl->minimum, ctrl->maximum );

    if ( isp_v4l2_ctrl_check_valid( ctrl ) < 0 ) {
        return -EINVAL;
    }

    switch ( ctrl->id ) {
    case V4L2_CID_BRIGHTNESS:
        ret = fw_intf_set_brightness( ctrl->val );
        break;
    case V4L2_CID_CONTRAST:
        ret = fw_intf_set_contrast( ctrl->val );
        break;
    case V4L2_CID_SATURATION:
        ret = fw_intf_set_saturation( ctrl->val );
        break;
    case V4L2_CID_HUE:
        ret = fw_intf_set_hue( ctrl->val );
        break;
    case V4L2_CID_SHARPNESS:
        ret = fw_intf_set_sharpness( ctrl->val );
        break;
    case V4L2_CID_COLORFX:
        ret = fw_intf_set_color_fx( ctrl->val );
        break;
    case V4L2_CID_HFLIP:
        ret = fw_intf_set_hflip( ctrl->val );
        break;
    case V4L2_CID_VFLIP:
        ret = fw_intf_set_vflip( ctrl->val );
        break;
    case V4L2_CID_AUTOGAIN:
        ret = fw_intf_set_autogain( ctrl->val );
        break;
    case V4L2_CID_GAIN:
        ret = fw_intf_set_gain( ctrl->val );
        break;
    case V4L2_CID_EXPOSURE_AUTO:
        ret = fw_intf_set_exposure_auto( ctrl->val );
        break;
    case V4L2_CID_EXPOSURE_ABSOLUTE:
        ret = fw_intf_set_exposure( ctrl->val );
        break;
    case V4L2_CID_EXPOSURE_AUTO_PRIORITY:
        ret = fw_intf_set_variable_frame_rate( ctrl->val );
        break;
    case V4L2_CID_AUTO_WHITE_BALANCE:
        ret = fw_intf_set_white_balance_auto( ctrl->val );
        break;
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
        ret = fw_intf_set_white_balance( ctrl->val );
        break;
    case V4L2_CID_FOCUS_AUTO:
        ret = fw_intf_set_focus_auto( ctrl->val );
        break;
    case V4L2_CID_FOCUS_ABSOLUTE:
        ret = fw_intf_set_focus( ctrl->val );
        break;
    }

    return ret;
}

static int isp_v4l2_ctrl_s_ctrl_custom( struct v4l2_ctrl *ctrl )
{
    int ret = 0;

    LOG( LOG_INFO, "Control - id:0x%x, val:%d, is_int:%d, min:%d, max:%d.\n",
         ctrl->id, ctrl->val, ctrl->is_int, ctrl->minimum, ctrl->maximum );

    if ( isp_v4l2_ctrl_check_valid( ctrl ) < 0 ) {
        LOG( LOG_ERR, "Invalid param: id:0x%x, val:0x%x, is_int:%d, min:0x%x, max:0x%x.\n",
             ctrl->id, ctrl->val, ctrl->is_int, ctrl->minimum, ctrl->maximum );

        return -EINVAL;
    }

    switch ( ctrl->id ) {
    case ISP_V4L2_CID_TEST_PATTERN:
        LOG( LOG_INFO, "new test_pattern: %d.\n", ctrl->val );
        ret = fw_intf_set_test_pattern( ctrl->val );
        break;
    case ISP_V4L2_CID_TEST_PATTERN_TYPE:
        LOG( LOG_INFO, "new test_pattern_type: %d.\n", ctrl->val );
        ret = fw_intf_set_test_pattern_type( ctrl->val );
        break;
    case ISP_V4L2_CID_AF_REFOCUS:
        LOG( LOG_INFO, "new focus: %d.\n", ctrl->val );
        ret = fw_intf_set_af_refocus( ctrl->val );
        break;
    case ISP_V4L2_CID_SENSOR_PRESET:
        LOG( LOG_INFO, "new sensor preset: %d.\n", ctrl->val );
        ret = fw_intf_isp_set_sensor_preset( ctrl->val );
        break;
    case ISP_V4L2_CID_AF_ROI:
        // map [0,127] to [0, 254] due to limitaton of V4L2_CTRL_TYPE_INTEGER.
        LOG( LOG_INFO, "new af roi: 0x%x.\n", ctrl->val * 2 );
        ret = fw_intf_set_af_roi( ctrl->val * 2 );
        break;
    case ISP_V4L2_CID_OUTPUT_FR_ON_OFF:
        LOG( LOG_INFO, "output FR on/off: 0x%x.\n", ctrl->val );
        ret = fw_intf_set_output_fr_on_off( ctrl->val );
        break;
    case ISP_V4L2_CID_OUTPUT_DS1_ON_OFF:
        LOG( LOG_INFO, "output DS1 on/off: 0x%x.\n", ctrl->val );
        ret = fw_intf_set_output_ds1_on_off( ctrl->val );
        break;
    }

    return ret;
}

static const struct v4l2_ctrl_ops isp_v4l2_ctrl_ops_custom = {
    .s_ctrl = isp_v4l2_ctrl_s_ctrl_custom,
};

static const struct v4l2_ctrl_config isp_v4l2_ctrl_test_pattern = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_TEST_PATTERN,
    .name = "ISP Test Pattern",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 1,
    .step = 1,
    .def = 0,
};

static const struct v4l2_ctrl_config isp_v4l2_ctrl_test_pattern_type = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_TEST_PATTERN_TYPE,
    .name = "ISP Test Pattern Type",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 3,
    .step = 1,
    .def = 3,
};

static const struct v4l2_ctrl_config isp_v4l2_ctrl_af_refocus = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_AF_REFOCUS,
    .name = "ISP AF Refocus",
    .type = V4L2_CTRL_TYPE_BUTTON,
    .min = 0,
    .max = 0,
    .step = 0,
    .def = 0,
};

static const struct v4l2_ctrl_config isp_v4l2_ctrl_sensor_preset = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_SENSOR_PRESET,
    .name = "ISP Sensor Preset",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 5,
    .step = 1,
    .def = 0,
};


static const struct v4l2_ctrl_config isp_v4l2_ctrl_af_roi = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_AF_ROI,
    .name = "ISP AF ROI",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 0x7F7F7F7F,
    .step = 1,
    .def = 0x20206060,
};


static const struct v4l2_ctrl_config isp_v4l2_ctrl_output_fr_on_off = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_OUTPUT_FR_ON_OFF,
    .name = "ISP FR ON/OFF",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 0x7FFFFFFF,
    .step = 1,
    .def = 0,
};


static const struct v4l2_ctrl_config isp_v4l2_ctrl_output_ds1_on_off = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .id = ISP_V4L2_CID_OUTPUT_DS1_ON_OFF,
    .name = "ISP DS1 ON/OFF",
    .type = V4L2_CTRL_TYPE_INTEGER,
    .min = 0,
    .max = 0x7FFFFFFF,
    .step = 1,
    .def = 0,
};

static const struct v4l2_ctrl_ops isp_v4l2_ctrl_ops = {
    .s_ctrl = isp_v4l2_ctrl_s_ctrl_standard,
};

static const struct v4l2_ctrl_config isp_v4l2_ctrl_class = {
    .ops = &isp_v4l2_ctrl_ops_custom,
    .flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
    .id = ISP_V4L2_CID_ISP_V4L2_CLASS,
    .name = "ARM ISP Controls",
    .type = V4L2_CTRL_TYPE_CTRL_CLASS,
};

#define ADD_CTRL_STD( id, min, max, step, def )                  \
    {                                                            \
        if ( fw_intf_validate_control( id ) ) {                  \
            v4l2_ctrl_new_std( hdl_std_ctrl, &isp_v4l2_ctrl_ops, \
                               id, min, max, step, def );        \
        }                                                        \
    }

#define ADD_CTRL_STD_MENU( id, max, skipmask, def )                   \
    {                                                                 \
        if ( fw_intf_validate_control( id ) ) {                       \
            v4l2_ctrl_new_std_menu( hdl_std_ctrl, &isp_v4l2_ctrl_ops, \
                                    id, max, skipmask, def );         \
        }                                                             \
    }

#define ADD_CTRL_CST( id, cfg, priv )                        \
    {                                                        \
        if ( fw_intf_validate_control( id ) ) {              \
            v4l2_ctrl_new_custom( hdl_cst_ctrl, cfg, priv ); \
        }                                                    \
    }

int isp_v4l2_ctrl_init( isp_v4l2_ctrl_t *ctrl )
{
    struct v4l2_ctrl_handler *hdl_std_ctrl = &ctrl->ctrl_hdl_std_ctrl;
    struct v4l2_ctrl_handler *hdl_cst_ctrl = &ctrl->ctrl_hdl_cst_ctrl;

    /* Init and add standard controls */
    v4l2_ctrl_handler_init( hdl_std_ctrl, 10 );
    v4l2_ctrl_new_custom( hdl_std_ctrl, &isp_v4l2_ctrl_class, NULL );

    /* general */
    ADD_CTRL_STD( V4L2_CID_BRIGHTNESS, 0, 255, 1, 128 );
    ADD_CTRL_STD( V4L2_CID_CONTRAST, 0, 255, 1, 128 );
    ADD_CTRL_STD( V4L2_CID_SATURATION, 0, 255, 1, 128 );
    ADD_CTRL_STD( V4L2_CID_HUE, 0, 255, 1, 128 );
    ADD_CTRL_STD( V4L2_CID_SHARPNESS, 0, 255, 1, 128 );
    ADD_CTRL_STD_MENU( V4L2_CID_COLORFX, 4, 0x1F0, 0 );
    /* orientation */
    ADD_CTRL_STD( V4L2_CID_HFLIP, 0, 1, 1, 0 );
    ADD_CTRL_STD( V4L2_CID_VFLIP, 0, 1, 1, 0 );
    /* exposure */
    ADD_CTRL_STD( V4L2_CID_AUTOGAIN, 0, 1, 1, 1 );
    ADD_CTRL_STD( V4L2_CID_GAIN, 100, 3200, 1, 100 );
    ADD_CTRL_STD_MENU( V4L2_CID_EXPOSURE_AUTO,
                       1, 0x0, 0 );
    ADD_CTRL_STD( V4L2_CID_EXPOSURE_ABSOLUTE,
                  1, 1000, 1, 33 );
    ADD_CTRL_STD( V4L2_CID_EXPOSURE_AUTO_PRIORITY,
                  0, 1, 1, 0 );
    /* white balance */
    ADD_CTRL_STD( V4L2_CID_AUTO_WHITE_BALANCE,
                  0, 1, 1, 1 );
    ADD_CTRL_STD( V4L2_CID_WHITE_BALANCE_TEMPERATURE,
                  2000, 8000, 1000, 5000 );
    /* focus */
    ADD_CTRL_STD( V4L2_CID_FOCUS_AUTO, 0, 1, 1, 1 );
    ADD_CTRL_STD( V4L2_CID_FOCUS_ABSOLUTE,
                  0, 255, 1, 0 );

    /* Init and add custom controls */
    v4l2_ctrl_handler_init( hdl_cst_ctrl, 2 );
    v4l2_ctrl_new_custom( hdl_cst_ctrl, &isp_v4l2_ctrl_class, NULL );

    ADD_CTRL_CST( ISP_V4L2_CID_TEST_PATTERN,
                  &isp_v4l2_ctrl_test_pattern, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_TEST_PATTERN_TYPE,
                  &isp_v4l2_ctrl_test_pattern_type, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_AF_REFOCUS,
                  &isp_v4l2_ctrl_af_refocus, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_SENSOR_PRESET,
                  &isp_v4l2_ctrl_sensor_preset, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_AF_ROI,
                  &isp_v4l2_ctrl_af_roi, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_OUTPUT_FR_ON_OFF,
                  &isp_v4l2_ctrl_output_fr_on_off, NULL );
    ADD_CTRL_CST( ISP_V4L2_CID_OUTPUT_DS1_ON_OFF,
                  &isp_v4l2_ctrl_output_ds1_on_off, NULL );

    /* Add control handler to v4l2 device */
    v4l2_ctrl_add_handler( hdl_std_ctrl, hdl_cst_ctrl, NULL );
    ctrl->video_dev->ctrl_handler = hdl_std_ctrl;

    v4l2_ctrl_handler_setup( hdl_std_ctrl );

    return 0;
}

void isp_v4l2_ctrl_deinit( isp_v4l2_ctrl_t *ctrl )
{
    v4l2_ctrl_handler_free( &ctrl->ctrl_hdl_std_ctrl );
    v4l2_ctrl_handler_free( &ctrl->ctrl_hdl_cst_ctrl );
}
