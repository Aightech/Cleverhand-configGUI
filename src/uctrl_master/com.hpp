#ifdef TEENSY_4 // only available on teensy 4
#include <NativeEthernet.h>
#endif


// Simple class managing comunication interfaces
class Com
{
  public:
    Com() {};

    // Begin the comunication with the available interfaces
    // the arguments are
    // - b : the baud of the serial comunication
    // - i3-i0 : are the bytes of the IP address 
    // - port : is the port listened by the server
    int begin(int b=9600, uint8_t i3=0, uint8_t i2=0, uint8_t i1=0, uint8_t i0=0, int port=-1)
    {
      Serial.begin(b);
#ifdef TEENSY_4 // only available on teensy 4
      byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
      Ethernet.begin(mac, IPAddress(i3, i2, i1, i0));
      m_server = new EthernetServer(port);
      m_server->begin();// start the server
#endif
      return 0;
    }

    // Return the number of byte available in the comunication buffer of the ethernet socket if available else Serial socket.
    int available()
    {
#ifdef TEENSY_4 // only available on teensy 4
      if (!isEthernet)
      {
        m_client = m_server->available();
        if (m_client)
          isEthernet = true;
      }

      if (isEthernet)
      {
        if (!m_client.connected())
          isEthernet = false;
        else
          return m_client.available();
      }
      else
#endif
        return Serial.available();
      return 0;
    }

    // Read n bytes the Ethernet socket if available else the Serial socket and store it in buff.
    // If the has_crc arg is true, the two last bytes are checked as CRC16
    // Return the number of bytes successfully read.
    size_t read(uint8_t* buff, size_t n, bool has_crc = false)
    {
      int n_r = 0;
#ifdef TEENSY_4 // only available on teensy 4 // only available on teensy 4
      if (isEthernet)
        n_r = m_client.read(buff, n);
      else
#endif
        n_r = Serial.readBytes((char*)buff, n);
      if (n_r != n)
        return 0;
      if (has_crc && CRC(buff, n - 2) != *(uint16_t *)(buff + n - 2))
        return 0;
      return n_r;

    }

    // Write n byte from buff on the Ethernet socket if available else the Serial socket.
    // If the add_crc arg is true, two more CRC16 bytes are written after the n bytes from buff.
    // Return the number of bytes successfully written (crc16  included).
    size_t write(uint8_t* buff, size_t n, bool add_crc = false)
    {
      *(uint16_t *)(buff + n) = CRC(buff, n);
#ifdef TEENSY_4 // only available on teensy 4
      if (isEthernet)
        return m_client.write(buff, n + 2);
#endif
      return Serial.write((char*)buff, n + 2);
    }


    // Compute and return the CRC over the n first bytes of buf
    uint16_t CRC(uint8_t *buf, int n)
    {
      m_crc_accumulator = 0;
      for (int i = 0; i < n; i++) CRC_check(buf[i]);
      return (m_crc_accumulator >> 8) | (m_crc_accumulator << 8);
    }

  private :
    // Create a CRC lookup table to compute CRC16 fatser.
    // poly represent the coeficients use for the polynome of the CRC
    void mk_crctable(uint16_t poly = 0x1021)
    {
      for (int i = 0; i < 256; i++) m_crctable[i] = crchware(i, poly, 0);
    }

    // Generate the values for the CRC lookup table.
    uint16_t crchware(uint16_t data, uint16_t genpoly, uint16_t accum)
    {
      static int i;
      data <<= 8;
      for (i = 8; i > 0; i--)
      {
        if ((data ^ accum) & 0x8000)
          accum = (accum << 1) ^ genpoly;
        else
          accum <<= 1;
        data <<= 1;
      }
      return accum;
    }

    // Function use in CRC computation
    void CRC_check(uint8_t data)
    {
      m_crc_accumulator = (m_crc_accumulator << 8) ^ m_crctable[(m_crc_accumulator >> 8) ^ data];
    };

  private:
#ifdef TEENSY_4 // only available on teensy 4
    EthernetClient m_client;
    EthernetServer* m_server;
    bool isEthernet = false;
#endif
    bool isSerial = false;

    uint16_t m_crctable[256];
    uint16_t m_crc_accumulator;

};
