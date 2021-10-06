-- #aula v1.0.0

include "../dist/interception.dll"
local keymap = include "_keymap"

-- Razer Logicool G600
local g600_key_id = "HID\\VID_046D&PID_C232"
local g600_mouse_id = "HID\\VID_046D&PID_C24A&REV_7702&MI_00"

-- Current process priority: HIGH
if not Aula.Interception.setCurrentProcessPriority(Aula.Interception.ProcessPriority.HIGH) then
    error("Failed to set current process priority")
end

-- Interception context
local context = Aula.Interception.Context.new()
if context:getState() == Aula.Object.State.FAILED then
    error("Failed to create interception context")
end

-- Find input devices
local g600_key = nil
local g600_mouse = nil

for _, device in ipairs(context:enumerateDevices()) do
    if device.hardwareId == g600_key_id then
        g600_key = device
    end
    if device.hardwareId == g600_mouse_id then
        g600_mouse = device
    end
end

if g600_key == nil then
    error("Failed to find Logicool G600 keyboard")
end

if g600_mouse == nil then
    error("Failed to find Logicool G600 mouse")
end

-- Set input filter
context:setKeyboardFilter(Aula.Interception.FILTER_KEY_ALL)
context:setMouseFilter(Aula.Interception.FILTER_MOUSE_ALL - Aula.Interception.FILTER_MOUSE_MOVE)

-- Wrapper of Aula.Interception.sendKeyAction
--- ※ 動作確認用。Raibow Six Siege 等では上手く動かない
local function send(stroke, vk)
    return Aula.Interception.sendKeyAction(
        vk,
        stroke.state == Aula.Interception.KEY_DOWN and Aula.Interception.KeyAction.DOWN or Aula.Interception.KeyAction.UP
    )
end

-- Recieve and process inputs
-- ※ Rainbow Six Siege 等のゲームは Win32API.SendInput 等のキー入力送信を防止する仕組みがあるようなので
---   interception_send のような低レベル API でキー入力送信する必要がある
while context:recieve() do
    local device = context:getCurrentDeviceIndex()
    local keystroke = context:getCurrentKeyStroke()
    local mousestroke = context:getCurrentMouseStroke()
    
    if keystroke then
        if keystroke.code == Aula.Interception.KC_BACK then break end -- Exit if BackSpace key pressed
        
        -- Logicool G600 key input
        if device == g600_key.index then
            if keystroke.code == keymap.g600["9"] then -- G600[09] => Keyboard[Q]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_Q, keystroke.state, 0)) then
                    goto continue
                end
                --[[ 以下のようなキーマッピングはチート対策されているゲームでは上手く動かない ]]
                -- if send(keystroke, Aula.Interception.VK_Q) then
                --     goto continue
                -- end
            end
            if keystroke.code == keymap.g600["10"] then -- G600[10] => Keyboard[W]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_W, keystroke.state, 0)) then
                    goto continue
                end
                -- if send(keystroke, Aula.Interception.VK_W) then
                --     goto continue
                -- end
            end
            if keystroke.code == keymap.g600["11"] then -- G600[11] => Keyboard[E]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_E, keystroke.state, 0)) then
                    goto continue
                end
                -- if send(keystroke, Aula.Interception.VK_E) then
                --     goto continue
                -- end
            end
            if keystroke.code == keymap.g600["12"] then -- G600[12] => Keyboard[A]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_A, keystroke.state, 0)) then
                    goto continue
                end
                -- if send(keystroke, Aula.Interception.VK_A) then
                --     goto continue
                -- end
            end
            if keystroke.code == keymap.g600["13"] then -- G600[13] => Keyboard[S]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_S, keystroke.state, 0)) then
                    goto continue
                end
                -- if send(keystroke, Aula.Interception.VK_S) then
                --     goto continue
                -- end
            end
            if keystroke.code == keymap.g600["14"] then -- G600[14] => Keyboard[D]
                if context:sendKeyStroke(device, Aula.Interception.KeyStroke.new(Aula.Interception.KC_D, keystroke.state, 0)) then
                    goto continue
                end
                -- if send(keystroke, Aula.Interception.VK_D) then
                --     goto continue
                -- end
            end
        end
        printf("Keyboard: %s\n\tstate: %d\n\tcode: %d\n\tinformation: %d\n",
            context:getHardwareId(device), keystroke.state, keystroke.code, keystroke.information)
    end

    if mousestroke then
        printf("Mouse: %s\n\tstate: %d\n\tflags: %d\n\trolling: %d\n\tinformation: %d\n",
            context:getHardwareId(device), mousestroke.state, mousestroke.flags, mousestroke.rolling, mousestroke.information)
    end

    -- Pass through input
    context:pass()

    :: continue ::
end
