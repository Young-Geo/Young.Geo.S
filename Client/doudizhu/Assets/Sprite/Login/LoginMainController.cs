using Assets.Sprite;
using Assets.Sprite.Server.Register;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;
using Assets;
using System.Net.Sockets;
using System;
using System.Text;
using System.Security.Cryptography;

public class LoginMainController : MonoBehaviour {

    public TweenScale loginPanelItween;      //主登录页面
    public TweenScale loginFastPanelItween;  //快速登录页面
    public TweenScale RegisterPanelItween;   //快速登录—注册页面
    public TweenScale loginPhonePanelItween; //手机登录页面

    //public static Hashtable uiht = null;
    //public static Hashtable dpht = null;
    public static Queue<string> uiqueue = null;
    public static Queue<string> dpqueue = null;

    public static Thread tDP;

    private static readonly object lockui = new object();
    private static readonly object lockdp = new object();


    public UIInput loginUsernameInput;
    public UIInput loginPasswordInput;

    public UIInput registerUsernameInput;
    //注册页面的密码
    public UIInput registerPasswordsInput;
    public UIInput registerPasswordsAgainInput;

    //界面显示的已经登录的账号
    public UILabel interfaceUsernameLabel;
    //注册页面两次输入的密码不相同提示
    public UILabel errorpassword;   //注册页面两次输入的密码不相同提示


    void Start()
    {

        if (null == tDP)
        {
            //加锁
            lock (lockui)
            {
                if (null == tDP)
                {
                    tDP = new Thread(tdp);
                }
                //解锁
            }
        }
        tDP.Start();

        /*
        if (null == uiht)
        {
            //加锁
            lock (lockui)
            {
                if (null == uiht)
                {
                    uiht = new Hashtable();
                }
                //解锁
            }
        }
        if (null == dpht)
        {
            //加锁
            lock (lockdp)
            {
                if (null == dpht)
                {
                    dpht = new Hashtable();
                }
                //解锁
            }
        }
        */
        if (null == uiqueue)
        {
            //加锁
            lock (lockui)
            {
                if (null == uiqueue)
                {
                    uiqueue = new Queue<string>();
                }
                //解锁
            }
        }
        if (null == dpqueue)
        {
            //加锁
            lock (lockdp)
            {
                if (null == dpqueue)
                {
                    dpqueue = new Queue<string>();
                }
                //解锁
            }
        }
        //ClientSocket.Work = work;
        //必须保证只创建一个tDP线程和一个ht，后续用单例模式保证
    }

    private void Update()
    {
        for (int i = 0; i < 10000000; i++)
        {
            Debug.Log(i);
        }
        string s = null;
        while (uiqueue.Count > 0)
        {
            lock (lockui)
            {
            s = uiqueue.Dequeue();
            }
            if (null != s)
            {
                string []strs = s.Split(':');
                if (null == strs) continue;

                switch (strs[0])
                {
                    case "login":
                        {
                            //value;
                            //登录成功切换相应界面
                            if ("登录成功".Equals(strs[1]))
                            {       
                                //返回初始登录界面
                                loginFastPanelItween.PlayReverse();
                                StartCoroutine(HidePanel(loginFastPanelItween.gameObject));
                                loginPanelItween.gameObject.SetActive(true);
                                loginPanelItween.PlayReverse();
                                interfaceUsernameLabel.text = username;
                                //跳转到主界面
                                AsyncOperation opreration = Application.LoadLevelAsync(2);
                                LoadScenceProgressBar._instance.Show(opreration);

                            }
                            else
                            {
                                Debug.Log("登录失败");

                            }

                        }
                        break;
                    case "Register":
                        {
                            if ("注册成功".Equals(strs[1]))
                            {
                                //返回到初始登录页面
                                RegisterPanelItween.PlayReverse();
                                StartCoroutine(HidePanel(RegisterPanelItween.gameObject));
                                loginFastPanelItween.gameObject.SetActive(true);
                                loginFastPanelItween.PlayReverse();
                                Onlogin_agin(register_username,register_password);


                            }
                            else
                            {
                                Debug.Log("注册失败");
                            }
                        }
                        break;
                    case "showuserinfo":
                        {
                            byte[] buff = System.Text.Encoding.ASCII.GetBytes(strs[1]);
                            int i = 0;
                            int id = 0;
                            int user_status = 0;
                            int money_z = 0;
                            int money_d = 0;
                            int solo_w = 0;
                            int solo_f = 0;
                            short solo_s = 0;
                            short lv = 0;

                            id = BitConverter.ToInt32(buff, i);
                            FaceMainController.set_id(id);

                            user_status = buff[ i += 4];
                            FaceMainController.set_user_status(user_status);
                            Debug.Log(user_status);
                            money_d = BitConverter.ToInt32(buff, i += 1);
                            FaceMainController.set_money_d(money_d);
                            money_z = BitConverter.ToInt32(buff, i += 4);
                            FaceMainController.set_money_z(money_z);
                            solo_w = BitConverter.ToInt32(buff, i += 4);
                            FaceMainController.set_solo_w(solo_w);
                            solo_f = BitConverter.ToInt32(buff, i += 4);
                            FaceMainController.set_solo_f(solo_f);
                            solo_s = BitConverter.ToInt16(buff, i += 4);
                            FaceMainController.set_solo_s(solo_s);
                            lv = BitConverter.ToInt16(buff, i += 2);
                            FaceMainController.set_lv(lv);
                        }
                        break;
                    default:
                        break;
                }
            }
            break;//跳出每次只处理一个item
        }

    }

