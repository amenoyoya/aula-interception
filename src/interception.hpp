#pragma once

#include <aula/core.hpp>
#include <windows.h>

#define INTERCEPTION_STATIC
#include "../interception/library/interception.h"

namespace Aula {
    namespace Interception {
        /// ProcessPriority enum
        enum ProcessPriority {
            IDLE = IDLE_PRIORITY_CLASS,
            BELOW_NORMAL = BELOW_NORMAL_PRIORITY_CLASS,
            NORMAL = NORMAL_PRIORITY_CLASS,
            ABOVE_NORMAL = ABOVE_NORMAL_PRIORITY_CLASS,
            HIGH = HIGH_PRIORITY_CLASS,
            REALTIME = REALTIME_PRIORITY_CLASS,
        };

        /// Set current process priority
        inline bool setCurrentProcessPriority(ProcessPriority priority) {
            return FALSE != SetPriorityClass(GetCurrentProcess(), priority);
        }

        /// InterceptionDevice wrapper struct
        struct Device {
            u32 index;
            std::string hardwareId;
            bool isKeyboard, isMouse;
        };

        /// InterceptionMouseStroke
        struct MouseStroke {
            u16 state, flags;
            i16 rolling;
            i32 x, y;
            u32 information;

            MouseStroke(): state(0), flags(0), rolling(0), x(0), y(0), information(0) {}
            explicit MouseStroke(u16 state, u16 flags, i16 rolling, i32 x, i32 y, u32 information):
                state(state), flags(flags), rolling(rolling), x(x), y(y), information(information) {}
        };

        /// InterceptionKeyStroke
        struct KeyStroke {
            u16 code, state;
            u32 information;

            KeyStroke(): code(0), state(0), information(0) {}
            explicit KeyStroke(u16 code, u16 state, u32 information): code(code), state(state), information(information) {}
        };

        /// InterceptionContext wrapper class
        class Context: public Object {
        public:
            Context(): Object(), context(0), device(0) {
                open();
            }
            ~Context() {
                close();
            }

            bool open() {
                close();
                if (0 == (context = interception_create_context())) {
                    _state = FAILED;
                    _message = "failed to create interception context";
                    return false;
                };
                _state = ACTIVE;
                return true;
            }

            void close() {
                if (context) {
                    interception_destroy_context(context);
                    context = 0;
                    device = 0;
                    memset(stroke, 0, sizeof(stroke));
                }
                _state = NONE;
                _message.clear();
            }

            /// Enumerate all deveices
            std::vector<Device> enumerateDevices() {
                std::vector<Device> devices;

                for (u32 i = 0; i < INTERCEPTION_MAX_KEYBOARD; ++i) {
                    u32 index = INTERCEPTION_KEYBOARD(i);
                    std::string hardwareId = getHardwareId(index);

                    if (hardwareId != "") devices.push_back({ index, hardwareId, true, false });
                }
                for (u32 i = 0; i < INTERCEPTION_MAX_MOUSE; ++i) {
                    u32 index = INTERCEPTION_MOUSE(i);
                    std::string hardwareId = getHardwareId(index);

                    if (hardwareId != "") devices.push_back({ index, hardwareId, false, true });
                }
                return std::move(devices);
            }

            /// Set keyboard input filter
            void setKeyboardFilter(u32 filter) {
                if (context) interception_set_filter(context, interception_is_keyboard, (InterceptionFilter)filter);
            }

            /// Set mouse input filter
            void setMouseFilter(u32 filter) {
                if (context) interception_set_filter(context, interception_is_mouse, (InterceptionFilter)filter);
            }

            /// Get device and stroke of current input
            // Use for while-loop
            bool recieve() {
                return context? interception_receive(context, device = interception_wait(context), &stroke, 1) > 0: false;
            }

            /// Send current input as default input
            bool pass() {
                return (context && device) ? 0 < interception_send(context, device, &stroke, 1) : false;
            }

            /// Send input to keyboard device
            bool sendKeyStroke(u32 deviceIndex, KeyStroke *stroke) {
                return context ? 0 < interception_send(context, deviceIndex, (InterceptionStroke *)stroke, 1) : false;
            }

            /// Send input to mouse device
            bool sendMouseStroke(u32 deviceIndex, MouseStroke *stroke) {
                return context ? 0 < interception_send(context, deviceIndex, (InterceptionStroke *)stroke, 1) : false;
            }

            /// Get current input as key stroke
            // nullptr will be return if the current input is not a key input
            KeyStroke *getCurrentKeyStroke() const {
                if (!device || !interception_is_keyboard(device)) return nullptr;
                return (KeyStroke *)&stroke;
            }

            /// Get current input as key stroke
            // nullptr will be return if the current input is not a key input
            MouseStroke *getCurrentMouseStroke() const {
                if (!device || !interception_is_mouse(device)) return nullptr;
                return (MouseStroke *)&stroke;
            }

            /// Get current device index
            u32 getCurrentDeviceIndex() const {
                return context ? (u32)device : 0;
            }

