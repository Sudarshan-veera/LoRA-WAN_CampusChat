#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LoraMasterSlave");

uint32_t packetsSent = 0;
uint32_t packetsReceived = 0;
double totalDelay = 0;

std::map<uint32_t, Time> sendTime;

double distanceArray[3] = {20, 30, 50};

double
GetPropagationDelay(double distance)
{
    return distance / 300000000.0; // speed of light
}

void
SendAck(uint32_t slaveId)
{
    packetsReceived++;

    Time delay = Simulator::Now() - sendTime[slaveId];
    totalDelay += delay.GetSeconds();

    std::cout << "ACK received by Slave "
              << slaveId
              << " at "
              << Simulator::Now().GetSeconds()
              << " s"
              << std::endl;
}

void
MasterReceive(uint32_t slaveId, double distance)
{
    std::cout << "Master received HELLO from Slave "
              << slaveId
              << " at "
              << Simulator::Now().GetSeconds()
              << " s"
              << std::endl;

    Simulator::Schedule(Seconds(5), &SendAck, slaveId);
}

void
SlaveSend(uint32_t slaveId, double distance)
{
    packetsSent++;

    sendTime[slaveId] = Simulator::Now();

    std::cout << "Slave "
              << slaveId
              << " sent HELLO at "
              << Simulator::Now().GetSeconds()
              << " s (distance "
              << distance
              << " m)"
              << std::endl;

    double propDelay = GetPropagationDelay(distance);

    Simulator::Schedule(Seconds(propDelay),
                        &MasterReceive,
                        slaveId,
                        distance);
}

int
main(int argc, char *argv[])
{
    std::cout << "LoRa Master-Slave Chat Simulation\n";

    for (int i = 0; i < 3; i++)
    {
        double distance = distanceArray[i];

        std::cout << "\n---- Distance Test: "
                  << distance
                  << " meters ----\n";

        Simulator::Schedule(Seconds(1),
                            &SlaveSend,
                            1,
                            distance);

        Simulator::Schedule(Seconds(2),
                            &SlaveSend,
                            2,
                            distance);
    }

    Simulator::Stop(Seconds(30));

    Simulator::Run();

    double packetLoss =
        ((packetsSent - packetsReceived) * 100.0) / packetsSent;

    double avgDelay = totalDelay / packetsReceived;

    double powerTx = packetsSent * 0.05;   // estimated power
    double powerRx = packetsReceived * 0.02;

    std::cout << "\n===== Simulation Results =====\n";

    std::cout << "Packets Sent: "
              << packetsSent
              << std::endl;

    std::cout << "Packets Received: "
              << packetsReceived
              << std::endl;

    std::cout << "Packet Loss (%): "
              << packetLoss
              << std::endl;

    std::cout << "Average Delay: "
              << avgDelay
              << " sec"
              << std::endl;

    std::cout << "Estimated TX Power: "
              << powerTx
              << " W"
              << std::endl;

    std::cout << "Estimated RX Power: "
              << powerRx
              << " W"
              << std::endl;

    Simulator::Destroy();
}
