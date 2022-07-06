#include <SPI.h>

#define READ 0b10000000
#define WRITE 0b00000000
#define REG_MASK 0b01111111

//class used to unify the different functions used to interact with the EMG modules
class ClvHdEMG
{
  public:
    // Initialise the SPI bus and the selction pins
    ClvHdEMG()
    {
      SPI.begin();
      SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
      for (int i = 0; i < 4; i++)
        pinMode(m_addPins[i], OUTPUT);
      pinMode(m_gpio1Pin, OUTPUT);
      digitalWrite(m_gpio1Pin, HIGH);
      delay(1);

      this->m_nb_board = nb_emg_connected();

      // quick blink on available modules
      for (int j = 15; j > 15 - this->m_nb_board; j--)
        this->blink(j, 30, 5);
    };

    ~ClvHdEMG()
    {};

    //Select the module by activating the coreponding address pins.
    void selectBrd(uint8_t id)
    {
      for (unsigned i = 0; i < 4; i++)
        digitalWrite(m_addPins[ i] , (id >> i) & 1 );
    }

    // Read n bytes starting from the reg address of the module coreponding to id.
    // All values are store in the val buff
    void readRegister(byte reg, byte val[], unsigned n = 1, uint8_t id = 15, bool order = true)
    {
      byte dataToSend = READ | (REG_MASK & reg);

      selectBrd(id);
      SPI.transfer(dataToSend);
      if (order)
        for (unsigned i = 0; i < n; i++)
          *(val + i) = SPI.transfer(0x00);
      else
        for (unsigned i = n - 1; i >= 0; i--)
          *(val + i) = SPI.transfer(0x00);
      selectBrd(id - 1);
    }

    // Write 1 byte to the reg address of the module coreponding to id.
    // All values are store in the val buff
    void writeRegister(byte reg, byte val, uint8_t id = 15)
    {
      byte dataToSend = WRITE | (REG_MASK & reg);;
      selectBrd(id);
      SPI.transfer(dataToSend);
      SPI.transfer(val);
      selectBrd(id-1);
    }

    // Scan available modules.
    int nb_emg_connected()
    {
      m_nb_board = 0;
      byte val = 0;
      for (unsigned i = 0; i < 16; i++)
      {
        readRegister(0x40, &val, 1, i);
        if (val == 0x01)
          m_nb_board++;
      }
      return m_nb_board;
    }

    uint8_t get_nbBoard()
    {
      return m_nb_board;
    }

    // Blink the module coresponding to id n times for dt_ms miliseconds 
    void blink(int id, int dt_ms, int n)
    {
      selectBrd(id);
      for (int i = 0; i < n; i++)
      {
        digitalWrite(m_gpio1Pin, i % 2);
        delay(dt_ms);
      }
      digitalWrite(m_gpio1Pin, LOW);
    }

    
  private:
    const int m_addPins[4] = {7, 8, 9, 10};
    const int m_gpio1Pin = 6;
    const int m_resetPin = 14;
    int m_nb_board = 0;

};
