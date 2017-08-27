using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoadScenceProgressBar : MonoBehaviour {

    public static LoadScenceProgressBar _instance;

    private GameObject renWuJiaZaiJinDuTiao;
    private UISlider progressBar;
    private bool isAsyn = false;
    private AsyncOperation ao = null;

    public void Awake()
    {
        _instance = this;
        renWuJiaZaiJinDuTiao = transform.Find("renwujiazaijindutiao").gameObject;
        progressBar = transform.Find("renwujiazaijindutiao/jindutiao_bg").GetComponent<UISlider>();
        gameObject.SetActive(false);
    }
    public void Update()
    {
        if (isAsyn)
        {
            progressBar.value = ao.progress;
        }
        
    }

    public void Show(AsyncOperation ao)
    {
        gameObject.SetActive(true);
        renWuJiaZaiJinDuTiao.SetActive(true);
        isAsyn = true;
        this.ao = ao;
    }
}
