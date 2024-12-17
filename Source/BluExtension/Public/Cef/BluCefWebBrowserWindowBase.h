#pragma once

#include "Interface/BluWebBrowserWindow.h"

THIRD_PARTY_INCLUDES_START
#include <include/internal/cef_ptr.h>
#include <include/internal/cef_types_wrappers.h>
THIRD_PARTY_INCLUDES_END

class CefBrowser;

class UBluEye;

/**
* Representation of a window drag region.
*/
struct BLUEXTENSION_API FWebBrowserDragRegion
{
	FWebBrowserDragRegion(const FIntRect& InRect, bool bInDraggable);

	FIntRect Rect;
	bool bDraggable;
};

class BLUEXTENSION_API FBluCefWebBrowserWindowBase : public TSharedFromThis<FBluCefWebBrowserWindowBase>, public IBluWebBrowserWindow
{
public:
	DECLARE_DERIVED_EVENT(FBluCefWebBrowserWindowBase, IBluWebBrowserWindow::FOnToolTip, FOnToolTip);

public:
	/**
	 * Gets the Cef Keyboard Modifiers based on a Key Event.
	 *
	 * @param KeyEvent The Key event.
	 * @return Bits representing keyboard modifiers.
	 */
	static int32 GetCefKeyboardModifiers(const FKeyEvent& KeyEvent);

	/**
	 * Gets the Cef Mouse Modifiers based on a Mouse Event.
	 *
	 * @param InMouseEvent The Mouse event.
	 * @return Bits representing mouse modifiers.
	 */
	static int32 GetCefMouseModifiers(const FPointerEvent& InMouseEvent);

	/**
	 * Gets the Cef Input Modifiers based on an Input Event.
	 *
	 * @param InputEvent The Input event.
	 * @return Bits representing input modifiers.
	 */
	static int32 GetCefInputModifiers(const FInputEvent& InputEvent);

	/**
	 * Is this platform able to support the accelerated paint path for CEF. 
	 *
	 * @return true if supported AND enabled on this platform, false otherwise.
	 */
	static bool CanSupportAcceleratedPaint();

public:
	FBluCefWebBrowserWindowBase(CefRefPtr<CefBrowser> NewCefBrowser);
	virtual ~FBluCefWebBrowserWindowBase() override;

	/** IBluWebBrowserWindow: @Interface @Begin */
	virtual bool IsValid() const override;
	virtual FIntPoint GetViewportSize() const override;
	virtual FSlateShaderResource* GetTexture(bool bIsPopup = false) override;
	virtual TSharedPtr<SWindow> GetParentWindow() const override;
	virtual bool CanGoBack() const override;
	virtual bool CanGoForward() const override;

	virtual FOnToolTip& OnToolTip() override;
	virtual FOnUrlChanged& OnUrlChanged() override;
	virtual FOnDragWindow& OnDragWindow() override;

	virtual void SetViewportSize(FIntPoint WindowSize, FIntPoint WindowPos = FIntPoint::NoneValue) override;
	virtual void SetParentWindow(TSharedPtr<SWindow> Window) override;

	virtual TSharedPtr<SViewport> CreateWidget() override;

	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) override;
	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent, bool bIsPopup) override;

	virtual void GoBack() override;
	virtual void GoForward() override;
	/** IBluWebBrowserWindow: @Interface @End */

protected:
	/** Used to convert a FPointerEvent to a CefMouseEvent. */
	CefMouseEvent GetCefMouseEvent(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup);

	/** Specifies whether or not a point falls within any tagged drag regions that are draggable. */
	bool IsInDragRegion(const FIntPoint& Point);

	void SetToolTip(const CefString& InToolTip);

	/** Helper that calls WasHidden on the CEF host object when the value changes */
	void SetIsHidden(bool bValue);

	/** Used by the key down and up handlers to convert Slate key events to the CEF equivalent. */
	void PopulateCefKeyEvent(const FKeyEvent& InKeyEvent, CefKeyEvent& OutKeyEvent);

	bool BlockInputInDirectHwndMode() const;

protected:
	/** Current tool tip. */
	FString ToolTipText;

	/** Current size of this window. */
	FIntPoint ViewportSize;

	/** Current position of this window. */
	FIntPoint ViewportPos;

	/** Current DPI scale factor of this window. */
	float ViewportDPIScaleFactor;

	/** Delegate for showing or hiding tool tips. */
	FOnToolTip ToolTipEvent;

	/** Delegate for broadcasting address changes. */
	FOnUrlChanged UrlChangedEvent;

	/** Delegate that is executed when a drag event is detected in an area of the web page tagged as a drag region. */
	FOnDragWindow DragWindowDelegate;

	/** Pointer to the CEF Browser for this window. */
	CefRefPtr<CefBrowser> InternalCefBrowser;

	/** Whether to allow forward and back navigation via the mouse thumb buttons. */
	bool bThumbMouseButtonNavigation;

	/** Tracks whether the widget is currently disabled or not*/
	bool bIsDisabled;

	/** Tracks whether the widget is currently hidden or not*/
	bool bIsHidden;

	/** Used to detect when the widget is hidden*/
	bool bTickedLastFrame;

	/** Tracks whether the widget has been resized and needs to be refreshed */
	bool bNeedsResize;

	/** Tracks whether or not the user initiated a window drag by clicking on a page's drag region. */
	bool bDraggingWindow;

	/** Used for unhandled key events forwarding. */
	TOptional<FCharacterEvent> PreviousCharacterEvent;
	TOptional<FKeyEvent> PreviousKeyDownEvent;
	TOptional<FKeyEvent> PreviousKeyUpEvent;
	bool bIgnoreCharacterEvent;
	bool bIgnoreKeyDownEvent;
	bool bIgnoreKeyUpEvent;

	bool bSupportsMouseWheel;

	FIntPoint PopupPosition;

	TArray<FWebBrowserDragRegion> DragRegions;

	TWeakPtr<SWindow> ParentWindow;

#if PLATFORM_WINDOWS || PLATFORM_MAC
	bool bInDirectHwndMode;
#endif
};