-- #aula v1.0.0

include "../dist/interception.dll"

-- Razer Tartarus V2 & Logicool G600
local tartarus_key_id = "HID\\VID_1532&PID_022B&REV_0200&MI_00"

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
local tartarus_key = nil
local g600_key = nil
local g600_mouse = nil

for _, device in ipairs(context:enumerateDevices()) do
    if device.hardwareId == tartarus_key_id then
        tartarus_key = device
    end
    if device.hardwareId == g600_key_id then
        g600_key = device
    end
    if device.hardwareId == g600_mouse_id then
        g600_mouse = device
    end
end

if tartarus_key == nil then
    error("Failed to find Razer Tartarus V2 keyboard")
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

-- Recieve and process inputs
while context:recieve() do
    local device = context:getCurrentDeviceIndex()
    -- Process for Razer Tartarus V2 or Logicool G600
    if device == tartarus_key.index or device == g600_key.index or device == g600_mouse.index then
        local keystroke = context:getCurrentKeyStroke()
        if keystroke then
            printf("Keyboard: %s\n\tstate: %d\n\tcode: %d\n", context:getHardwareId(device), keystroke.state, keystroke.code)
        end

        local mousestroke = context:getCurrentMouseStroke()
        if mousestroke then
            printf("Mouse: %s\n\tstate: %d\n\tflags: %d\n\trolling: %d\n",
                context:getHardwareId(device), mousestroke.state, mousestroke.flags, mousestroke.rolling)
        end
    end

    -- Exit if ESC key pressed
    local keystroke = context:getCurrentKeyStroke()
    if keystroke and keystroke.code == 1 then
        break
    end

    -- Pass through input
    context:pass()
end
