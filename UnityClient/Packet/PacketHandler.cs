using Google.Protobuf;
using UnityClient.Network;

namespace UnityClient.Packet
{
    public class PacketHandler
    {
        public static void S_LOGIN_Handler(PacketSession session, IMessage packet)
        {
            Console.WriteLine("S_LOGIN_Handler");
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
