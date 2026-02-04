using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FlipperXR : InteractivePuzzlePieceXR<HingeJoint>
{
    [Range(0f, 4096f)]
    public float power = 800f;

    void Awake ()
    {
        JointMotor flipperMotor = physicsComponent.motor;
        flipperMotor.targetVelocity = power;
        physicsComponent.motor = flipperMotor;
    }
    
    public void SetPower(float newPower)
    {
        //power = newPower;
        power = Mathf.Clamp(newPower, 0f, 4096f);
        JointMotor motor = physicsComponent.motor;
        motor.targetVelocity = power;
        physicsComponent.motor = motor;
    }

    protected override void ApplyActiveState ()
    {
        physicsComponent.useMotor = true;
    }

    protected override void ApplyInactiveState ()
    {
        physicsComponent.useMotor = false;
    }
}
