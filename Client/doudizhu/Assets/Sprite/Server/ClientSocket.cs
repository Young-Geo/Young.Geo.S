using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using UnityEngine;
using System.Collections;
using System.IO;
using Assets.Sprite.Server;

namespace Assets
{
    public class ClientSocket
    {
        private static  Socket clientSocket = null;

        //是否连接标识
        public static bool IsConnected = false;
        //private ClientSocket client = null;
        public delegate int do_work(byte []buf, int buf_len);
        private static do_work work;
        private static byte[] buffer = new byte[1024];

        public static do_work Work
        {
            get
            {
                return work;
            }

            set
            {
                work = value;
            }
        }

        public static Socket clientsocket
        {
            get
            {
                return clientSocket;
            }

            set
            {
                clientSocket = value;
            }
        }


        public static byte[] Buffer
        {
            get
            {
                return buffer;
            }

            set
            {
                buffer = value;
            }
        }

        
        private ClientSocket()
        {
            if (null == clientSocket)
                clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            //if (null == client)
            //    client = new ClientSocket();
        }


        /// <summary>
        /// 连接指定Ip和端口的服务器
        /// </summary>
        /// <param name="ip"></param>
        /// <param name="port"></param>
        public static void ConnectServer(string ip, int port)
        {
            if (null == clientSocket)
                clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPAddress mIp = IPAddress.Parse(ip);
            IPEndPoint ip_end_point = new IPEndPoint(mIp, port);

            try
            {
                clientSocket.Connect(ip_end_point);
                IsConnected = true;
                Debug.Log("连接服务器成功");
                //clientSocket.BeginReceive(Buffer, 0, Buffer.Length, SocketFlags.None, new AsyncCallback(ReceiveMessage), clientSocket);
            }
            catch (Exception)
            {
                IsConnected = false;
                Debug.Log("连接失败");
                
            }
        }

        /// <summary>
        /// 发送数据给服务器
        /// </summary>
        /// <param name="data"></param>
        public void SendMessage(string data)
        {
            if (IsConnected == false)
                return;
            try
            {
                
            }
            catch
            {

                IsConnected = false;
                clientSocket.Shutdown(SocketShutdown.Both);
                clientSocket.Close();
            }
        }

        public static int send_buf(byte[] message)
        {
            return clientSocket.Send(WriteMessage(message));
        }

        public static void ReceiveMessage(IAsyncResult ar)
        {
            byte[] buf = null;
            int len = 0;
            try
            {
                var socket = ar.AsyncState as ClientSocket;

                var length = ClientSocket.clientSocket.EndReceive(ar);
                //读取出来消息内容  
                //var message = Encoding.ASCII.GetString(buffer, 0, length);

                pkt.pkt_parsed_data(ref buf, ref len, Buffer, length);
                if (null != work)
                    work(buf, len);

                //接收下一个消息(因为这是一个递归的调用，所以这样就可以一直接收消息了）  
                clientSocket.BeginReceive(Buffer, 0, Buffer.Length, SocketFlags.None, new AsyncCallback(ReceiveMessage), clientSocket);
            }
            catch (Exception ex)
            {
                Debug.Log("rec error " + ex.Message);
            }
        }
        public static int rec_buf(ref byte []buf)
        {
            int len = 0, re = 0;
            byte[] pkt_buf = null;
            len = 1024;
            pkt_buf = new byte[len];
            clientSocket.Receive(pkt_buf);
            pkt.pkt_parsed_data(ref buf, ref re,pkt_buf, 9);
            return re;
        }
        public static byte[] WriteMessage(byte[] message)
        {
            MemoryStream ms = null;
            using (ms = new MemoryStream())
            {
                ms.Position = 0;
                BinaryWriter writer = new BinaryWriter(ms);
                ushort msglen = (ushort)message.Length;
                writer.Write(msglen);
                writer.Write(message);
                writer.Flush();
                return ms.ToArray();
            }
        }

    }
}
