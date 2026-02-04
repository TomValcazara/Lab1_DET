using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Collider)), RequireComponent(typeof(AudioSource))]
public class TargetTriggerXR : MonoBehaviour
{
    public float uiDelay = 3f;
    public Collider marble;
    public TimingRecordingXR timingRecording;
    public TargetGroupWeightControl targetGroupWeightControl;
    public ParticleSystem completeParticleSystem;
    public WebSocketClientExample WebSocketClient;

    AudioSource m_AudioSource;

    void Awake ()
    {
        m_AudioSource = GetComponent<AudioSource> ();
    }

    void OnTriggerEnter (Collider other)
    {
        if (other == marble)
        {
            completeParticleSystem.Play();
            timingRecording.GoalReached (uiDelay);
//            targetGroupWeightControl.ApplySpecificFocus (marble.attachedRigidbody);
            m_AudioSource.PlayOneShot (m_AudioSource.clip);
            WebSocketClient.SendLEDWinON();
            
        }
    }
}
