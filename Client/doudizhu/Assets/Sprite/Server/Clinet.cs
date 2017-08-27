using Assets;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class Clinet : MonoBehaviour {



    public  string ip = "192.168.1.9";
    public  int port = 8080;
    
	// Use this for initialization
	void Start ()
    {
        ClientSocket.ConnectServer(ip, 8080);
        if (false == ClientSocket.IsConnected) {
            Debug.Log("1");
        }
        else
        {
            Debug.Log("2");
        }
    }


	
	// Update is called once per frame
	void Update () {

        

	}
}
