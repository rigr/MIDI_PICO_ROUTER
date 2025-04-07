#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_HOST               1
#define CFG_TUH_ENABLED             1
#define CFG_TUH_MAX_SPEED           1
#define CFG_TUH_MIDI                1
#define CFG_TUSB_HOST_DEVICE_MAX    8

#define CFG_TUSB_DEVICE             1
#define CFG_TUD_ENABLED             1
#define CFG_TUD_MIDI                1

#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_HOST | OPT_MODE_DEVICE)
#define CFG_TUSB_OS                 OPT_OS_NONE
#define CFG_TUSB_DEBUG              1

#define CFG_TUH_MIDI_RX_BUFSIZE     64
#define CFG_TUH_MIDI_TX_BUFSIZE     64
#define CFG_TUD_MIDI_RX_BUFSIZE     64
#define CFG_TUD_MIDI_TX_BUFSIZE     64

#endif /* _TUSB_CONFIG_H_ */
