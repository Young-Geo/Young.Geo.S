using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Sprite.Server
{
    public class pkt
    {
        protected const short HEADPACKAGELEN = 6;

        public const byte PKT_START_TAG = 0xA5;
        public const byte PKT_FRAM_TAG = 0xB5;
        public const byte PKT_END_TAG = 0x5A;

        /// <summary>
        /// 创建一个buffer
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="bufferLength"></param>
        /// <returns></returns>
        public static byte[] build(ref byte[] buffer, int bufferLength)
        {

            int len = 0, i = 0;
            byte[] res = null;
            byte[] a = new byte[2];

            if (null == buffer || bufferLength <= 0) {
                Debug.Log("buffer为空");
                return null;
            }

            len = bufferLength + HEADPACKAGELEN;
            if (null == (res = new byte[len])) {
                Debug.Log("创建错误");
                return null;
            }

            res[i++] = PKT_START_TAG;
            res[i++] = 0;
            res[i++] = PKT_FRAM_TAG;
            Out16(ref a, (short)len);
            res[i++] = a[0];
            res[i++] = a[1];

            for (int j = 0; j < bufferLength; j++)
            {
                res[i++] = buffer[j];

            }
            res[i] = PKT_END_TAG;
            res[1] = pkt_build_check_sum(res, len);

            return res;
        }
        public static int pkt_match_head(byte []buf, int len, byte tag)
        {
            int i = 0;
            if (null == buf)
            {
                return -1;
            }
            for (i = 0; i < len; ++i)
            {
                if (buf[i] == tag)
                {
                    return i;
                }
            }
            return -1;
        }

        /// <summary>
        /// 回来的包
        /// </summary>
        /// <param name="data"></param>
        /// <param name="data_size"></param>
        /// <param name="pkt"></param>
        /// <param name="pkt_size"></param>
        /// <returns></returns>
        public static bool pkt_parsed_data(ref byte []data, ref int data_size, byte []pkt, int pkt_size)
        {
            int i = 0;
            byte xor = 0;
            short fram_size = 0;

            if (0 > (i = pkt_match_head(pkt, pkt_size, PKT_START_TAG))) {
                Debug.Log("pkt_parsed_data pkt_match_head error");
                return false;
            }
            ++i;
            xor = pkt[i];
            pkt[i++] = 0;
            if (false == pkt_check_sum(pkt, pkt_size, xor))
            {
                Debug.Log("pkt_parsed_data pkt_check_sum error");
                return false;
            }

            if (PKT_FRAM_TAG != pkt[i++]) {
                Debug.Log("pkt_parsed_data PKT_FRAM_TAG error");
                return false;
            }
            byte[] in16 = new byte[2];
            in16[0] = pkt[i++];
            in16[1] = pkt[i++];
            In16(in16, ref fram_size);
            data_size = fram_size - 6;
            data = new byte[data_size];
            Array.Copy(pkt, i, data, 0, data_size);
            return true;
        }
        public static byte pkt_build_check_sum(byte[] result, int size)
        {
            int i;
            byte sum = 0;

            for (i = 0; i < size; i++)
            {
                sum ^= result[i];
            }

            return sum;
        }

        public static bool pkt_check_sum(byte[] result, int size, byte tag)
        {
            int i;
            byte sum = 0;

            for (i = 0; i < size; i++)
            {
                sum ^= result[i];
            }

            if (tag != sum)
            {
                return false;
            }
            return true;
        }


        public static bool Out16(ref byte []buf, short v)
        {
            if (null == buf)
            {
                Debug.Log("out16 v值为空");
                return false;
            }
            buf[0] = (byte)((v & 0xff00) >> 8);
            buf[1] = (byte)(v & 0x00ff);
            return true;
        }

        public static bool In16(byte[] buf, ref short v)
        {
            if (null == buf)
            {
                Debug.Log("In16 v值为空");
                return false;
            }
            v = (byte)(buf[0]);
            v = (short)((v << 8) | (buf[1]));
            return true;
        }

    }
}
