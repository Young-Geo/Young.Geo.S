using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BgUp : MonoBehaviour {

    private UIButton AvatarButton;

     void Awake()
    {
        AvatarButton = transform.Find("AvatarImageButton").GetComponent<UIButton>();
        EventDelegate ed = new EventDelegate(this, "OnAvatarButtonClick");
        AvatarButton.onClick.Add(ed);
    }

    public void OnAvatarButtonClick()
    {
        PersonalInformationScript._instance.Show();
    }
}
