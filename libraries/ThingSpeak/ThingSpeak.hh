/**
 * @file ThingSpeak.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_IOT_THINGSPEAK_HH
#define COSA_IOT_THINGSPEAK_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"
#include "Cosa/IOBuffer.hh"

/**
 * ThingSpeak client implementation; Channel update and TalkBack
 * command handler.
 * 
 * @section Reference
 * 1. ThingSpeak API, http://community.thingspeak.com/documentation/api/
 */
class ThingSpeak {
public:
  class Channel;
  class TalkBack;
  
  class Client {
  public:
    /**
     * Construct Thingspeak client.
     */
    Client();

    /**
     * Destruct Thingspeak client. Close and release socket.
     */
    ~Client();

    /**
     * Start Thingspeak client with given socket. Returns true if
     * successful otherwise false.
     * @param[in] sock connection-oriented socket to use.
     * @return bool
     */
    bool begin(Socket* sock);

    /**
     * Stop Thingspeak client, close and release socket. Returns true
     * if successful (correct mode) otherwise false.
     * @return bool
     */
    bool end();

  private:
    /**
     * Connect to the server. Return zero if successful otherwise 
     * negative error code.
     * @return zero or negative error code.
     */
    int connect();

    /**
     * Disconnect from the server. Return zero if successful otherwise 
     * negative error code.
     * @return zero or negative error code.
     */
    int disconnect();

    Socket* m_sock;
    
    friend class Channel;
    friend class TalkBack;
  };

  class Channel {
  public:
    /**
     * Construct channel to given client and given write key.
     * @param[in] client Thingspeak client.
     * @param[in] key api write key for channel (in program memory).
     */
    Channel(Client* client, const char* key);

    /**
     * Post given entry update to channel. The entry string is in
     * the format "field1=value1&...&field8=value8". Number of fields
     * depend on the channel definition in Thingspeak. Returns zero if
     * successful otherwise negative error code.
     * @param[in] entry string with field-value update.
     * @param[in] status entry annotation string (in program memory).
     * @return zero if successful otherwise negative error code.
     */
    int post(const char* entry, str_P status = NULL);

  private:
    Client* m_client;
    const char* m_key;
  };

  /**
   * ThingSpeak channel update support. Construct entry command
   * using field identity and value updates.
   */
  class Entry {
  public:
    /**
     * Construct ThingSpeak channel update entry. Initiate buffer 
     * andio-stream for creating command string.
     */
    Entry() : 
      m_buf(), 
      m_cout(&m_buf) 
    {}

    /**
     * Set field with given identity and value. The type of the
     * value is also given as a template parameter. The type must
     * the iostream output operator.
     * @param[in] T value type.
     * @param[in] id field identity (1..8).
     * @param[in] value for field.
     */
    template<class T> 
    void set_field(uint8_t id, T value)
    {
      if (!m_buf.is_empty()) m_cout << '&';
      m_cout << PSTR("field") << id << '=' << value;
    }

    /**
     * Set field with given identity, unsigned 16-bit value, decimals 
     * and sign. 
     * @param[in] id field identity (1..8)
     * @param[in] value for field.
     * @param[in] decimals scaling of value.
     * @param[in] sign add sign if true.
     */
    void set_field(uint8_t id, uint16_t value, uint8_t decimals, bool sign = false);

    /**
     * Set field with given identity, signed 16-bit value and decimals.
     * @param[in] id field identity (1..8)
     * @param[in] value for field.
     * @param[in] decimals scaling of value.
     */
    void set_field(uint8_t id, int16_t value, uint8_t decimals)
      __attribute__((always_inline))
    {
      bool sign = (value < 0);
      if (sign) value = -value;
      set_field(id, (uint16_t) value, decimals, sign);
    }

