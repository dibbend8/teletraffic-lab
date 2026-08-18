#include "ns3/core-module.h"
#include <iostream>

using namespace ns3;

class BinarySymmetricChannel
{
public:
  BinarySymmetricChannel(double flipProbability, uint32_t steps)
    : m_flipProbability(flipProbability),
      m_steps(steps),
      m_state(0),
      m_stepCount(0)
  {
    m_uniform = CreateObject<UniformRandomVariable>();
    m_uniform->SetStream(7);
  }

  void Start()
  {
    std::cout << "t=" << Simulator::Now().GetSeconds() << "s start state=" << m_state << std::endl;
    Simulator::Schedule(Seconds(1.0), &BinarySymmetricChannel::Step, this);
  }

  void Step()
  {
    if (m_stepCount >= m_steps)
    {
      std::cout << "t=" << Simulator::Now().GetSeconds() << "s finished" << std::endl;
      return;
    }

    double r = m_uniform->GetValue();
    if (r < m_flipProbability)
    {
      m_state = 1 - m_state;
    }

    std::cout << "t=" << Simulator::Now().GetSeconds() << "s state=" << m_state << std::endl;
    m_stepCount++;

    if (m_stepCount < m_steps)
    {
      Simulator::Schedule(Seconds(1.0), &BinarySymmetricChannel::Step, this);
    }
  }

private:
  double m_flipProbability;
  uint32_t m_steps;
  int m_state;
  uint32_t m_stepCount;
  Ptr<UniformRandomVariable> m_uniform;
};

int main(int argc, char *argv[])
{
  CommandLine cmd;
  double flipProbability = 0.2;
  uint32_t steps = 10;
  cmd.AddValue("p", "Bit-flip probability per step", flipProbability);
  cmd.AddValue("steps", "Number of steps to simulate", steps);
  cmd.Parse(argc, argv);

  RngSeedManager::SetSeed(1);
  RngSeedManager::SetRun(5);

  BinarySymmetricChannel channel(flipProbability, steps);
  Simulator::Schedule(Seconds(0.0), &BinarySymmetricChannel::Start, &channel);
  Simulator::Stop(Seconds(steps + 0.5));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
