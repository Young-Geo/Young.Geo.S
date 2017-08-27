using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PersonalInformationScript : MonoBehaviour {

    public static PersonalInformationScript _instance;

    private UIButton CloseButton;

    private TweenPosition tween;

     void Awake()
    {
        _instance = this;
        CloseButton = transform.Find("CloseButton").GetComponent<UIButton>();
        tween = this.GetComponent<TweenPosition>();
        EventDelegate ed = new EventDelegate(this, "OnButtonCloseClick");
        CloseButton.onClick.Add(ed);
    }

    public void Show()
    {
        tween.PlayForward();
    }

    public void OnButtonCloseClick()
    {
        tween.PlayReverse();
    }
}
