using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Sprite.Server.Register
{
    public class Register
    {
        private string username;
        private string password;
        private int inx;

        #region 封装字段
        public string Username
        {
            get
            {
                return username;
            }

            set
            {
                username = value;
            }
        }

        public string Password
        {
            get
            {
                return password;
            }

            set
            {
                password = value;
            }
        }

        public int Inx
        {
            get
            {
                return inx;
            }

            set
            {
                inx = value;
            }
        }

        #endregion

        public Register(string username, string password)
        {
            this.username = username;
            this.password = password;

        }

        public Register() { }

        public bool Registers()
        {
            int i = 0;
            byte[] usernameByte = System.Text.Encoding.Default.GetBytes(Username);
            byte[] passwordByte = System.Text.Encoding.Default.GetBytes(Password);
            byte[] username = new byte[(int)WORKTYPE_LEN.USERNAME_LEN];
            byte[] password = new byte[(int)WORKTYPE_LEN.PASSWORD_LEN];
            Array.Clear(username, 0, username.Length);
            Array.Clear(password, 0, password.Length);
            for (int j = 0; j < usernameByte.Length; j++)
            {
                username[j] = usernameByte[j];
            }
            for (int j = 0; j < passwordByte.Length; j++)
            {
                password[j] = passwordByte[j];
            }
            int len = (int)WORKTYPE_LEN.USERNAME_LEN + (int)WORKTYPE_LEN.PASSWORD_LEN + (int)WORKTYPE_LEN.TYPE_LEN;
            byte[] buf = new byte[len];
            short type = (short)WORK.REGISTER;
            buf[i++] = (byte)(type & 0x00ff);
            buf[i++] = (byte)((type & 0xff00) >> 8);
            for (int j = 0; j < (int)WORKTYPE_LEN.USERNAME_LEN; j++)
            {
                buf[i++] = username[j];
            }
            for (int j = 0; j < (int)WORKTYPE_LEN.PASSWORD_LEN; j++)
            {
                buf[i++] = password[j];
            }
            byte[] sen_buf = pkt.build(ref buf, len);
            int sem_num = ClientSocket.send_buf(sen_buf);
            if (sem_num <= 0)
            {
                Debug.Log("sem_num <= 0 ");
                return false;
            }
            return true;
        }

        public bool rec_Register()
        {
            byte[] rec = null;
            int len = 0, i = 0;
            short work_tag = 0;

            len = ClientSocket.rec_buf(ref rec);
            if (len <= 2)
            {
                return false;
            }

            work_tag = (rec[1]);
            work_tag = (short)((work_tag << 8) | rec[0]);

            if (work_tag != (short)WORK.REC_REGISTER)
            {
                return false;
            }
            i += 2;
            this.inx = rec[i];
            if (0 == inx)
            {
                return false;
            }
            return true;
        }
    }
}