    /**
     * Set field with given identity, unsigned 32-bit value, decimals 
     * and sign. 
     * @param[in] id field identity (1..8)
     * @param[in] value for field.
     * @param[in] decimals scaling of value.
     * @param[in] sign add sign if true.
     */
    void set_field(uint8_t id, uint32_t value, uint8_t decimals, bool sign = false);

    /**
     * Set field with given identity, signed 32-bit value and decimals.
     * @param[in] id field identity (1..8)
     * @param[in] value for field.
     * @param[in] decimals scaling of value.
     */
    void set_field(uint8_t id, int32_t value, uint8_t decimals)
      __attribute__((always_inline))
    {
      bool sign = (value < 0);
      if (sign) value = -value;
      set_field(id, (uint32_t) value, decimals, sign);
    }

    /**
     * Allow update structure to be cast to a constant string.
     * Will terminate the current update string and return the
     * buffer contents.
     * @return command string.
     */
    operator const char*()
      __attribute__((always_inline))
    {
      m_cout << ends;
      return ((const char*) m_buf);
    }

    /**
     * Empty the internal buffer to allow construction of a
     * new command line.
     */
    void empty()
      __attribute__((always_inline))
    {
      m_buf.empty();
    }

  private:
    static const size_t BUF_MAX = 128;
    IOBuffer<BUF_MAX> m_buf;
    IOStream m_cout;
  };
  
  /**
   * ThingSpeak TalkBack API client. Allow handing of commands
   * queued on server.
   */
  class TalkBack {
    friend class Command;
  public:

    /**
     * TalkBack command handler. Applications should sub-class
     * to implement application commands. 
     */
    class Command {
      friend class TalkBack;
    public:
      /**
       * Construct a command within the given talkback command
       * queue and with the given string.
       * @param[in] talkback handler.
       * @param[in] string for command.
       */
      Command(TalkBack* talkback, const char* string) :
	m_talkback(talkback),
	m_string((str_P) string)
      {}

      /**
       * Construct a command within the given talkback command
       * queue and with the given string.
       * @param[in] talkback handler.
       * @param[in] string for command.
       */
      Command(TalkBack* talkback, str_P string) :
	m_talkback(talkback),
	m_string(string)
      {}

      /**
       * Return command string. It is stored in program memory.
       * @return string.
       */
      str_P get_string() const
      {
	return (m_string);
      }

      /**
       * @override ThingSpeak::TalkBack::Command
       * The command handler. Called by TalkBack::execute_next_command
       * when receiving a command string that matches.
       */
      virtual void execute() {}
      
    protected:
      TalkBack* m_talkback;
      str_P m_string;

    private:
      Command* m_next;
    };

    /**
     * Construct a TalkBack handler connected to the given
     * ThingSpeak client with the given key and identity.
     * @param[in] client thingspeak client.
     * @param[in] key talkback api key.
     * @param[in] id talkback identity.
     */
    TalkBack(Client* client, const char* key, uint16_t id);

    /**
     * Add given command to the TalkBack handler.
     * @param[in] command to add.
     */
    void add(Command* command)
      __attribute__((always_inline))
    {
      command->m_next = m_first;
      m_first = command;
    }

    /**
     * Execute next command in server talkback command queue. Returns
     * zero if successful otherwise negative error code. 
     * @return zero or negative error code.
     */
    int execute_next_command();
    
    /**
     * Add the given command to the talkback command queue at the
     * given position. Default position is the end of the queue. The
     * command string should be in program memory. Returns
     * zero if successful otherwise negative error code. 
     * @param[in] string command in program memory.
     * @param[in] position in queue (first is one(1), last is zero(0)).
     * @return zero or negative error code.
     */
    int add_command_P(str_P string, uint8_t position = 0); 

  private:
    /**
     * Lookup command given command string. Returns reference to
     * Command handler or NULL.
     * @return Command or NULL.
     */
    Command* lookup(const char* name);

    Client* m_client;
    const char* m_key;
    uint16_t m_id;
    Command* m_first;
  };
};

#endif
