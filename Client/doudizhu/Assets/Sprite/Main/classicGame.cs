using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class classicGame : MonoBehaviour {

    public static classicGame _instance;

    private UIButton ExitButton;

    private TweenPosition tween;

     void Awake()
    {
        _instance = this;
        ExitButton = transform.Find("ExitButton").GetComponent<UIButton>();
        tween = this.GetComponent<TweenPosition>();
        EventDelegate ed = new EventDelegate(this, "OnButtonExitclik");
        ExitButton.onClick.Add(ed);
    }

    public void Show()
    {
        tween.PlayForward();
    }

    public void OnButtonExitclik()
    {
        tween.PlayReverse();
    }
}
