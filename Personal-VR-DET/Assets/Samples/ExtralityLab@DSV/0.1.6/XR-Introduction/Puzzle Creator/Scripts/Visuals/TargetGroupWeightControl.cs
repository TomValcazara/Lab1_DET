using System.Collections;
using System.Collections.Generic;
using Unity.Cinemachine;
using UnityEngine;


public class TargetGroupWeightControl : MonoBehaviour
{
    [Header("Weight Control Settings")]
    public float weightDamping = 1f;
    public AnimationCurve speedToWeightCurve = new AnimationCurve(new Keyframe(0.1f, 0f), new Keyframe(1f, 1f, 3f, 3f));
    public AnimationCurve averageToFocusDistanceCurve = AnimationCurve.Linear (0f, 0f, 20f, 20f);
    public AnimationCurve rangeToApertureCurve = AnimationCurve.Linear (0f, 0.65f, 6f, 1f);

    Rigidbody FocusTarget;
    Transform Camera;
    CinemachineTargetGroup m_TargetGroup;
    Rigidbody[] m_TargetRigidbodies;

    void Awake ()
    {
        Camera = FindFirstObjectByType<Camera> ().transform;
        m_TargetGroup = GetComponent<CinemachineTargetGroup> ();

        for (int i = 0; i < m_TargetGroup.Targets.Count; i++)
        {
            m_TargetGroup.Targets[i].Weight = i == 0 ? 1f : 0f;
        }
        
        m_TargetRigidbodies = new Rigidbody[m_TargetGroup.Targets.Count];
        for (int i = 0; i < m_TargetRigidbodies.Length; i++)
        {
            m_TargetRigidbodies[i] = m_TargetGroup.Targets[i].Object.GetComponent<Rigidbody> ();
        }
    }

    void Update ()
    {
        for (int i = 0; i < m_TargetRigidbodies.Length; i++)
        {
            float weight;
            if (FocusTarget == null)
            {
                weight = speedToWeightCurve.Evaluate (m_TargetRigidbodies[i].linearVelocity.magnitude);
            }
            else
            {
                weight = m_TargetRigidbodies[i] == FocusTarget ? 1f : 0f;
            }
            weight = Mathf.Clamp01 (weight);
            m_TargetGroup.Targets[i].Weight = Mathf.MoveTowards (m_TargetGroup.Targets[i].Weight, weight, weightDamping * Time.deltaTime);
        }
    }

    public void ApplySpecificFocus (Rigidbody focusTarget)
    {
        FocusTarget = focusTarget;
    }
}