    public void pushhash(Queue<string> queue, object lockht, string type, string value)
    {
        if (null == queue || null == lockht || null == type || null == value) {
            Debug.Log("pushhash NULL");
            return;
        }
        //加锁
        lock (lockht)
        {
            //ht.Add(type, value);
            queue.Enqueue(type + ":" + value);
        }
    }

    public int work(byte []buf, int len)
    {
        //服务器数据处理
        short work_tag = 0;
        int i = 0;
        //work_tag = (buf[1]);
        //work_tag = (short)((work_tag << 8) | buf[0]);
        work_tag = BitConverter.ToInt16(buf, i);
        i += 2;

        switch (work_tag)
        {
            case (short)WORK.REC_LOGIN:
                {
                    byte tag = buf[i++];
                    if (tag == 0)
                    {
                        pushhash(uiqueue, lockui, "login", "登录失败");
                    }
                    else
                    {
                        pushhash(uiqueue, lockui, "login", "登录成功");
                        byte[] ar = new byte[len - 3];
                        Array.Copy(buf, 3, ar, 0, len - 3);
                        pushhash(uiqueue, lockui, "showuserinfo", System.Text.Encoding.ASCII.GetString(ar));
                    }
                }
                break;
            case (short)WORK.REC_REGISTER:
                {
                    byte tag = buf[i++];
                    if (tag == 0)
                    {
                        pushhash(uiqueue, lockui, "Register", "注册失败");
                    }
                    else
                    {
                        pushhash(uiqueue, lockui, "Register", "注册成功");

                    }

                }
                break;
            default:
                break;
        }

        return 0;
    }

    #region  登录页面
    #region 账号密码登录页面
    #region 登录页面跳转到快速登录
    public void OnFastLogin()
    {
        
        loginPanelItween.PlayForward();
        StartCoroutine(HidePanel(loginPanelItween.gameObject));
        loginFastPanelItween.gameObject.SetActive(true);
        loginFastPanelItween.PlayForward();//正播动画
    }
    #endregion
    #region 登录页面通过登录到初始登录界面

    public static string username;
    public void OnLogin()
    {
        //得到的用户名和密码存储起来'
            string username = loginUsernameInput.value.Trim();
            string password = loginPasswordInput.value.Trim();
        Debug.Log(username);
        Debug.Log(password);
        if (username == null || password == null || "".Equals(username) || "".Equals(password))
        {
            Debug.Log("null");
            return;
        }
            string pass = UserMd5(password, 1);
            string userpass = username + ":" + pass;
            pushhash(dpqueue, lockdp, "login", userpass);
            LoginMainController.username = username;
    }

    public void Onlogin_agin(string username, string password)
    {
        //tdp();
        string userpass = username + ":" + password;
        pushhash(dpqueue, lockdp, "login", userpass);
        LoginMainController.username = username;
    }

    #endregion
    #region 账号、密码登录跳转到初始登录页面
    public void LoginFastClose()
    {
        loginFastPanelItween.PlayForward();
        StartCoroutine(HidePanel(loginFastPanelItween.gameObject));
        loginPanelItween.gameObject.SetActive(true);
        loginPanelItween.PlayReverse();//倒播动画
    }
    #endregion
    #endregion

