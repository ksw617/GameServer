using System.Net.Sockets;
using System.Net;

namespace UnityClient.Network
{
    public class ClientService
    {
        Func<Session>? sessionFactory;

        public void Connect(IPEndPoint endPoint, Func<Session> sessionFactory, int count = 1)
        {
            for (int i = 0; i < count; i++)
            {
                Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                this.sessionFactory = sessionFactory;

                SocketAsyncEventArgs args = new();
                args.Completed += OnConnectCompleted;
                args.RemoteEndPoint = endPoint;
                args.UserToken = socket;

                RegisterConnect(args);
            }
        }

        void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket? socket = args.UserToken as Socket;
            if (socket == null)
                return;

            bool pending = socket.ConnectAsync(args);
            if (pending == false)
                OnConnectCompleted(null, args);
        }

        void OnConnectCompleted(object? sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success)
            {
                if (sessionFactory != null)
                {
                    Session session = sessionFactory.Invoke();
                    if (args.ConnectSocket != null)
                    {
                        session.Connect(args.ConnectSocket);
                        if (args.RemoteEndPoint != null)
                        {
                            session.OnConnected(args.RemoteEndPoint);
                        }
                    }  
                }
            }
            else
            {
                Console.WriteLine($"OnConnectCompleted Fail: {args.SocketError}");
            }
        }
    }
}
