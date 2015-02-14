/**
 * @file Cosa/SPI/Driver/MCP4822.hh
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

#ifndef COSA_SPI_DRIVER_MCP4822_HH
#define COSA_SPI_DRIVER_MCP4822_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa SPI driver for Microchip MCP4822 Digital-to-Analog converter
 * The MCP4822/4812/4822 devices are dual 8-bit, 10-bit and 12-bit
 * buffered voltage output Digital-to-Analog Converters (DACs),
 * respectively. The devices operate from a single 2.7V to 5.5V
 * supply with SPI compatible Serial Peripheral Interface.
 *
 * @section Circuit
 * The MCP4822 has two buffered voltage outputs (VOUTA and VOUTB)
 * and a high precision internal voltage reference. Each DAC can
 * be operated in Active or Shutdown mode individually by setting
 * the Configuration register bits. Synchronous updates of the DAC
 * outputs can be performed by using the LDAC pin.
 * @code
 *                     MCP4822
 *                 +-------------+
 * (VCC)---------1-|VCC     VOUTA|-8------------->
 * (D9)----------2-|CE        VSS|-7---------(GND)
 * (D13/SCK)-----3-|SCK     VOUTB|-6------------->
 * (D11/MOSI)----4-|SDI      LDAC|-5------(D14/D3)
 *                 +-------------+
 * @endcode
 *
 * @section References
 * 1. http://ww1.microchip.com/downloads/en/DeviceDoc/22249A.pdf
 * DS22249A, Rev. A, April 2010.
 */
class MCP4822 : private SPI::Driver {
public:
    /** DACA or DACB Selection bit mask */
    enum DAC_SELECT : uint16_t {
        DAC_A = 0x0000,
        DAC_B = 0x8000
    } __attribute__((packed));
    
    /** Output Gain Selection bit mask */
    enum GAIN_SELECT : uint16_t {
        X1 = 0x2000,
        X2 = 0x0000
    } __attribute__((packed));
    
    /**
     * Construct MCP4822 driver with given chip select and latch pins.
     */
#if !defined(BOARD_ATTINY)
    MCP4822(Board::DigitalPin csn = Board::D3, OutputPin latch = Board::D15) :
      SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DEFAULT_CLOCK, 0, SPI::MSB_ORDER, NULL),
      m_voltages(),
      p_latch(latch)
    {}
#else
    MCP4822(Board::DigitalPin csn = Board::D14, OutputPin latch = Board::D15) :
      SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DEFAULT_CLOCK, 0, SPI::MSB_ORDER, NULL),
      m_voltages(),
      p_latch(latch)
    {}
#endif
    
    /**
     * Issue an voltage set command to DAC channel dac. The output is automatically
     * scaled to get the best output resolution.
     * @param[in] dac DAC channel to set
     * @param[in] voltage integer voltage between 0 and 2^13 (0V-4.096V)
     */
    void set_voltage(DAC_SELECT dac, uint16_t voltage);
    
    /**
     * Issue an voltage set command to both DAC channels and latch synchronously.
     * The output is automatically scaled to get the best output resolution.
     * @param[in] voltageA integer voltage between 0 and 2^13 (0V-4.096V)
     * @param[in] voltageB integer voltage between 0 and 2^13 (0V-4.096V)
     */
    void set_voltages(uint16_t voltageA, uint16_t voltageB);
    
    /**
     * Issue an shutdown command to DAC channel dac. The output is connected to a
     * large resistance to ground.
     * @param[in] dac DAC channel to shutdown
     */
    void shutdown(DAC_SELECT dac);
    
    /**
     * Return latest commanded voltage for DAC channel A.
     * @return float between 0V-4.096V
     */
    float get_voltage(DAC_SELECT dac) const
    __attribute__((always_inline))
    {
        // equation derived from equation in datasheet p.17,
        // except that we're keeping track of the gain in our int
        switch (dac) {
            case DAC_SELECT::DAC_A: return (m_voltages.voltageA/8192) * 4.096;
            case DAC_SELECT::DAC_B: return (m_voltages.voltageB/8192) * 4.096;
        }
    }
    
    /**
     * Return latest commanded voltage for DAC channel A.
     * @return 13-bit integer voltage (full scale 0V-4.096V)
     */
    int16_t get_raw_voltage(DAC_SELECT dac) const
    __attribute__((always_inline))
    {
        switch (dac) {
            case DAC_SELECT::DAC_A: return m_voltages.voltageA;
            case DAC_SELECT::DAC_B: return m_voltages.voltageB;
        }
    }

    
protected:
    /** Output Shutdown Control bit 
     * The output is disabled if this bit is set to zero.
     */
    static const uint16_t EN_OUT = 0x1000;
    
    /** DAC Input Data bit mask 
     * We want to mask off any bits more significant than bit D11
     * (datasheet p.22)
     */
    static const uint16_t DATA_MASK = 0x0FFF;
    
    /** Voltage value storage */
    struct voltage_t
    {
        uint16_t voltageA;
        uint16_t voltageB;
    };
    
    voltage_t m_voltages;
    
    /** Latch (LDAC) pin */
    OutputPin p_latch;
    
    /**
     * Issue given command and return result.
     * @param[in] cmd command code.
     * @return result.
     */
    void issue(uint16_t cmd);

};

/**
 * Print the latest output voltage to the given output stream.
 * @param[in] outs output stream.
 * @param[in] mcp instance.
 * @return output stream.
 */
extern IOStream& operator<<(IOStream& outs, MCP4822& mcp);

#endif
