
using Google.Protobuf;
using System.Net;
using UnityClient.Network;
using UnityClient.Packet;


Thread.Sleep(1000);

ServerSession session = new();

//string host = Dns.GetHostName();
//IPHostEntry ipHost = Dns.GetHostEntry(host);
//IPAddress ipAddr = ipHost.AddressList[0];
IPAddress ipAddr = IPAddress.Parse("127.0.0.1");
IPEndPoint endPoint = new IPEndPoint(ipAddr, 27015);

ClientService service = new ClientService();
service.Connect(endPoint, () => { return session; }, 1);

while (true)
{
    List<PacketMessage> list = PacketQueue.Instance.PopAll();
    foreach (PacketMessage packet in list)
    {
        Action<PacketSession, IMessage>? handler = ClientPacketManager.Instance.GetPacketHandler(packet.Id);
        if (handler != null && packet.Message != null)
            handler.Invoke(session, packet.Message);
    }
}