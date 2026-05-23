#pragma once

#include "CustomSX1272.h"
#include "RadioLibWrappers.h"

#ifndef USE_SX1272
#define USE_SX1272
#endif

class CustomSX1272Wrapper : public RadioLibWrapper {
public:
  CustomSX1272Wrapper(CustomSX1272& radio, mesh::MainBoard& board) : RadioLibWrapper(radio, board) { }
  bool isReceivingPacket() override { 
    return ((CustomSX1272 *)_radio)->isReceiving();
  }
  float getCurrentRSSI() override {
    return ((CustomSX1272 *)_radio)->getRSSI(false);
  }
  float getLastRSSI() const override { return ((CustomSX1272 *)_radio)->getRSSI(); }
  float getLastSNR() const override { return ((CustomSX1272 *)_radio)->getSNR(); }

  float packetScore(float snr, int packet_len) override {
    int sf = ((CustomSX1272 *)_radio)->spreadingFactor;
    return packetScoreInt(snr, sf, packet_len);
  }
};
