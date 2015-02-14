/**
 * @file Cosa/SPI/Driver/MCP4822.cpp
 * @version 0.1
 *
 * @section License
 * Copyright (C) 2015, Patrick Fasano
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "MCP4822.hh"
#include "Cosa/Trace.hh"

void
MCP4822::set_voltage(DAC_SELECT dac, uint16_t voltage)
{
    // the MCP4822 expects 2-byte commands
    uint16_t command = 0x0000;
    
    // rescale if the voltage needs the preamp, and set the correct
    // gain selection bit
    if (voltage > 4095) {
        command = (voltage >> 1) & DATA_MASK;
        command = command | GAIN_SELECT::X2;
    } else {
        command = voltage & DATA_MASK;
        command = command | GAIN_SELECT::X1;
    }
    
    // set the channel bit and enable the DAC
    command = command | dac;
    command = command | EN_OUT;
    
    issue(command);
    
    // store the value we just set
    switch (dac) {
        case DAC_SELECT::DAC_A: m_voltages.voltageA = voltage; break;
        case DAC_SELECT::DAC_B: m_voltages.voltageB = voltage; break;
    }
}

void
MCP4822::set_voltages(uint16_t voltageA, uint16_t voltageB)
{
    // bring the latch pin high to disable voltage update on CS up
    p_latch.high();
    
    // set each voltage sequentially
    set_voltage(DAC_SELECT::DAC_A, voltageA);
    set_voltage(DAC_SELECT::DAC_B, voltageB);
    
    // bring the latch pin low to output new voltages
    p_latch.low();
}

void
MCP4822::shutdown(DAC_SELECT dac)
{
    // the shutdown command is simply the channel select bit with nothing
    // else set
    uint16_t command = 0x0000 | dac;
    
    issue(command);
    
    // store the value we just set
    switch (dac) {
        case DAC_SELECT::DAC_A: m_voltages.voltageA = 0; break;
        case DAC_SELECT::DAC_B: m_voltages.voltageB = 0; break;
    }
}

void
MCP4822::issue(uint16_t cmd)
{
    spi.acquire(this);
    spi.begin();
    // this chip expects instructions in big-endian format
    cmd = swap(cmd);
    spi.write(&cmd, sizeof(cmd));
    spi.end();
    spi.release();
}

IOStream&
operator<<(IOStream& outs, MCP4822& mcp)
{
    outs << PSTR("MCP4822(voltage A = ") << mcp.get_raw_voltage(MCP4822::DAC_SELECT::DAC_A)
    << PSTR(", voltage B = ") << mcp.get_raw_voltage(MCP4822::DAC_SELECT::DAC_B)
    << PSTR(")");
    return (outs);
}
