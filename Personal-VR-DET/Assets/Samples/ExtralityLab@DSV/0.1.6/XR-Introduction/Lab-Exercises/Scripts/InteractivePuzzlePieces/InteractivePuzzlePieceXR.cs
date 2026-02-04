using UnityEngine;
using UnityEngine.InputSystem;

public abstract class InteractivePuzzlePieceXR<TComponent> : BaseInteractivePuzzlePieceXR
where TComponent : Component
{
    public TComponent physicsComponent;
}


public abstract class BaseInteractivePuzzlePieceXR : MonoBehaviour
{
    //public KeyCode interactKey = KeyCode.Space;
    public InputActionReference interactActionReference;

    public Rigidbody rb;
    public AudioClip activateSound;
    public AudioClip deactivateSound;
    public AudioSource puzzleAudioSource;

    bool m_IsControllable;

    public bool activateState = false; // Activates the object
    public bool playOneTimeActivateAudio = false; // Audio for one-time activation audio
    public bool playOneTimeDeactivateAudio = false; // Audio for one-time deactivation audio

    public bool esp32Controlled;
    
    void Update()
    {
        if (m_IsControllable)
        {
            //if(Input.GetKey(interactKey))
            // if(interactActionReference.action.IsPressed())
            //if(interactActionReference.action.IsPressed() || esp32Controlled && m_IsControlable)
            if(interactActionReference.action.IsPressed() || esp32Controlled)
            {
                activateState = true;
            }
            else
            {
                activateState = false;
            }


            //if(Input.GetKeyDown(interactKey) )
            if (interactActionReference.action.WasPressedThisFrame())
            {
                playOneTimeActivateAudio = true;
            }

            //if(Input.GetKeyUp(interactKey) )
            if(interactActionReference.action.WasReleasedThisFrame())
            {
                playOneTimeDeactivateAudio = true;
                activateState = false;
            }
        }

        // Check variable states to execute the actions
        if (activateState)
        {
            ApplyActiveState();
        }
        else
        {
            ApplyInactiveState();
        }

        if (playOneTimeActivateAudio)
        {
            PlayOneTimeActivateAudio();
        }

        if (playOneTimeDeactivateAudio)
        {
            PlayOneTimeDeactivateAudio();
        }
    }

    protected abstract void ApplyActiveState ();

    protected abstract void ApplyInactiveState ();

    public void PlayOneTimeActivateAudio()
    {
        playOneTimeActivateAudio = false;
        if(activateSound == null) return;
        puzzleAudioSource.pitch = Random.Range(0.8f, 1.2f);
        puzzleAudioSource.PlayOneShot(activateSound);
    }

    public void PlayOneTimeDeactivateAudio()
    {
        playOneTimeDeactivateAudio = false;
        if ( activateSound == null) return;
        puzzleAudioSource.pitch = Random.Range(0.8f, 1.2f);
        puzzleAudioSource.PlayOneShot(deactivateSound);
    }

    public void EnableControl ()
    {
        m_IsControllable = true;
    }
}