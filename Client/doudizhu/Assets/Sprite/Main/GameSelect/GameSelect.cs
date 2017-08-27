using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameSelect : MonoBehaviour {

    private UIButton classicGamePlayButton;

    private void Awake()
    {
        classicGamePlayButton = transform.Find("ClassicGame").GetComponent<UIButton>();
        EventDelegate ed = new EventDelegate(this, "OnClassicGameButtonClick");
        classicGamePlayButton.onClick.Add(ed);
    }

    public void OnClassicGameButtonClick()
    {
        classicGame._instance.Show();
    }
}
