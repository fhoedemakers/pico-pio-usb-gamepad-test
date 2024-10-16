/*
 * author : Shuichi TAKANO
 * since  : Thu Jul 29 2021 03:39:11
 */

#include <tusb.h>
#include <stdio.h>
#include "gamepad.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_REPORT 4

    namespace
    {
        uint8_t previousbuffer[64];
        bool firstReport = true;
        uint8_t _report_count[CFG_TUH_HID];
        tuh_hid_report_info_t _report_info_arr[CFG_TUH_HID][MAX_REPORT];

        bool isDS4(uint16_t vid, uint16_t pid)
        {
            return vid == 0x054c && (pid == 0x09cc || pid == 0x05c4);
        }

        bool isDS5(uint16_t vid, uint16_t pid)
        {
            return vid == 0x054c && pid == 0x0ce6;
        }
        bool isPSClassic(uint16_t vid, uint16_t pid)
        {
            return vid == 0x054c && pid == 0x0cda;
        }

        bool isGenesisMini(uint16_t vid, uint16_t pid)
        {
            return vid == 0x0ca3 && (pid == 0x0025 || pid == 0x0024);
        }

        bool isMantaPad(uint16_t vid, uint16_t pid)
        {
            return vid == 0x081f && pid == 0xe401;
        }

        struct DS4Report
        {
            // https://www.psdevwiki.com/ps4/DS4-USB

            struct Button1
            {
                inline static constexpr int SQUARE = 1 << 4;
                inline static constexpr int CROSS = 1 << 5;
                inline static constexpr int CIRCLE = 1 << 6;
                inline static constexpr int TRIANGLE = 1 << 7;
            };

            struct Button2
            {
                inline static constexpr int L1 = 1 << 0;
                inline static constexpr int R1 = 1 << 1;
                inline static constexpr int L2 = 1 << 2;
                inline static constexpr int R2 = 1 << 3;
                inline static constexpr int SHARE = 1 << 4;
                inline static constexpr int OPTIONS = 1 << 5;
                inline static constexpr int L3 = 1 << 6;
                inline static constexpr int R3 = 1 << 7;
            };

            uint8_t reportID;
            uint8_t stickL[2];
            uint8_t stickR[2];
            uint8_t buttons1;
            uint8_t buttons2;
            uint8_t ps : 1;
            uint8_t tpad : 1;
            uint8_t counter : 6;
            uint8_t triggerL;
            uint8_t triggerR;
            // ...

            int getHat() const { return buttons1 & 15; }
        };

        struct DS5Report
        {
            uint8_t reportID;
            uint8_t stickL[2];
            uint8_t stickR[2];
            uint8_t triggerL;
            uint8_t triggerR;
            uint8_t counter;
            uint8_t buttons[3];
            // ...

            struct Button
            {
                inline static constexpr int SQUARE = 1 << 4;
                inline static constexpr int CROSS = 1 << 5;
                inline static constexpr int CIRCLE = 1 << 6;
                inline static constexpr int TRIANGLE = 1 << 7;
                inline static constexpr int L1 = 1 << 8;
                inline static constexpr int R1 = 1 << 9;
                inline static constexpr int L2 = 1 << 10;
                inline static constexpr int R2 = 1 << 11;
                inline static constexpr int SHARE = 1 << 12;
                inline static constexpr int OPTIONS = 1 << 13;
                inline static constexpr int L3 = 1 << 14;
                inline static constexpr int R3 = 1 << 15;
                inline static constexpr int PS = 1 << 16;
                inline static constexpr int TPAD = 1 << 17;
            };

            int getHat() const { return buttons[0] & 15; }
        };

        // Report for Genesis Mini controller
        struct GenesisMiniReport
        {
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
            uint8_t byte4;
            uint8_t byte5;
            uint8_t byte6;
            uint8_t byte7;
            uint8_t byte8;
            struct Button
            {

                inline static constexpr int A = 0b01000000;
                inline static constexpr int B = 0b00100000;
                inline static constexpr int C = 0b00000010;
                inline static constexpr int START = 0b00100000;
                inline static constexpr int UP = 0;
                inline static constexpr int DOWN = 0b11111111;
                inline static constexpr int LEFT = 0;
                inline static constexpr int RIGHT = 0b11111111;
                ;
            };
        };
        struct PSClassicReport
        {
            uint8_t buttons;
            // Idle      00010100
            // up        00000100
            // upright   00001000
            // right     00011000
            // rightdown 00101000
            // down      00100100
            // downleft  00100000
            // left      00010000
            // leftup    00100000
            // start     00010110
            // select    00010101
            // St + sel  00010111
            // selectup  00000101
            // selectdown 00100101
            uint8_t hat;
            struct Button
            {
                inline static constexpr int ButtonsIdle = 0x00;
                inline static constexpr int HatIdle = 0b00010100;
                inline static constexpr int Circle = 0x02;
                inline static constexpr int Cross = 0x04;
                inline static constexpr int SELECT = 0b00010101;
                inline static constexpr int START = 0b00010110;
                inline static constexpr int UP = 0b00000100;
                inline static constexpr int UPRIGHT = 0b00001000;
                inline static constexpr int RIGHT = 0b00011000;
                inline static constexpr int RIGHTDOWN = 0b00101000;
                inline static constexpr int DOWN = 0b00100100;
                inline static constexpr int DOWNLEFT = 0b00100000;
                inline static constexpr int LEFT = 0b00010000;
                inline static constexpr int LEFTUP = 0b00000000;
                inline static constexpr int SELECTUP = 0b00000101;
                inline static constexpr int SELECTDOWN = 0b00100101;
                inline static constexpr int SELECTSTART = 0b00010111;
            };
            int getHat() const { return hat; }
        };
        // Report for MantaPad, cheap AliExpress SNES controller
        struct MantaPadReport
        {
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
            uint8_t byte4;
            uint8_t byte5;
            uint8_t byte6;
            uint8_t byte7;
            uint8_t byte8;

            struct Button
            {
                inline static constexpr int A = 0b00100000;
                inline static constexpr int B = 0b01000000;
                inline static constexpr int X = 0b00010000;
                inline static constexpr int Y = 0b10000000;
                inline static constexpr int SELECT = 0b00010000;
                inline static constexpr int START = 0b00100000;
                inline static constexpr int UP = 0b00000000;
                inline static constexpr int DOWN = 0b11111111;
                inline static constexpr int LEFT = 0b00000000;
                inline static constexpr int RIGHT = 0b11111111;
                inline static constexpr int SHOULDERLEFT = 0b00000001;
                inline static constexpr int SHOULDERRIGHT = 0b00000010;
            };
        };
    }
    static uint8_t const keycode2ascii[128][2] = {HID_KEYCODE_TO_ASCII};
    // look up new key in previous keys
    static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
    {
        for (uint8_t i = 0; i < 6; i++)
        {
            if (report->keycode[i] == keycode)
                return true;
        }

        return false;
    }

    static void process_kbd_report(hid_keyboard_report_t const *report)
    {
        static hid_keyboard_report_t prev_report = {0, 0, {0}}; // previous report to check key released

        //------------- example code ignore control (non-printable) key affects -------------//
        for (uint8_t i = 0; i < 6; i++)
        {
            // A = 4, S = 22, Z=29, X=27, UP=82, DOWN=81, LEFT=80, RIGHT=79
            if (report->keycode[i])
            {
                if (find_key_in_report(&prev_report, report->keycode[i]))
                {
                    // exist in previous report means the current key is holding
                }
                else
                {
                    // not existed in previous report means the current key is pressed
                    bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
                    uint8_t ch = keycode2ascii[report->keycode[i]][is_shift ? 1 : 0];
                    printf("Key code pressed: %d Ascii - ", report->keycode[i]);
                    if (ch == '\r')
                    {
                        printf("\r"); // added new line for enter key
                    }
                    else
                    {
                        putchar(ch);
                    }

                    // #ifndef __ICCARM__                  // TODO IAR doesn't support stream control ?
                    //                     fflush(stdout); // flush right away, else nanolib will wait for newline
                    // #endif
                    putchar('\n');
                }
            }
            // TODO example skips key released
        }

        prev_report = *report;
    }
    void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len)
    {
        uint16_t vid, pid;
        tuh_vid_pid_get(dev_addr, &vid, &pid);

        printf("HID device address = %d, instance = %d is mounted\n", dev_addr, instance);
        printf("VID = %04x, PID = %04x\r\n", vid, pid);

        const char *protocol_str[] = {"None", "Keyboard", "Mouse"}; // hid_protocol_type_t
        uint8_t const interface_protocol = tuh_hid_interface_protocol(dev_addr, instance);

        // Parse report descriptor with built-in parser
        _report_count[instance] = tuh_hid_parse_report_descriptor(_report_info_arr[instance], MAX_REPORT, desc_report, desc_len);
        printf("HID has %u reports and interface protocol = %d:%s\n", _report_count[instance],
               interface_protocol, protocol_str[interface_protocol]);

        if (!tuh_hid_receive_report(dev_addr, instance))
        {
            printf("Error: cannot request to receive report\r\n");
        }
    }

    void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
    {
        printf("HID device address = %d, instance = %d is unmounted\n", dev_addr, instance);
    }

    void tuh_hid_report_received_cb(uint8_t dev_addr,
                                    uint8_t instance, uint8_t const *report, uint16_t len)
    {
        uint8_t const rpt_count = _report_count[instance];
        tuh_hid_report_info_t *rpt_info_arr = _report_info_arr[instance];
        tuh_hid_report_info_t *rpt_info = NULL;

        uint16_t vid, pid;
        tuh_vid_pid_get(dev_addr, &vid, &pid);

        if (isDS4(vid, pid))
        {
            if (sizeof(DS4Report) <= len)
            {
                auto r = reinterpret_cast<const DS4Report *>(report);
                if (r->reportID != 1)
                {
                    printf("Invalid reportID %d\n", r->reportID);
                    return;
                }

                auto &gp = io::getCurrentGamePadState(0);
                gp.axis[0] = r->stickL[0];
                gp.axis[1] = r->stickL[1];
                gp.buttons =
                    (r->buttons1 & DS4Report::Button1::CROSS ? io::GamePadState::Button::B : 0) |
                    (r->buttons1 & DS4Report::Button1::CIRCLE ? io::GamePadState::Button::A : 0) |
                    (r->buttons2 & DS4Report::Button2::SHARE ? io::GamePadState::Button::SELECT : 0) |
                    (r->tpad ? io::GamePadState::Button::SELECT : 0) |
                    (r->buttons2 & DS4Report::Button2::OPTIONS ? io::GamePadState::Button::START : 0);
                gp.hat = static_cast<io::GamePadState::Hat>(r->getHat());
                gp.convertButtonsFromAxis(0, 1);
                gp.convertButtonsFromHat();
            }
            else
            {
                printf("Invalid DS4 report size %zd\n", len);
                return;
            }
        }
        else if (isDS5(vid, pid))
        {
            if (sizeof(DS5Report) <= len)
            {

                auto r = reinterpret_cast<const DS5Report *>(report);
                if (r->reportID != 1)
                {
                    printf("Invalid reportID %d\n", r->reportID);
                    return;
                }

                auto buttons = r->buttons[0] | (r->buttons[1] << 8) | (r->buttons[2] << 16);

                auto &gp = io::getCurrentGamePadState(0);
                gp.axis[0] = r->stickL[0];
                gp.axis[1] = r->stickL[1];
                gp.buttons =
                    (buttons & DS5Report::Button::CROSS ? io::GamePadState::Button::B : 0) |
                    (buttons & DS5Report::Button::CIRCLE ? io::GamePadState::Button::A : 0) |
                    (buttons & (DS5Report::Button::SHARE | DS5Report::Button::TPAD) ? io::GamePadState::Button::SELECT : 0) |
                    (buttons & DS5Report::Button::OPTIONS ? io::GamePadState::Button::START : 0);
                gp.hat = static_cast<io::GamePadState::Hat>(r->getHat());
                gp.convertButtonsFromAxis(0, 1);
                gp.convertButtonsFromHat();
            }
            else
            {
                printf("Invalid DS5 report size %zd\n", len);
                return;
            }
        }
        else if (isMantaPad(vid, pid))
        {
            if (sizeof(MantaPadReport) == len)
            {
                auto r = reinterpret_cast<const MantaPadReport *>(report);
                auto &gp = io::getCurrentGamePadState(0);
                gp.buttons =
                    (r->byte6 & MantaPadReport::Button::A ? io::GamePadState::Button::A : 0) |
                    (r->byte6 & MantaPadReport::Button::B ? io::GamePadState::Button::B : 0) |
                    (r->byte7 & MantaPadReport::Button::START ? io::GamePadState::Button::START : 0) |
                    (r->byte7 & MantaPadReport::Button::SELECT ? io::GamePadState::Button::SELECT : 0) |
                    (r->byte2 == MantaPadReport::Button::UP ? io::GamePadState::Button::UP : 0) |
                    (r->byte2 == MantaPadReport::Button::DOWN ? io::GamePadState::Button::DOWN : 0) |
                    (r->byte1 == MantaPadReport::Button::LEFT ? io::GamePadState::Button::LEFT : 0) |
                    (r->byte1 == MantaPadReport::Button::RIGHT ? io::GamePadState::Button::RIGHT : 0);

                if (memcmp(previousbuffer, report, len) != 0 || firstReport)
                {
                    firstReport = false;
                    printf("MantaPad    : len = %d  - ", len);
                    // print in binary len report bytes
                    for (int i = 0; i < len; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            printf("%d", (report[i] >> (7 - j)) & 1);
                        }
                        printf(" ");
                    }

                    printf("\n");
                    // print 8 bytes of report in hex
                    printf("                        ");
                    for (int i = 0; i < len; i++)
                    {
                        printf("      %02x ", report[i]);
                    }
                    printf("\n");
                    memcpy(previousbuffer, report, len);
                }
            }
            else
            {
                printf("Invalid MantaPad report size %zd\n", len);
                return;
            }
        }
        else if (isGenesisMini(vid, pid))
        {
            if (sizeof(GenesisMiniReport) == len)
            {
                auto r = reinterpret_cast<const GenesisMiniReport *>(report);
                auto &gp = io::getCurrentGamePadState(0);
                gp.buttons =
                    (r->byte6 & GenesisMiniReport::Button::B ? io::GamePadState::Button::B : 0) |
                    (r->byte6 & GenesisMiniReport::Button::A ? io::GamePadState::Button::A : 0) |
                    (r->byte7 & GenesisMiniReport::Button::C ? io::GamePadState::Button::SELECT : 0) |
                    (r->byte7 & GenesisMiniReport::Button::START ? io::GamePadState::Button::START : 0) |
                    (r->byte5 == GenesisMiniReport::Button::UP ? io::GamePadState::Button::UP : 0) |
                    (r->byte5 == GenesisMiniReport::Button::DOWN ? io::GamePadState::Button::DOWN : 0) |
                    (r->byte4 == GenesisMiniReport::Button::LEFT ? io::GamePadState::Button::LEFT : 0) |
                    (r->byte4 == GenesisMiniReport::Button::RIGHT ? io::GamePadState::Button::RIGHT : 0);

                if (memcmp(previousbuffer, report, len) != 0 || firstReport)
                {
                    firstReport = false;
                    printf("Genesis Mini: len = %d - ", len);
                    // print in binary len report bytes
                    for (int i = 0; i < len; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            printf("%d", (report[i] >> (7 - j)) & 1);
                        }
                        printf(" ");
                    }

                    printf("\n");
                    // print 8 bytes of report in hex
                    printf("                        ");
                    for (int i = 0; i < len; i++)
                    {
                        printf("      %02x ", report[i]);
                    }
                    printf("\n");
                    memcpy(previousbuffer, report, len);
                }
            }
            else
            {
                printf("Invalid Genesis Mini report size %zd\n", len);
                return;
            }
        }
        else if (isPSClassic(vid, pid))
        {
            if (sizeof(PSClassicReport) == len)
            {
                auto r = reinterpret_cast<const PSClassicReport *>(report);
                auto &gp = io::getCurrentGamePadState(0);
                gp.buttons =
                    (r->buttons & PSClassicReport::Button::Cross ? io::GamePadState::Button::B : 0) |
                    (r->buttons & PSClassicReport::Button::Circle ? io::GamePadState::Button::A : 0);

                switch (r->hat)
                {
                case PSClassicReport::Button::UP:
                    gp.buttons = gp.buttons | io::GamePadState::Button::UP;
                    break;
                case PSClassicReport::Button::UPRIGHT:
                    gp.buttons = gp.buttons | io::GamePadState::Button::UP | io::GamePadState::Button::RIGHT;
                    break;
                case PSClassicReport::Button::RIGHT:
                    gp.buttons = gp.buttons | io::GamePadState::Button::RIGHT;
                    break;
                case PSClassicReport::Button::RIGHTDOWN:
                    gp.buttons = gp.buttons | io::GamePadState::Button::RIGHT | io::GamePadState::Button::DOWN;
                    break;
                case PSClassicReport::Button::DOWN:
                    gp.buttons = gp.buttons | io::GamePadState::Button::DOWN;
                    break;
                case PSClassicReport::Button::DOWNLEFT:
                    gp.buttons = gp.buttons | io::GamePadState::Button::DOWN | io::GamePadState::Button::LEFT;
                    break;
                case PSClassicReport::Button::LEFT:
                    gp.buttons = gp.buttons | io::GamePadState::Button::LEFT;
                    break;
                case PSClassicReport::Button::LEFTUP:
                    gp.buttons = gp.buttons | io::GamePadState::Button::LEFT | io::GamePadState::Button::UP;
                    break;
                case PSClassicReport::Button::SELECT:
                    gp.buttons = gp.buttons | io::GamePadState::Button::SELECT;
                    break;
                case PSClassicReport::Button::START:
                    gp.buttons = gp.buttons | io::GamePadState::Button::START;
                    break;
                case PSClassicReport::Button::SELECTSTART:
                    gp.buttons = gp.buttons | io::GamePadState::Button::SELECT | io::GamePadState::Button::START;
                    break;
                case PSClassicReport::Button::SELECTUP:
                    gp.buttons = gp.buttons | io::GamePadState::Button::SELECT | io::GamePadState::Button::UP;
                    break;
                case PSClassicReport::Button::SELECTDOWN:
                    gp.buttons = gp.buttons | io::GamePadState::Button::SELECT | io::GamePadState::Button::DOWN;
                    break;
                default:
                    break;
                }

                if (memcmp(previousbuffer, report, len) != 0 || firstReport)
                {

                    printf("Hat: %d\n", r->hat & PSClassicReport::Button::LEFT);
                    firstReport = false;
                    printf("PS Classic: len = %d - ", len);
                    // print in binary len report bytes
                    for (int i = 0; i < len; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            printf("%d", (report[i] >> (7 - j)) & 1);
                        }
                        printf(" ");
                    }

                    printf("\n");
                    // print 8 bytes of report in hex
                    printf("                        ");
                    for (int i = 0; i < len; i++)
                    {
                        printf("      %02x ", report[i]);
                    }
                    printf("\n");
                    memcpy(previousbuffer, report, len);
                }
            }
            else
            {
                printf("Invalid PSClassic Mini report size %zd\n", len);
                return;
            }
        }
        else
        {
            if (rpt_count == 1 && rpt_info_arr[0].report_id == 0)
            {
                // Simple report without report ID as 1st byte
                rpt_info = &rpt_info_arr[0];
            }
            else
            {
                // Composite report, 1st byte is report ID, data starts from 2nd byte
                uint8_t const rpt_id = report[0];

                // Find report id in the arrray
                for (uint8_t i = 0; i < rpt_count; i++)
                {
                    if (rpt_id == rpt_info_arr[i].report_id)
                    {
                        rpt_info = &rpt_info_arr[i];
                        break;
                    }
                }

                report++;
                len--;
            }

            if (!rpt_info)
            {
                printf("Couldn't find the report info for this report !\n");
                return;
            }

            //        printf("usage %d, %d\n", rpt_info->usage_page, rpt_info->usage);

            if (rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP)
            {
                switch (rpt_info->usage)
                {
                case HID_USAGE_DESKTOP_KEYBOARD:
                {
                    // TU_LOG1("HID receive keyboard report\n");
                    //  Assume keyboard follow boot report layout
                    //  process_kbd_report((hid_keyboard_report_t const *)report);
                    auto r = reinterpret_cast<const hid_keyboard_report_t *>(report);
                    auto &gp = io::getCurrentGamePadState(0);
                    gp.buttons = 0;
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        // A = 4, S = 22, Z=29, X=27, UP=82, DOWN=81, LEFT=80, RIGHT=79
                        if (r->keycode[i])
                        {
                            switch (r->keycode[i])
                            {
                            case 4:
                                gp.buttons |= io::GamePadState::Button::SELECT;
                                break;
                            case 22:
                                gp.buttons |= io::GamePadState::Button::START;
                                break;
                            case 29:
                                gp.buttons |= io::GamePadState::Button::B;
                                break;
                            case 27:
                                gp.buttons |= io::GamePadState::Button::A;
                                break;
                            case 82:
                                gp.buttons |= io::GamePadState::Button::UP;
                                break;
                            case 81:
                                gp.buttons |= io::GamePadState::Button::DOWN;
                                break;
                            case 80:
                                gp.buttons |= io::GamePadState::Button::LEFT;
                                break;
                            case 79:
                                gp.buttons |= io::GamePadState::Button::RIGHT;
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    break;
                }
                case HID_USAGE_DESKTOP_MOUSE:
                    TU_LOG1("HID receive mouse report\n");
                    // Assume mouse follow boot report layout
                    //                process_mouse_report((hid_mouse_report_t const *)report);
                    break;

                case HID_USAGE_DESKTOP_JOYSTICK:
                {
                    // TU_LOG1("HID receive joystick report\n");
                    struct JoyStickReport
                    {
                        uint8_t axis[3];
                        uint8_t buttons;
                        // 実際のところはしらん
                    };
                    auto *rep = reinterpret_cast<const JoyStickReport *>(report);
                    //                printf("x %d y %d button %02x\n", rep->axis[0], rep->axis[1], rep->buttons);
                    auto &gp = io::getCurrentGamePadState(0);
                    gp.axis[0] = rep->axis[0];
                    gp.axis[1] = rep->axis[1];
                    gp.axis[2] = rep->axis[2];
                    gp.buttons = rep->buttons;
                    gp.convertButtonsFromAxis(0, 1);

                    // BUFFALO BGC-FC801
                    // VID = 0411, PID = 00c6
                }
                break;

                case HID_USAGE_DESKTOP_GAMEPAD:
                    TU_LOG1("HID receive gamepad report\n");

                    break;

                default:
                    break;
                }
            }
        }

        if (!tuh_hid_receive_report(dev_addr, instance))
        {
            printf("Error: cannot request to receive report\r\n");
        }
    }

#ifdef __cplusplus
}
#endif