using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FaceMainController : MonoBehaviour
{
    public UILabel lab_username; //用户昵称
    public UILabel lab_id;       //用户id
    public UILabel lab_user_status;   //角色状态
    public UILabel lab_money_d;  //豆
    public UILabel lab_money_z;  //钻石
    public UILabel lab_solo_w;   //对战赢局
    public UILabel lab_solo_f;   //对战输局
    public UILabel lab_solo_s;   //对战连胜
    public UILabel lab_lv;       //角色等级

    public UILabel lab_username1;
    public UILabel lab_money_d2;
    public UILabel lab_lv1;
    public UILabel lab_solo_z;

    private static int id;
    private static int user_status;
    private static int money_d;
    private static int money_z;
    private static int solo_w;
    private static int solo_f;
    private static int solo_s;
    private static int lv;


    public static void set_id(int idd)
    {
        id = idd;
    }

    public static void set_user_status(int user_statuss)
    {
        user_status = user_statuss;
    }

    public static void set_money_d(int money_dd)
    {
        money_d = money_dd;
    }

    public static void set_money_z(int money_zz)
    {
        money_z = money_zz;
    }

    public static void set_solo_w(int solo_ww)
    {
        solo_w = solo_ww;
    }

    public static void set_solo_f(int solo_ff)
    {
        solo_f = solo_ff;
    }
    public static void set_solo_s(int solo_ss)
    {
        solo_s = solo_ss;
    }
    public static void set_lv(int lvv)
    {
        lv = lvv;
    }




    public void Update()
    {
        lab_username.text = LoginMainController.username;
        lab_username1.text = LoginMainController.username;

        lab_id.text = id.ToString();
        lab_user_status.text = user_status.ToString();
        lab_money_d.text = money_d.ToString();
        lab_money_z.text = money_z.ToString();
        lab_solo_w.text = solo_w.ToString();
        lab_solo_f.text = solo_f.ToString();
        lab_solo_s.text = solo_s.ToString();
        lab_lv.text = lv.ToString();

        lab_money_d2.text = money_d.ToString();
        lab_lv1.text = lv.ToString();

        string solo_z = solo_w.ToString() + solo_f.ToString();
        lab_solo_z.text = solo_z.ToString();
    }
}