            /// Get input device hardware_id
            // @param u32 deviceIndex: if unsigned(-1) is designated, get the current device hardware_id
            std::string getHardwareId(u32 deviceIndex = (u32)(-1)) const {
                wchar_t hardwareId[512];
                if (interception_get_hardware_id(
                    context,
                    deviceIndex == (u32)(-1) ? device : (InterceptionDevice)deviceIndex,
                    (void *)hardwareId,
                    sizeof(hardwareId)
                ) == 0) return "";
                return std::move(Encoding::toUTF8(hardwareId));
            }
        private:
            InterceptionContext context;
            InterceptionDevice  device;
            InterceptionStroke  stroke;
        };


        /*** マウス・キー入力エミュレーション ***/

        struct Point {
            i32 x, y;
        };

        struct Size {
            i32 width, height;
        };

        /// カーソル位置取得
        inline std::unique_ptr<Point> getCursorPos() {
            std::unique_ptr<Point> pos(new Point{0, 0});
            return FALSE == GetCursorPos((POINT*)pos.get()) ? nullptr : std::move(pos);
        }
        
        /// スクリーンサイズ取得
        inline std::unique_ptr<Size> getScreenSize() {
            return std::move(std::unique_ptr<Size>(new Size{
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN)
            }));
        }

        /// カーソル位置を移動
        inline bool warpCursor(i32 x, i32 y) {
            INPUT input = {
                INPUT_MOUSE,
                x * 65535 / (GetSystemMetrics(SM_CXSCREEN) - 1),
                y * 65535 / (GetSystemMetrics(SM_CYSCREEN) - 1),
                0,
                MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,
                0,
                0
            };
            return FALSE != SendInput(1, &input, sizeof(INPUT));
        }
        
        /// マウスボタン入力
        enum MouseAction {
            LEFTDOWN   = 0x0002,
            LEFTUP     = 0x0004,
            RIGHTDOWN  = 0x0008,
            RIGHTUP    = 0x0010,
            MIDDLEDOWN = 0x0020,
            MIDDLEUP   = 0x0040,
            XDOWN      = 0x0080,
            XUP        = 0x0100,
        };
        inline bool sendMouseAction(MouseAction action) {
            INPUT input = {INPUT_MOUSE, 0, 0, 0, (DWORD)action, 0, 0};
            return FALSE != SendInput(1, &input, sizeof(INPUT));
        }
        
        /// ホイール回転
        // @param rot: 回転量は120の倍数。通常は 120 か -120 でOK
        inline bool sendMouseWheel(i32 rot) {
            INPUT input = {INPUT_MOUSE, 0, 0, (DWORD)rot, MOUSEEVENTF_WHEEL, 0, 0};
            return FALSE != SendInput(1, &input, sizeof(INPUT));
        }
        
        /// キー入力
        enum KeyAction {
            UP = -1, // 離す
            DOWNUP = 0, // 押して離す
            DOWN = 1, // 押しっぱなし
        };

        // @param vkey: VK_***
        // @param mode: -1=離す, 0=押して離す, 1=押しっぱなし
        inline bool sendKeyAction(u8 vkey, KeyAction mode = DOWNUP) {
            if (mode == 0) {
                INPUT input[2];
                input[0].type = input[1].type = INPUT_KEYBOARD;
                input[0].ki.wVk = input[1].ki.wVk = vkey;
                input[0].ki.wScan = input[1].ki.wScan = MapVirtualKey(vkey, 0);
                input[0].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
                input[1].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
                input[0].ki.time = input[1].ki.time = 0;
                input[0].ki.dwExtraInfo = input[1].ki.dwExtraInfo = GetMessageExtraInfo();
                return FALSE != SendInput(2, input, sizeof(INPUT));
            }

            INPUT input;
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = vkey;
            input.ki.wScan = MapVirtualKey(input.ki.wVk, 0);
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | (mode < 0? KEYEVENTF_KEYUP: 0);
            input.ki.time = 0;
            input.ki.dwExtraInfo = GetMessageExtraInfo();
            return FALSE != SendInput(1, &input, sizeof(INPUT));
        }
        
        /// 全角(UTF-8)・半角文字列入力
        // ファンクションキーやコントロールキーを使わないならこっちのほうが便利
        inline bool sendKeyString(const std::string &keys) {
            std::wstring str = Encoding::utf8ToWideString(keys);
            
            for (wchar_t *p = (wchar_t*)str.c_str(); *p; ++p) {
                INPUT input[2];
                
                input[0].type = input[1].type = INPUT_KEYBOARD;
                input[0].ki.wVk = input[1].ki.wVk = 0;
                input[0].ki.wScan = input[1].ki.wScan = *p;
                input[0].ki.time = input[1].ki.time = 0;
                input[0].ki.dwExtraInfo = input[1].ki.dwExtraInfo = GetMessageExtraInfo();
                input[0].ki.dwFlags = KEYEVENTF_UNICODE;
                input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
                if (FALSE == SendInput(2, input, sizeof(INPUT))) return false;
            }
            return true;
        }
    }
}