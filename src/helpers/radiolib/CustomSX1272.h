#pragma once

#include <RadioLib.h>

class CustomSX1272 : public SX1272 {
  public:
    CustomSX1272(Module *mod) : SX1272(mod) { }

  #ifdef RP2040_PLATFORM
    bool std_init(SPIClassRP2040* spi = NULL)
  #else
    bool std_init(SPIClass* spi = NULL)
  #endif
    {
  #ifdef LORA_CR
      uint8_t cr = LORA_CR;
  #else
      uint8_t cr = 5;
  #endif

  #if defined(P_LORA_SCLK)
    #ifdef NRF52_PLATFORM
      if (spi) { spi->setPins(P_LORA_MISO, P_LORA_SCLK, P_LORA_MOSI); spi->begin(); }
    #elif defined(RP2040_PLATFORM)
      if (spi) {
        spi->setMISO(P_LORA_MISO);
        spi->setSCK(P_LORA_SCLK);
        spi->setMOSI(P_LORA_MOSI);
        spi->begin();
      }
    #else
      if (spi) spi->begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);
    #endif
  #endif
      int status = begin(LORA_FREQ, LORA_BW, LORA_SF, cr, 0x12, LORA_TX_POWER, 16);
      if (status != RADIOLIB_ERR_NONE) {
        Serial.print("ERROR: radio init failed: ");
        Serial.println(status);
        return false;
      }
  #ifdef SX127X_CURRENT_LIMIT
      setCurrentLimit(SX127X_CURRENT_LIMIT);
  #endif

  #if defined(SX176X_RXEN) || defined(SX176X_TXEN)
    #ifndef SX176X_RXEN
      #define SX176X_RXEN RADIOLIB_NC
    #endif
    #ifndef SX176X_TXEN
      #define SX176X_TXEN RADIOLIB_NC
    #endif
      setRfSwitchPins(SX176X_RXEN, SX176X_TXEN);
  #endif

      setCRC(1);

      return true;
    }

    bool isReceiving() {
      return (getModemStatus() & 0x0F) != 0;
    }

    int tryScanChannel() {
      int16_t state = startChannelScan();
      RADIOLIB_ASSERT(state);

      unsigned long timeout = millis() + 16;
      while(!this->mod->hal->digitalRead(this->mod->getIrq()) && millis() < timeout) {
        this->mod->hal->yield();
        if(this->mod->hal->digitalRead(this->mod->getGpio())) {
          return(RADIOLIB_PREAMBLE_DETECTED);
        }
      }
      return 0;
    }
};
