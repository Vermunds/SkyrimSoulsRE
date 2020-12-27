Scriptname DLC2SummonDremoraMerchantScript extends ActiveMagicEffect  

ObjectReference Property DLC2DremoraRef  Auto  
ObjectReference Property DLC2DremoraMerchantMarker  Auto  
EffectShader Property ConjureEffect  Auto  
Scene Property DLC2DremoraScene Auto
GlobalVariable Property DLC2DremoraForceGreet  Auto

;; FX Added by Dan
Activator property ConjureEffectActivator Auto  
ImageSpaceModifier property ConjureImageSpace Auto

Event OnEffectStart(actor akTarget, actor akCaster)
	(DLC2DremoraRef as actor).StopCombat()
	DLC2DremoraRef.Moveto(game.GetPlayer())
	DLC2DremoraRef.placeAtMe(ConjureEffectActivator) ;; Added by Dan
	ConjureImageSpace.Apply()
	ConjureEffect.Play(DLC2DremoraRef, 1)
EndEvent

Event OnEffectFinish(actor akTarget, actor akCaster)
	While (Utility.IsInMenuMode())
		Utility.Wait(0.1)
	EndWhile
	ConjureEffect.Play(DLC2DremoraRef, 1)
	DLC2DremoraRef.Disable(true)
	utility.wait(1)	
	DLC2DremoraRef.MoveTo(DLC2DremoraMerchantMarker)
	(DLC2DremoraRef as actor).StopCombat()
	DLC2DremoraRef.Enable()
	DLC2DremoraForceGreet.SetValue(0)
EndEvent

