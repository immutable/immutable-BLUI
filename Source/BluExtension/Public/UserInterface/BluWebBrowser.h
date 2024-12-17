#pragma once

#include "Blueprint/UserWidget.h"
#include "Widgets/SCompoundWidget.h"

#include "BluWebBrowser.generated.h"

class UBluEye;

class FBluCefWebBrowserWindow;
class IBluWebBrowserWindow;
class SBluWebBrowserView;

class BLUEXTENSION_API SBluWebBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBluWebBrowser) :
			_InitialURL(),
			_InitialWidth(INDEX_NONE),
			_InitialHeight(INDEX_NONE),
			_OnUrlChanged()
		{
		}

		/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
		SLATE_ARGUMENT(FString, InitialURL)

		SLATE_ARGUMENT(int32, InitialWidth)

		SLATE_ARGUMENT(int32, InitialHeight)

		/** Called when the Url changes. */
		SLATE_EVENT(FOnTextChanged, OnUrlChanged)

	SLATE_END_ARGS()

	virtual ~SBluWebBrowser() override;

	/** SWidget: @Interface @Begin */
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	/** SWidget: @Interface @End */

	void Construct(const FArguments& Args);

	void LoadURL(const FString& URL);

protected:
	/** A delegate that is invoked when document address changed. */
	FOnTextChanged OnUrlChanged;

	TSharedPtr<FBluCefWebBrowserWindow> BrowserWindow;
	TSharedPtr<SBluWebBrowserView> BrowserView;

	UBluEye* BluEye = nullptr;

	mutable FVector2D CachedWidgetSize;
};

UCLASS()
class BLUEXTENSION_API UBluWebBrowser : public UWidget
{
	GENERATED_BODY()

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

	TSharedPtr<SBluWebBrowser> S_WebBrowser;
};