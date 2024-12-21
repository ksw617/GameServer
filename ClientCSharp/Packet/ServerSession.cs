using ClientCSharp.Network;
using Google.Protobuf;
using Protocol;
using System.Net;

namespace ClientCSharp.Packet
{
    internal class ServerSession : PacketSession
    {
        public void Send(IMessage packet)
        {
            string msgName = packet.Descriptor.Name.Replace("_", string.Empty);
            PacketID packetID = (PacketID)Enum.Parse(typeof(PacketID), msgName, true);
            ushort size = (ushort)packet.CalculateSize();
            byte[] sendBuffer = new byte[size + 4];
            Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
            Array.Copy(BitConverter.GetBytes((ushort)packetID), 0, sendBuffer, 2, sizeof(ushort));
            Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);
            Send(new ArraySegment<byte>(sendBuffer));
        }

        public override void OnConnected(EndPoint endPoint)
        {
            Console.WriteLine("OnConnected");

            LoginRequest packet = new LoginRequest
            {
                UserId = "player1",
                Token = "debug-token-12345"
            };

            // C++와 동일하게 PacketHandler를 통해 직렬화
            var sendBuffer = ClientPacketHandler.MakeSendBuffer(packet);
            Send(sendBuffer);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine("OnDisconnected");
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            Console.WriteLine("OnRecvPacket");
            ClientPacketHandler.Instance.HandlePacket(this, buffer);
        }

        public override void OnSend(int numOfBytes)
        {
            Console.WriteLine("OnSend");
        }

    }
}
