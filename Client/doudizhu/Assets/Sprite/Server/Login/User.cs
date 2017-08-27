using Assets.Sprite.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Sprite
{
    public class User
    {
        private string username;
        private string password;
        private int inx = 0;

        #region 封装字段

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

        public User(string username, string password)
        {
            this.username = username;
            this.password = password;
        }
        public User(){}
        public bool login()
        {
            int i = 0;
            byte[] usernameByte = System.Text.Encoding.Default.GetBytes(Username);
            byte[] passwordByte = System.Text.Encoding.Default.GetBytes(Password);
            byte[] user = new byte[(int)WORKTYPE_LEN.USERNAME_LEN];
            byte[] pass = new byte[(int)WORKTYPE_LEN.PASSWORD_LEN];
            Array.Clear(user, 0, user.Length);
            Array.Clear(pass, 0, pass.Length);

            for (int j = 0; j < usernameByte.Length; j++)
            {
                user[j] = usernameByte[j];
            }
            for (int j = 0; j < passwordByte.Length; j++)
            {
                pass[j] = passwordByte[j];
            }
            int len = (int)WORKTYPE_LEN.USERNAME_LEN + (int)WORKTYPE_LEN.PASSWORD_LEN + (int)WORKTYPE_LEN.TYPE_LEN;
            byte[] buf = new byte[len];
            short type = (short)WORK.LOGIN;
            buf[i++] = (byte)(type & 0x00ff);
            buf[i++] = (byte)((type & 0xff00) >> 8);
            for (int j = 0; j < (int)WORKTYPE_LEN.USERNAME_LEN; j++)
            {
                buf[i++] = user[j];
            }
            for (int j = 0; j < (int)WORKTYPE_LEN.PASSWORD_LEN; j++)
            {
                buf[i++] = pass[j];
            }
            byte [] sen_buf = pkt.build(ref buf, len);
            int sen_num = ClientSocket.send_buf(sen_buf);
            if (sen_num <= 0) {
                //错误日志
                return false;
            }
            //return rec_login();
            return true;
        }

        public bool rec_login()
        {
            //rec
            byte[] rec = null;
            int len = 0, i = 0;
            short work_tag = 0;

            len = ClientSocket.rec_buf(ref rec);
            if (len <= 2) {
                //error
                return false;
            }

            work_tag = (rec[1]);
            work_tag = (short)((work_tag << 8) | rec[0]);
            //pkt.In16(rec, ref work_tag);
            if (work_tag != (short)WORK.REC_LOGIN) {
                //error
                return false;
            }
            i += 2;
            this.inx = rec[i];
            if (0 == inx) {
                //error
                return false;
            }
            return true;
        }
        
    }
}