    #region 注册页面
    #region 登录页面跳转到注册页面
    public void OnRegister()
    {
        loginFastPanelItween.PlayForward();
        StartCoroutine(HidePanel(loginFastPanelItween.gameObject));
        RegisterPanelItween.gameObject.SetActive(true);
        RegisterPanelItween.PlayForward();
    }
    #endregion
    #region 注册页面关闭跳转到快速登录页面
    public void OnRegisterClose()
    {
        RegisterPanelItween.PlayForward();
        StartCoroutine(HidePanel(RegisterPanelItween.gameObject));
        loginFastPanelItween.gameObject.SetActive(true);
        loginFastPanelItween.PlayReverse();
    }
    #endregion
    #region 注册页面注册并跳转到登录页面


    public static string register_username;
    public static string register_password;
    public void OnRegisterAndLogin()
    {
        string registerUsername = registerUsernameInput.value;
        string registerpassword1 = registerPasswordsInput.value;
        string registerpassword2 = registerPasswordsAgainInput.value;
        if (registerpassword1 != registerpassword2)
        {
            print("两次的密码输入不相同");
            string error = "两次输入的密码不相同";
            errorpassword.text = error;
            return;
        }
        else
        {
            //md5加密，密码
            string pass = UserMd5(registerpassword1, 1);
            LoginMainController.register_username = registerUsername;
            LoginMainController.register_password = pass;
            Debug.Log(register_username);
            Debug.Log(register_password);
            string registerpass = registerUsername + ":" + pass;
            pushhash(dpqueue, lockdp, "Register", registerpass);
            interfaceUsernameLabel.text = registerUsername;
        }
    }

    #endregion
    #endregion

    #region 手机登录页面
    #region 登录页面跳转到手机登录页面
    public void OnPhoneLogin()
    {
        loginPanelItween.PlayForward();
        StartCoroutine(HidePanel(loginPanelItween.gameObject));
        loginPhonePanelItween.gameObject.SetActive(true);
        loginPhonePanelItween.PlayForward();
    }
    #endregion
    #region 手机登录关闭跳转到
    public void LoginPhoneClose()
    {
        loginPhonePanelItween.PlayForward();
        StartCoroutine(HidePanel(loginPhonePanelItween.gameObject));
        loginPanelItween.gameObject.SetActive(true);
        loginPanelItween.PlayReverse();
    }
    #endregion
    #endregion

    #region 隐藏面板时间
    IEnumerator HidePanel(GameObject go)
    {
        yield return new WaitForSeconds(0.4f);
        go.SetActive(false);
    }
    #endregion

    #endregion

   #region 线程二处理数据
    public void tdp()
    {
        ClientSocket.clientsocket.BeginReceive(ClientSocket.Buffer, 0, ClientSocket.Buffer.Length, SocketFlags.None, new AsyncCallback(ClientSocket.ReceiveMessage), ClientSocket.clientsocket);
        ClientSocket.Work = work;
        string s = null;
        //OnLogin1();
        while (true)
        {
            Debug.Log("asdsd");
            //不能退出while
            //foreach (DictionaryEntry de in dpht)
            while (dpqueue.Count > 0)
            {
                lock (lockdp)
                {
                    s = dpqueue.Dequeue();
                }
                string []strs = s.Split(':');
                if (null == strs) continue;

                switch (strs[0])
                {
                    case "login":
                        {
                            User user = new User(strs[1], strs[2]);
                            user.login();
                        }
                        break;
                    case "Register":
                        {
                            Register register = new Register(strs[1], strs[2]);
                            register.Registers();
                        }
                        break;
                    default:
                        break;
                }
            }
            Thread.Sleep(500);
        }
    }
    #endregion

    static string UserMd5(string str, byte flag)
    {
        string cl = str;
        StringBuilder pwd = new StringBuilder();
        MD5 md5 = MD5.Create();//实例化一个md5对像
                               // 加密后是一个字节类型的数组，这里要注意编码UTF8/Unicode等的选择　
        byte[] s = md5.ComputeHash(Encoding.UTF8.GetBytes(cl));
        // 通过使用循环，将字节类型的数组转换为字符串，此字符串是常规字符格式化所得
        for (int i = 0; i < s.Length; i++)
        {
            // 将得到的字符串使用十六进制类型格式。格式后的字符是小写的字母，如果使用大写（X）则格式后的字符是大写字符
            if (flag == 1)
            {
                pwd.Append(s[i].ToString("X2"));
            }
            else
            {
                pwd.Append(s[i].ToString("x2"));
            }
            //pwd = pwd + s[i].ToString("X");

        }
        return pwd.ToString();
    }
}
