#pragma once

#include "Blueprint/UserWidget.h"
#include "Widgets/SCompoundWidget.h"

#include "BluWebBrowser.generated.h"

class UBluEye;

class IBluWebBrowserWindow;
class SBluWebBrowserView;

class BLUEXTENSION_API SBluWebBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBluWebBrowser)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& Args, TSharedPtr<IBluWebBrowserWindow> BrowserWindow);

protected:
	TSharedPtr<SBluWebBrowserView> BrowserView;
};

UCLASS()
class BLUEXTENSION_API UBluWebBrowser : public UUserWidget
{
	GENERATED_BODY()

public:
	/** UWidget: @Interface @Begin */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	/** UWidget: @Interface @End */

	/** UUserWidget: @Interface @Begin */
	virtual void NativeOnInitialized() override;
	/** UUserWidget: @Interface @End */

	UFUNCTION(BlueprintPure)
	UBluEye* GetBluEye() const;

protected:
	/** UWidget: @Interface @Begin */
	virtual TSharedRef<SWidget> RebuildWidget() override;
	/** UWidget: @Interface @End */

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere)
	FString InitialURL;

	/** Width of the view resolution */
	UPROPERTY(EditAnywhere)
	int32 InitialWidth = INDEX_NONE;

	/** Height of the view resolution */
	UPROPERTY(EditAnywhere)
	int32 InitialHeight = INDEX_NONE;

	UPROPERTY(Transient)
	UBluEye* BluEye;

	TSharedPtr<SBluWebBrowser> S_WebBrowser;
};