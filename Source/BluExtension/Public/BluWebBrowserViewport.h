#pragma once

class IBluWebBrowserWindow;

class BLUEXTENSION_API FBluWebBrowserViewport : public ISlateViewport
{
public:
	FBluWebBrowserViewport(TSharedPtr<IBluWebBrowserWindow> NewWebBrowserWindow, bool bNewIsPopup = false);

	/** ISlateViewport: @Interface @Begin */
	virtual FIntPoint GetSize() const override;
	virtual FSlateShaderResource* GetViewportRenderTargetTexture() const override;
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) override;
	virtual bool RequiresVsync() const override;
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent) override;
	/** ISlateViewport: @Interface @End */

protected:
	TSharedPtr<IBluWebBrowserWindow> WebBrowserWindow;
	bool bIsPopup = false;
};