#pragma once
#include "cmodel_cmdbuf.h"

class CModelCmdbufphobos3 : public CModelCmdbuf {
public:
  CModelCmdbufphobos3();
  ~CModelCmdbufphobos3() override;

  virtual bmerr_t rt_device_open(int index, bmdev_t *dev) override;
protected:
  virtual void enable_interrupt(uint8_t *cmdbuf, size_t sz) override;
  virtual void set_eod(uint8_t *cmdbuf, uint64_t sz) override;
};
