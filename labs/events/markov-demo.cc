// Simple discrete-time Markov-chain demo using ns-3 event scheduling.
// This file is focused on event-driven state updates with no sockets or packets.

#include "ns3/core-module.h"
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;

class MarkovApp : public Application
{
public:
  static TypeId GetTypeId()
  {
    static TypeId tid = TypeId("MarkovApp")
      .SetParent<Application>()
      .AddConstructor<MarkovApp>();
    return tid;
  }

  void Setup(uint32_t steps, double stepInterval, std::string chainType, std::string outPrefix, uint32_t targetState)
  {
    m_steps = steps;
    m_stepInterval = Seconds(stepInterval);
    m_chainType = chainType;
    m_outPrefix = outPrefix;
    m_targetState = targetState;
    ConfigureChain();
  }

private:
  void ConfigureChain()
  {
    if (m_chainType == "three_state")
    {
      m_P = {
        {0.0, 0.5, 0.5},
        {0.5, 0.0, 0.5},
        {0.5, 0.5, 0.0}
      };
    }
    else if (m_chainType == "absorbing")
    {
      m_P = {
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0},
        {0.0, 0.0, 0.0, 1.0}
      };
    }
    else
    {
      m_P = {{0.3, 0.7}, {0.6, 0.4}};
    }
    m_numStates = m_P.size();
  }

  void StartApplication() override
  {
    m_uniform = CreateObject<UniformRandomVariable>();
    m_currentState = 0;
    m_stateSeq.reserve(m_steps + 1);
    m_stateSeq.push_back(m_currentState);
    m_hitRecorded = false;
    m_stepCount = 0;
    Simulator::Schedule(Seconds(0.0), &MarkovApp::Step, this);
  }

  void StopApplication() override
  {
    std::ofstream ofs(m_outPrefix + "_seq.txt");
    double t = 0.0;
    for (uint32_t i = 0; i < m_stateSeq.size(); ++i)
    {
      ofs << t << " " << m_stateSeq[i] << "\n";
      t += m_stepInterval.GetSeconds();
    }
    ofs.close();
  }

  void Step()
  {
    if (m_stepCount >= m_steps)
    {
      Simulator::Stop();
      return;
    }

    double u = m_uniform->GetValue(0.0, 1.0);
    double c = 0.0;
    uint32_t next = m_currentState;
    for (uint32_t j = 0; j < m_numStates; ++j)
    {
      c += m_P[m_currentState][j];
      if (u <= c)
      {
        next = j;
        break;
      }
    }

    m_currentState = next;
    m_stateSeq.push_back(m_currentState);
    m_stepCount++;

    if (!m_hitRecorded && m_currentState == m_targetState)
    {
      m_hitRecorded = true;
      m_hittingTimes.push_back(m_stepCount);
    }

    Simulator::Schedule(m_stepInterval, &MarkovApp::Step, this);
  }

  Ptr<UniformRandomVariable> m_uniform;
  std::vector<std::vector<double>> m_P;
  uint32_t m_numStates = 0;
  uint32_t m_currentState = 0;
  std::vector<uint32_t> m_stateSeq;
  uint32_t m_steps = 1000;
  Time m_stepInterval = Seconds(0.001);
  uint32_t m_stepCount = 0;
  std::string m_chainType = "three_state";
  std::string m_outPrefix = "markov-out";
  uint32_t m_targetState = 0;
  bool m_hitRecorded = false;
  std::vector<uint32_t> m_hittingTimes;
};

int main(int argc, char *argv[])
{
  CommandLine cmd;
  uint32_t steps = 10000;
  double stepInterval = 0.001;
  std::string chainType = "three_state";
  std::string outPrefix = "markov-out";
  uint32_t targetState = 1;
  uint32_t runIndex = 1;

  cmd.AddValue("steps", "Number of discrete steps to simulate", steps);
  cmd.AddValue("stepInterval", "Seconds between steps", stepInterval);
  cmd.AddValue("chainType", "Type of chain: three_state | absorbing | twostate", chainType);
  cmd.AddValue("outPrefix", "Output filename prefix", outPrefix);
  cmd.AddValue("targetState", "Target state index for hitting time", targetState);
  cmd.AddValue("run", "RNG run index", runIndex);
  cmd.Parse(argc, argv);

  RngSeedManager::SetRun(runIndex);

  Ptr<MarkovApp> app = CreateObject<MarkovApp>();
  app->Setup(steps, stepInterval, chainType, outPrefix, targetState);

  NodeContainer nodes;
  nodes.Create(1);
  nodes.Get(0)->AddApplication(app);
  app->SetStartTime(Seconds(0.0));
  app->SetStopTime(Seconds(steps * stepInterval + 0.1));

  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
