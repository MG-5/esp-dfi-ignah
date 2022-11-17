#include "LedControl.hpp"
#include "helpers/freertos.hpp"

void LedControl::init()
{
    initGpios();
    clearShiftRegisters();
}

//--------------------------------------------------------------------------------------------------
void LedControl::submitImage(const uint8_t *image, size_t length)
{
    if (length < (TotalLeds / 8))
        return;

    // send LED data to row 1 and 3 simultanously
    setTripleControlShiftRegister(0b101);
    shiftDualLedData(0, image);

    // send LED data to row 2 and 4 simultanously
    setTripleControlShiftRegister(0b1010);
    shiftDualLedData(LedsPerRow, image);

    // send LED data to row 5 at last
    setTripleControlShiftRegister(0b10000);
    shiftLedDataRow5(image);

    triggerRegisterClock();
}

//--------------------------------------------------------------------------------------------------
void LedControl::initGpios()
{
    ControlShiftInput.init(GPIO_MODE_OUTPUT);
    ControlShiftClock.init(GPIO_MODE_OUTPUT);
    ControlEnableOutput.init(GPIO_MODE_OUTPUT);
    RegisterClear.init(GPIO_MODE_OUTPUT);
    RegisterClock.init(GPIO_MODE_OUTPUT);
    ShiftDataIn2.init(GPIO_MODE_OUTPUT);
    ShiftDataIn1.init(GPIO_MODE_OUTPUT);
    ShiftClock2.init(GPIO_MODE_OUTPUT);
    ShiftClock1.init(GPIO_MODE_OUTPUT);
}

//--------------------------------------------------------------------------------------------------
void LedControl::setTripleControlShiftRegister(uint8_t rows)
{
    uint8_t secondRegister =
        0b00000000; // controls OR-Gate, which can disable LEDs, not needed at moment
    uint8_t thirdRegister = 0b00000000; // some power control, not needed at moment

    shiftControlData(thirdRegister);
    shiftControlData(secondRegister);
    shiftControlData(rows);
}

//--------------------------------------------------------------------------------------------------
void LedControl::triggerControlShiftRegisterOutput()
{
    ControlEnableOutput.write(true);
    delay200ns();
    ControlEnableOutput.write(false);
    delay200ns();
}

//--------------------------------------------------------------------------------------------------
void LedControl::shiftControlData(uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        bool pinState = 1 & (byte >> (7 - (i % 8)));

        ControlShiftInput.write(pinState);

        ControlShiftClock.write(true);
        delay200ns();
        ControlShiftClock.write(false);
        delay200ns();
    }
}

//--------------------------------------------------------------------------------------------------
void LedControl::clearShiftRegisters()
{
    setTripleControlShiftRegister(0xFF);
    RegisterClear.write(false);
    triggerControlShiftRegisterOutput();
    RegisterClear.write(true);
    triggerControlShiftRegisterOutput();
}

//--------------------------------------------------------------------------------------------------
void LedControl::triggerRegisterClock()
{
    RegisterClock.write(true);
    triggerControlShiftRegisterOutput();
    RegisterClock.write(false);
    triggerControlShiftRegisterOutput();
}

//--------------------------------------------------------------------------------------------------
void LedControl::shiftDualLedData(const size_t startPosition, const uint8_t *image)
{
    for (int i = startPosition; i < startPosition + LedsPerRow; i++)
    {
        bool pinState1 = 1 & (image[i / 8] >> (7 - (i % 8)));
        ShiftDataIn1.write(pinState1);

        bool pinState2 = 1 & (image[i / 8 + (2 * (LedsPerRow / 8))] >> (7 - (i % 8)));
        ShiftDataIn2.write(pinState2);

        ShiftClock1.write(true);
        ShiftClock2.write(true);
        triggerControlShiftRegisterOutput();

        ShiftClock1.write(false);
        ShiftClock2.write(false);
        triggerControlShiftRegisterOutput();
    }
}

//--------------------------------------------------------------------------------------------------
void LedControl::shiftLedDataRow5(const uint8_t *image)
{
    for (int i = 4 * LedsPerRow; i < 5 * LedsPerRow; i++)
    {
        bool pinState = 1 & (image[i / 8] >> (7 - (i % 8)));
        ShiftDataIn2.write(pinState);

        ShiftClock2.write(true);
        triggerControlShiftRegisterOutput();

        ShiftClock2.write(false);
        triggerControlShiftRegisterOutput();
    }
}

//--------------------------------------------------------------------------------------------------
void LedControl::delay200ns()
{
#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_160
#error "Frequency is not set to 160MHz"
#endif

    // 160MHz = 6,25ns per cycle
    // 200ns / 6,25ns = 32 cycles

    asm("nop; nop; nop; nop; nop; nop; nop; nop;"
        "nop; nop; nop; nop; nop; nop; nop; nop;"
        "nop; nop; nop; nop; nop; nop; nop; nop;"
        "nop; nop; nop; nop; nop; nop; nop; nop;");
}
