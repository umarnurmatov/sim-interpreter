#include "base_interp_executor.hpp"

class InterpExecutor : public BaseInterpExecutor
{
public:
  enum Mode {
    kLinear,
    kCached
  };

  InterpExecutor(Mode mode = kCached)
    : m_mode{ mode }
  {}

  int run() override;

private:

  Mode m_mode;
};


