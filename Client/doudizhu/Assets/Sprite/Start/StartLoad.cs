using Assets;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class StartLoad : MonoBehaviour {


    string ip = "192.168.1.9";    
    void Start () {

        ClientSocket.ConnectServer(ip, 8080);
        if (false == ClientSocket.IsConnected)
        {
            SceneManager.LoadScene("TryAgainServerOrStand-aloneGame");
            Debug.Log("3");
        }
        else
        {
            SceneManager.LoadScene("login");
            Debug.Log("2");
        }
    }
	
}
