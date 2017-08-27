using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class test : MonoBehaviour {

    //将准备好的MP3格式的背景声音文件拖入此处

    public AudioClip backgroundMusic;



    //将准备好的MP3格式的音效文件拖入此处

    public AudioClip palyOnceSound;



    //按钮音效

    public AudioClip buttonSound;

    //用于控制声音的AudioSource组件

    private AudioSource _audioSource;



    //是否播放游戏背景音乐

    private bool isPlayMusic;

    //是否播放按键音效

    private bool isPlayButtonSound;



    void Awake()

    {

        //在添加此脚本的对象中添加AudioSource组件，此处为摄像机

        _audioSource = this.gameObject.AddComponent<AudioSource>();

        //设置循环播放

        _audioSource.loop = true;

        //设置音量为最大，区间在0-1之间

        _audioSource.volume = 1.0f;

        //设置audioClip

        _audioSource.clip = backgroundMusic;

    }



    void Update()

    {

        //如果isPlayMusic为false,则暂停播放背景音乐

        if (isPlayMusic == false) _audioSource.Pause();

    }



    void OnGUI()

    {

        //绘制播放按钮并设置其点击后的处理

        if (GUI.Button(new Rect(10, 10, 80, 30), "Play"))

        {

            //播放声音

            if (isPlayMusic) _audioSource.Play();

            AddBtnSound();

        }



        //绘制暂停按钮并设置其点击后的处理

        if (GUI.Button(new Rect(10, 50, 80, 30), "Pause"))

        {

            //暂停声音，暂停后再播放，则声音为继续播放

            _audioSource.Pause();

            AddBtnSound();

        }



        //绘制停止按钮并设置其点击后的处理

        if (GUI.Button(new Rect(10, 90, 80, 30), "Stop"))

        {

            //停止播放，停止后再播放，则声音为从头播放

            _audioSource.Stop();

            AddBtnSound();

        }



        //绘制添加音效按钮,PlayOnShot方式添加音效

        if (GUI.Button(new Rect(100, 10, 150, 30), "AddSound_Method_1"))

        {

            _audioSource.PlayOneShot(palyOnceSound);

            AddBtnSound();

        }



        //绘制添加音效按钮，PlayClipAtPoint方式添加音效

        if (GUI.Button(new Rect(100, 50, 150, 30), "AddSound_Method_2"))

        {

            AudioSource.PlayClipAtPoint(palyOnceSound, _audioSource.transform.localPosition);

            AddBtnSound();

        }



        //音量控制Label

        GUI.Label(new Rect(10, 130, 100, 30), "音量大小调节");

        //音量控制slider

        _audioSource.volume = GUI.HorizontalSlider(new Rect(120, 130, 100, 20), _audioSource.volume, 0.0f, 1.0f);



        //选择是否播放背景音乐

        isPlayMusic = GUI.Toggle(new Rect(10, 170, 100, 20), isPlayMusic, "背景音乐");

        //选择是否播放按键声音

        isPlayButtonSound = GUI.Toggle(new Rect(120, 170, 100, 20), isPlayButtonSound, "按钮音效");



    }



    //添加按键声音

    private void AddBtnSound()

    {

        if (isPlayButtonSound) AudioSource.PlayClipAtPoint(buttonSound, _audioSource.transform.localPosition);

    }

}
