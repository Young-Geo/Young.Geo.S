using Assets;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class AgainLink : MonoBehaviour {


    public void againLink()
    {
        ClientSocket.ConnectServer("192.168.1.9", 8080);
        if (false == ClientSocket.IsConnected)
        {
            Debug.Log("cuowu");
        }
        else
        {
            SceneManager.LoadScene("login");
        }
    }

}
