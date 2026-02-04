using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Playables;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class SceneCompletion : MonoBehaviour
{
    public ScreenFader screenFader;
    public GameObject panel;
    public WebSocketClientExample WebSocketClient;

    // Called when target of level is achieved
    public void CompleteLevel (float time)
    {
        panel.SetActive (true);
    }

    // UI Button
    public void ReloadLevel ()
    {
        screenFader.FadeOut(() => SceneManager.LoadSceneAsync (gameObject.scene.buildIndex));
        WebSocketClient.SendLEDWinOFF();
    }
}