#include "ns3/core-module.h"
#include <iostream>

using namespace ns3;

class MM1QueueApp : public Application
{
public:
  static TypeId GetTypeId()
  {
    static TypeId tid = TypeId("MM1QueueApp")
      .SetParent<Application>()
      .AddConstructor<MM1QueueApp>();
    return tid;
  }

  void Setup(double arrivalRate, double serviceRate, uint32_t maxEvents)
  {
    m_maxEvents = maxEvents;
    m_queueLength = 0;
    m_serverBusy = false;
    m_arrivals = 0;
    m_departures = 0;
    m_arrivalRv = CreateObject<ExponentialRandomVariable>();
    m_serviceRv = CreateObject<ExponentialRandomVariable>();
    m_arrivalRv->SetAttribute("Mean", DoubleValue(1.0 / arrivalRate));
    m_serviceRv->SetAttribute("Mean", DoubleValue(1.0 / serviceRate));
  }

private:
  void StartApplication() override
  {
    std::cout << "t=" << Simulator::Now().GetSeconds() << "s queue start" << std::endl;
    ScheduleNextArrival();
  }

  void StopApplication() override
  {
    std::cout << "t=" << Simulator::Now().GetSeconds() << "s queue stop: arrivals="
              << m_arrivals << ", departures=" << m_departures << ", finalQueueLen="
              << m_queueLength << std::endl;
  }

  void ScheduleNextArrival()
  {
    double interArrival = m_arrivalRv->GetValue();
    Simulator::Schedule(Seconds(interArrival), &MM1QueueApp::Arrival, this);
  }

  void Arrival()
  {
    m_arrivals++;
    m_queueLength++;
    std::cout << "t=" << Simulator::Now().GetSeconds() << "s arrival, queueLen=" << m_queueLength << std::endl;

    if (!m_serverBusy)
    {
      m_serverBusy = true;
      ScheduleDeparture();
    }

    if (m_arrivals < m_maxEvents)
    {
      ScheduleNextArrival();
    }
  }

  void ScheduleDeparture()
  {
    double serviceTime = m_serviceRv->GetValue();
    Simulator::Schedule(Seconds(serviceTime), &MM1QueueApp::Departure, this);
  }

  void Departure()
  {
    if (m_queueLength > 0)
    {
      m_queueLength--;
      m_departures++;
    }

    std::cout << "t=" << Simulator::Now().GetSeconds() << "s departure, queueLen=" << m_queueLength << std::endl;

    if (m_queueLength > 0)
    {
      ScheduleDeparture();
    }
    else
    {
      m_serverBusy = false;
    }
  }

  uint32_t m_maxEvents = 20;
  uint32_t m_arrivals = 0;
  uint32_t m_departures = 0;
  int m_queueLength = 0;
  bool m_serverBusy = false;
  Ptr<ExponentialRandomVariable> m_arrivalRv;
  Ptr<ExponentialRandomVariable> m_serviceRv;
};

int main(int argc, char *argv[])
{
  CommandLine cmd;
  double arrivalRate = 2.0;
  double serviceRate = 3.0;
  uint32_t maxEvents = 20;

  cmd.AddValue("lambda", "Arrival rate in arrivals/sec", arrivalRate);
  cmd.AddValue("mu", "Service rate in departures/sec", serviceRate);
  cmd.AddValue("events", "Number of arrivals to simulate", maxEvents);
  cmd.Parse(argc, argv);

  NodeContainer nodes;
  nodes.Create(1);

  Ptr<MM1QueueApp> app = CreateObject<MM1QueueApp>();
  app->Setup(arrivalRate, serviceRate, maxEvents);
  nodes.Get(0)->AddApplication(app);
  app->SetStartTime(Seconds(0.0));
  app->SetStopTime(Seconds(50.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
