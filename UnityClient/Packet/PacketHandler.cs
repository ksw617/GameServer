using Google.Protobuf;
using Google.Protobuf.Protocol;
using UnityClient.Network;

namespace UnityClient.Packet
{
    public class PacketHandler
    {
        public static void S_LOGIN_Handler(PacketSession session, IMessage packet)
        {
            Console.WriteLine("Connected");

            S_Login s_Login = packet as S_Login;

            C_EnterGame sendPacket = new C_EnterGame();

            Player player = new Player();

            player.Id = s_Login.PlayerID;
            player.Name = "TEST";

            sendPacket.Player = player;

            ServerSession serverSession = session as ServerSession;
            serverSession.Send(sendPacket);


        }

        public static void S_ENTER_GAME_Handler(PacketSession session, IMessage packet)
        {
            Console.WriteLine("S_ENTER_GAME_Handler");
        }

        public static void S_JOIN_GAME_Handler(PacketSession session, IMessage packet)
        {
            Console.WriteLine("S_JOIN_GAME_Handler");
        }

        public static void S_CHAT_Handler(PacketSession session, IMessage packet)
        {
            Console.WriteLine("S_CHAT_Handler");

        }

    }
}
